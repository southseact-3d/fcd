#include "Timeline.h"
#include "Application.h"
#include "Document.h"
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/Transactions.h>
#include <Base/Console.h>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QStyleOptionSlider>
#include <QToolTip>
#include <QAction>
#include <QMessageBox>

using namespace Gui;

TimelineWidgetItem::TimelineWidgetItem(const TimelineOperation& op, QWidget* parent)
    : QToolButton(parent)
    , m_operation(op)
    , m_isCurrent(false)
{
    setFixedSize(48, 48);
    setIconSize(QSize(32, 32));
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setToolTip(QString::fromStdString(op.name));
    setIcon(op.icon);
    setAutoRaise(true);
}

void TimelineWidgetItem::setIcon(const QIcon& icon)
{
    QToolButton::setIcon(icon);
}

void TimelineWidgetItem::setCurrent(bool current)
{
    m_isCurrent = current;
    if (current) {
        setStyleSheet(
            QStringLiteral("QToolButton { background-color: #3498db; "
                          "border: 2px solid #2980b9; border-radius: 4px; }")
        );
    }
    else {
        setStyleSheet(
            QStringLiteral("QToolButton { background-color: transparent; "
                          "border: 1px solid #555; border-radius: 4px; } "
                          "QToolButton:hover { background-color: #444; }")
        );
    }
}

void TimelineWidgetItem::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);

    QAction* editAction = menu.addAction(tr("Edit"));
    editAction->setIcon(QIcon::fromTheme(QStringLiteral("document-edit")));

    menu.addSeparator();

    QAction* deleteAction = menu.addAction(tr("Delete"));
    deleteAction->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));

    connect(editAction, &QAction::triggered, this, [this]() {
        Q_EMIT editRequested(m_operation);
    });

    connect(deleteAction, &QAction::triggered, this, [this]() {
        Q_EMIT deleteRequested(m_operation);
    });

    menu.exec(event->globalPos());
}

void TimelineWidgetItem::mouseDoubleClickEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        Q_EMIT editRequested(m_operation);
    }
    QToolButton::mouseDoubleClickEvent(event);
}

TimelineSlider::TimelineSlider(Qt::Orientation orientation, QWidget* parent)
    : QSlider(orientation, parent)
{
    setTickPosition(QSlider::TicksBelow);
    setTickInterval(1);
    setSingleStep(1);
    setPageStep(1);
    setTracking(true);
}

void TimelineSlider::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        QStyleOptionSlider opt;
        initStyleOption(&opt);
        QRect sliderRect = style()
                             ->subControlRect(QStyle::CC_Slider, &opt,
                                              QStyle::SC_SliderHandle, this);

        if (!sliderRect.contains(event->pos())) {
            int val = minimum() + ((maximum() - minimum()) * event->pos().x()) / width();
            setValue(val);
            Q_EMIT sliderMovedToPosition(val);
        }
    }
    QSlider::mousePressEvent(event);
}

void TimelineSlider::mouseMoveEvent(QMouseEvent* event)
{
    QSlider::mouseMoveEvent(event);
}

Timeline::Timeline(QWidget* parent)
    : QWidget(parent)
    , m_document(nullptr)
    , m_currentPosition(0)
    , m_isDragging(false)
{
    setupUI();
    loadOperationIcons();
}

Timeline::~Timeline() = default;

void Timeline::setupUI()
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);

    auto controlLayout = new QHBoxLayout();
    controlLayout->setSpacing(4);

    m_btnStart = new QToolButton(this);
    m_btnStart->setIcon(QIcon::fromTheme(QStringLiteral("media-skip-backward")));
    m_btnStart->setToolTip(tr("Go to start"));
    m_btnStart->setAutoRaise(true);
    m_btnStart->setFixedSize(28, 28);
    connect(m_btnStart, &QToolButton::clicked, this, &Timeline::goToStart);

    m_btnPrev = new QToolButton(this);
    m_btnPrev->setIcon(QIcon::fromTheme(QStringLiteral("media-seek-backward")));
    m_btnPrev->setToolTip(tr("Previous operation"));
    m_btnPrev->setAutoRaise(true);
    m_btnPrev->setFixedSize(28, 28);
    connect(m_btnPrev, &QToolButton::clicked, this, &Timeline::goToPrevious);

    m_slider = new TimelineSlider(Qt::Horizontal, this);
    m_slider->setRange(0, 0);
    m_slider->setValue(0);
    m_slider->setMinimumWidth(100);
    connect(m_slider, &QSlider::sliderMoved, this, &Timeline::onSliderMoved);
    connect(m_slider, &QSlider::sliderReleased, this, &Timeline::onSliderReleased);

    m_btnNext = new QToolButton(this);
    m_btnNext->setIcon(QIcon::fromTheme(QStringLiteral("media-seek-forward")));
    m_btnNext->setToolTip(tr("Next operation"));
    m_btnNext->setAutoRaise(true);
    m_btnNext->setFixedSize(28, 28);
    connect(m_btnNext, &QToolButton::clicked, this, &Timeline::goToNext);

    m_btnEnd = new QToolButton(this);
    m_btnEnd->setIcon(QIcon::fromTheme(QStringLiteral("media-skip-forward")));
    m_btnEnd->setToolTip(tr("Go to end"));
    m_btnEnd->setAutoRaise(true);
    m_btnEnd->setFixedSize(28, 28);
    connect(m_btnEnd, &QToolButton::clicked, this, &Timeline::goToEnd);

    m_positionLabel = new QLabel(this);
    m_positionLabel->setText(QStringLiteral("0 / 0"));
    m_positionLabel->setMinimumWidth(60);

    controlLayout->addWidget(m_btnStart);
    controlLayout->addWidget(m_btnPrev);
    controlLayout->addWidget(m_slider, 1);
    controlLayout->addWidget(m_btnNext);
    controlLayout->addWidget(m_btnEnd);
    controlLayout->addWidget(m_positionLabel);

    m_operationsScroll = new QScrollArea(this);
    m_operationsScroll->setWidgetResizable(true);
    m_operationsScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_operationsScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_operationsScroll->setFixedHeight(60);
    m_operationsScroll->setFrameShape(QFrame::NoFrame);

    m_operationsContainer = new QWidget();
    m_operationsLayout = new QHBoxLayout(m_operationsContainer);
    m_operationsLayout->setContentsMargins(4, 4, 4, 4);
    m_operationsLayout->setSpacing(4);
    m_operationsLayout->addStretch();

    m_operationsScroll->setWidget(m_operationsContainer);

    mainLayout->addLayout(controlLayout);
    mainLayout->addWidget(m_operationsScroll);

    setStyleSheet(QStringLiteral(
        "Timeline { background-color: #2d2d2d; border-top: 1px solid #444; }"
        "QToolButton { color: #ddd; }"
        "QLabel { color: #ddd; }"
        "QSlider::groove:horizontal { background: #444; height: 8px; border-radius: 4px; }"
        "QSlider::handle:horizontal { background: #3498db; width: 16px; margin: -4px 0; "
        "border-radius: 8px; }"
        "QSlider::sub-page:horizontal { background: #2980b9; border-radius: 4px; }"
    ));

    setMinimumHeight(100);
    setMaximumHeight(120);
}

void Timeline::loadOperationIcons()
{
    m_operationIcons["Sketch"] = QIcon::fromTheme(QStringLiteral("sketch"));
    m_operationIcons["Pad"] = QIcon::fromTheme(QStringLiteral("PartDesign_Pad"));
    m_operationIcons["Pocket"] = QIcon::fromTheme(QStringLiteral("PartDesign_Pocket"));
    m_operationIcons["Hole"] = QIcon::fromTheme(QStringLiteral("PartDesign_Hole"));
    m_operationIcons["Fillet"] = QIcon::fromTheme(QStringLiteral("PartDesign_Fillet"));
    m_operationIcons["Chamfer"] = QIcon::fromTheme(QStringLiteral("PartDesign_Chamfer"));
    m_operationIcons["Revolution"] = QIcon::fromTheme(QStringLiteral("PartDesign_Revolution"));
    m_operationIcons["Groove"] = QIcon::fromTheme(QStringLiteral("PartDesign_Groove"));
    m_operationIcons[" Loft"] = QIcon::fromTheme(QStringLiteral("PartDesign_Loft"));
    m_operationIcons["Sweep"] = QIcon::fromTheme(QStringLiteral("PartDesign_Sweep"));
    m_operationIcons["Thickness"] = QIcon::fromTheme(QStringLiteral("PartDesign_Thickness"));
    m_operationIcons["Mirror"] = QIcon::fromTheme(QStringLiteral("PartDesign_Mirrored"));
    m_operationIcons["LinearPattern"] =
        QIcon::fromTheme(QStringLiteral("PartDesign_LinearPattern"));
    m_operationIcons["PolarPattern"] =
        QIcon::fromTheme(QStringLiteral("PartDesign_PolarPattern"));
    m_operationIcons["MultiTransform"] =
        QIcon::fromTheme(QStringLiteral("PartDesign_MultiTransform"));
    m_operationIcons["Boolean"] = QIcon::fromTheme(QStringLiteral("Part_Boolean"));
    m_operationIcons["Fuse"] = QIcon::fromTheme(QStringLiteral("Part_Fuse"));
    m_operationIcons["Common"] = QIcon::fromTheme(QStringLiteral("Part_Common"));
    m_operationIcons["Cut"] = QIcon::fromTheme(QStringLiteral("Part_Cut"));
    m_operationIcons["Import"] = QIcon::fromTheme(QStringLiteral("Part_Import"));
    m_operationIcons["default"] = QIcon::fromTheme(QStringLiteral("document-new"));
}

QIcon Timeline::getIconForOperation(const std::string& type) const
{
    auto it = m_operationIcons.find(type);
    if (it != m_operationIcons.end()) {
        return it->second;
    }
    return m_operationIcons.at("default");
}

void Timeline::setDocument(App::Document* doc)
{
    if (m_document == doc) {
        return;
    }

    m_document = doc;
    refresh();
}

void Timeline::refresh()
{
    updateOperationsList();
    updateSliderRange();
}

void Timeline::clear()
{
    m_operations.clear();
    for (auto* widget : m_operationWidgets) {
        delete widget;
    }
    m_operationWidgets.clear();
    m_slider->setRange(0, 0);
    m_positionLabel->setText(QStringLiteral("0 / 0"));
}

void Timeline::updateOperationsList()
{
    for (auto* widget : m_operationWidgets) {
        delete widget;
    }
    m_operationWidgets.clear();
    m_operations.clear();

    if (!m_document) {
        return;
    }

    const std::vector<App::DocumentObject*>& objects = m_document->getObjects();

    int index = 0;
    for (auto* obj : objects) {
        if (!obj || !obj->isAttachedToDocument()) {
            continue;
        }

        std::string objType = obj->getTypeId().getName();

        TimelineOperation op;
        op.id = obj->getNameInDocument();
        op.name = obj->Label.getStrValue();
        op.type = objType;
        op.icon = getIconForOperation(objType);
        op.index = index;
        op.isCurrent = (index == m_currentPosition);
        op.documentName = m_document->getName();
        op.objectName = obj->getNameInDocument();

        m_operations.push_back(op);

        auto* widget = new TimelineWidgetItem(op, m_operationsContainer);
        widget->setCurrent(op.isCurrent);
        connect(widget, &TimelineWidgetItem::editRequested, this,
                &Timeline::onOperationEdited);
        connect(widget, &TimelineWidgetItem::deleteRequested, this,
                &Timeline::onOperationDeleted);

        m_operationsLayout->insertWidget(index, widget);
        m_operationWidgets.push_back(widget);

        index++;
    }

    m_operationsLayout->addStretch();

    if (m_currentPosition >= index) {
        m_currentPosition = std::max(0, index - 1);
    }

    updateSliderRange();
}

void Timeline::updateSliderRange()
{
    int maxPos = std::max(0, static_cast<int>(m_operations.size()) - 1);
    m_slider->setRange(0, maxPos);
    m_slider->setValue(m_currentPosition);
    m_positionLabel->setText(
        QString::number(m_currentPosition + 1) + QStringLiteral(" / ")
        + QString::number(m_operations.size())
    );
}

int Timeline::currentPosition() const
{
    return m_currentPosition;
}

void Timeline::setCurrentPosition(int position)
{
    if (position < 0 || position >= static_cast<int>(m_operations.size())) {
        return;
    }

    m_currentPosition = position;
    m_slider->setValue(position);

    for (size_t i = 0; i < m_operationWidgets.size(); ++i) {
        m_operationWidgets[i]->setCurrent(i == static_cast<size_t>(position));
    }

    m_positionLabel->setText(
        QString::number(position + 1) + QStringLiteral(" / ")
        + QString::number(m_operations.size())
    );

    if (!m_isDragging) {
        Q_EMIT positionChanged(position);
    }
}

int Timeline::operationCount() const
{
    return static_cast<int>(m_operations.size());
}

void Timeline::goToStart()
{
    if (m_operations.empty()) {
        return;
    }
    setCurrentPosition(0);
}

void Timeline::goToEnd()
{
    if (m_operations.empty()) {
        return;
    }
    setCurrentPosition(static_cast<int>(m_operations.size()) - 1);
}

void Timeline::goToPrevious()
{
    if (m_currentPosition > 0) {
        setCurrentPosition(m_currentPosition - 1);
    }
}

void Timeline::goToNext()
{
    if (m_currentPosition < static_cast<int>(m_operations.size()) - 1) {
        setCurrentPosition(m_currentPosition + 1);
    }
}

void Timeline::onOperationEdited(const TimelineOperation& op)
{
    Base::Console().message("Edit operation: %s\n", op.name.c_str());

    if (m_document) {
        App::DocumentObject* obj = m_document->getObject(op.objectName.c_str());
        if (obj) {
            Gui::Document* guiDoc = Gui::Application::Instance->getDocument(m_document);
            if (guiDoc) {
                Gui::ViewProvider* vp = guiDoc->getViewProvider(obj);
                if (vp)
                    guiDoc->setEdit(vp);
            }
        }
    }
}

void Timeline::onOperationDeleted(const TimelineOperation& op)
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Delete Operation"),
        tr("Are you sure you want to delete '%1'?").arg(QString::fromStdString(op.name)),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (m_document) {
            m_document->openTransaction("Delete timeline operation");
            App::DocumentObject* obj = m_document->getObject(op.objectName.c_str());
            if (obj) {
                m_document->removeObject(op.objectName.c_str());
            }
            m_document->commitTransaction();
            m_document->recompute();
            refresh();
        }
    }
}

void Timeline::onSliderMoved(int position)
{
    m_isDragging = true;
    setCurrentPosition(position);

    if (position >= 0 && position < static_cast<int>(m_operations.size())) {
        QToolTip::showText(
            QCursor::pos(),
            QString::fromStdString(m_operations[position].name),
            this
        );
    }
}

void Timeline::onSliderReleased()
{
    m_isDragging = false;
    Q_EMIT positionChanged(m_currentPosition);
}

void Timeline::applyStateAtPosition(int position)
{
    if (!m_document || position < 0
        || position >= static_cast<int>(m_operations.size())) {
        return;
    }
}

void Timeline::restoreFullState()
{
    if (!m_document) {
        return;
    }
    m_document->recompute();
}

#include "moc_Timeline.cpp"
