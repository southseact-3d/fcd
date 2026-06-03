/***************************************************************************
 *   Copyright (c) 2025 Tungsten CAD Project                              *
 *                                                                         *
 *   This file is part of the Tungsten CAD development system.             *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include "Timeline.h"
#include "Application.h"
#include "Document.h"
#include "Command.h"
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/Transactions.h>
#include <App/SuppressibleExtension.h>
#include <Base/Console.h>

#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QStyleOptionSlider>
#include <QToolTip>
#include <QAction>
#include <QMessageBox>
#include <QMimeData>
#include <QDrag>
#include <QPainter>

using namespace Gui;

// ==================== TimelineWidgetItem ====================

TimelineWidgetItem::TimelineWidgetItem(const TimelineOperation& op, QWidget* parent)
    : QToolButton(parent)
    , m_operation(op)
    , m_isCurrent(false)
    , m_isSuppressed(op.isSuppressed)
{
    setFixedSize(48, 48);
    setIconSize(QSize(32, 32));
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setToolTip(QString::fromStdString(op.name));
    setIcon(op.icon);
    setAutoRaise(true);
    setAcceptDrops(true);

    updateStyle();
}

void TimelineWidgetItem::setIcon(const QIcon& icon)
{
    QToolButton::setIcon(icon);
}

void TimelineWidgetItem::setCurrent(bool current)
{
    m_isCurrent = current;
    updateStyle();
}

void TimelineWidgetItem::setSuppressed(bool suppressed)
{
    m_isSuppressed = suppressed;
    m_operation.isSuppressed = suppressed;
    updateStyle();
}

void TimelineWidgetItem::setRolledBack(bool rolledBack)
{
    m_operation.isRolledBack = rolledBack;
    updateStyle();
}

void TimelineWidgetItem::updateStyle()
{
    if (m_isCurrent) {
        setStyleSheet(
            QStringLiteral("QToolButton { background-color: #3498db; "
                           "border: 2px solid #2980b9; border-radius: 4px; }")
        );
    }
    else if (m_isSuppressed) {
        setStyleSheet(
            QStringLiteral("QToolButton { background-color: transparent; "
                           "border: 1px solid #555; border-radius: 4px; "
                           "opacity: 0.4; } "
                           "QToolButton:hover { background-color: #444; }")
        );
    }
    else if (m_operation.isRolledBack) {
        setStyleSheet(
            QStringLiteral("QToolButton { background-color: transparent; "
                           "border: 1px solid #555; border-radius: 4px; "
                           "opacity: 0.5; } "
                           "QToolButton:hover { background-color: #444; }")
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

    QAction* suppressAction = menu.addAction(
        m_isSuppressed ? tr("Unsuppress") : tr("Suppress")
    );
    suppressAction->setIcon(QIcon::fromTheme(QStringLiteral("visibility")));

    QAction* rollbackAction = menu.addAction(tr("Roll History Marker Here"));
    rollbackAction->setIcon(QIcon::fromTheme(QStringLiteral("media-seek-forward")));

    menu.addSeparator();

    QAction* deleteAction = menu.addAction(tr("Delete"));
    deleteAction->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));

    connect(editAction, &QAction::triggered, this, [this]() {
        Q_EMIT editRequested(m_operation);
    });

    connect(suppressAction, &QAction::triggered, this, [this]() {
        Q_EMIT suppressToggled(m_operation, !m_isSuppressed);
    });

    connect(rollbackAction, &QAction::triggered, this, [this]() {
        Q_EMIT rollbackHere(m_operation);
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

void TimelineWidgetItem::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPosition = event->pos();
    }
    QToolButton::mousePressEvent(event);
}

void TimelineWidgetItem::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        int distance = (event->pos() - m_dragStartPosition).manhattanLength();
        if (distance >= QApplication::startDragDistance()) {
            Q_EMIT dragStarted(m_operation.index);
        }
    }
    QToolButton::mouseMoveEvent(event);
}

void TimelineWidgetItem::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasFormat(QStringLiteral("application/x-tungsten-timeline"))) {
        event->acceptProposedAction();
    }
}

void TimelineWidgetItem::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasFormat(QStringLiteral("application/x-tungsten-timeline"))) {
        QByteArray data = event->mimeData()->data(QStringLiteral("application/x-tungsten-timeline"));
        int fromIndex = data.toInt();
        Q_EMIT dragDropped(fromIndex, m_operation.index);
        event->acceptProposedAction();
    }
}

// ==================== TimelineSlider ====================

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

// ==================== TimelineGroup ====================

TimelineGroup::TimelineGroup(const QString& name, int startIndex, QWidget* parent)
    : QWidget(parent)
    , m_name(name)
    , m_startIndex(startIndex)
    , m_expanded(true)
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(2);

    auto headerLayout = new QHBoxLayout();
    headerLayout->setContentsMargins(4, 2, 4, 2);
    headerLayout->setSpacing(4);

    m_expandButton = new QToolButton(this);
    m_expandButton->setIcon(QIcon::fromTheme(QStringLiteral("list-remove")));
    m_expandButton->setFixedSize(16, 16);
    m_expandButton->setAutoRaise(true);
    connect(m_expandButton, &QToolButton::clicked, this, [this]() {
        setExpanded(!m_expanded);
    });

    m_nameLabel = new QLabel(m_name, this);
    m_nameLabel->setStyleSheet(QStringLiteral("QLabel { color: #aaa; font-size: 10px; }"));

    headerLayout->addWidget(m_expandButton);
    headerLayout->addWidget(m_nameLabel);
    headerLayout->addStretch();

    m_contentWidget = new QWidget(this);
    m_contentLayout = new QHBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(4, 2, 4, 2);
    m_contentLayout->setSpacing(4);

    mainLayout->addLayout(headerLayout);
    mainLayout->addWidget(m_contentWidget);
}

void TimelineGroup::addWidget(TimelineWidgetItem* widget)
{
    m_widgets.push_back(widget);
    m_contentLayout->addWidget(widget);
}

void TimelineGroup::removeWidget(TimelineWidgetItem* widget)
{
    auto it = std::find(m_widgets.begin(), m_widgets.end(), widget);
    if (it != m_widgets.end()) {
        m_widgets.erase(it);
        m_contentLayout->removeWidget(widget);
    }
}

void TimelineGroup::setExpanded(bool expanded)
{
    m_expanded = expanded;
    m_contentWidget->setVisible(expanded);
    m_expandButton->setIcon(
        QIcon::fromTheme(expanded ? QStringLiteral("list-remove") : QStringLiteral("list-add"))
    );
    Q_EMIT groupExpanded(m_name, expanded);
}

void TimelineGroup::setName(const QString& name)
{
    m_name = name;
    m_nameLabel->setText(name);
}

// ==================== Timeline ====================

Timeline::Timeline(QWidget* parent)
    : QWidget(parent)
    , m_document(nullptr)
    , m_currentPosition(0)
    , m_isDragging(false)
    , m_isPlaying(false)
    , m_playbackTimer(nullptr)
{
    setupUI();
    setupPlaybackControls();
    loadOperationIcons();
}

Timeline::~Timeline()
{
    stopPlayback();
}

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

void Timeline::setupPlaybackControls()
{
    m_btnPlay = new QToolButton(this);
    m_btnPlay->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    m_btnPlay->setToolTip(tr("Play timeline"));
    m_btnPlay->setAutoRaise(true);
    m_btnPlay->setFixedSize(28, 28);
    connect(m_btnPlay, &QToolButton::clicked, this, [this]() {
        if (m_isPlaying) {
            stopPlayback();
        }
        else {
            startPlayback();
        }
    });
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
    m_operationIcons["Loft"] = QIcon::fromTheme(QStringLiteral("PartDesign_Loft"));
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

    // Disconnect old signals
    connectNewObject.disconnect();
    connectDelObject.disconnect();
    connectUndo.disconnect();
    connectRedo.disconnect();

    m_document = doc;

    if (doc) {
        // Connect to document signals for auto-refresh
        connectNewObject = doc->signalNewObject.connect(
            [this](const App::DocumentObject&) {
                refresh();
            }
        );

        connectDelObject = doc->signalDeletedObject.connect(
            [this](const App::DocumentObject&) {
                refresh();
            }
        );

        connectUndo = doc->signalUndo.connect(
            [this](const App::Document&) {
                restoreFullState();
                refresh();
            }
        );

        connectRedo = doc->signalRedo.connect(
            [this](const App::Document&) {
                restoreFullState();
                refresh();
            }
        );
    }

    refresh();
}

void Timeline::refresh()
{
    updateOperationsList();
    updateSliderRange();
}

void Timeline::clear()
{
    stopPlayback();
    m_operations.clear();
    for (auto* widget : m_operationWidgets) {
        delete widget;
    }
    m_operationWidgets.clear();
    for (auto& [idx, group] : m_groups) {
        delete group;
    }
    m_groups.clear();
    m_timelineSuppressedObjects.clear();
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

    // Remove old stretch widget
    QLayoutItem* stretchItem = nullptr;
    for (int i = m_operationsLayout->count() - 1; i >= 0; --i) {
        QLayoutItem* item = m_operationsLayout->itemAt(i);
        if (item->spacerItem()) {
            stretchItem = item;
            break;
        }
    }
    if (stretchItem) {
        m_operationsLayout->removeItem(stretchItem);
        delete stretchItem;
    }

    if (!m_document) {
        m_operationsLayout->addStretch();
        return;
    }

    const std::vector<App::DocumentObject*>& objects = m_document->getObjects();

    int index = 0;
    for (auto* obj : objects) {
        if (!obj || !obj->isAttachedToDocument()) {
            continue;
        }

        std::string objType = obj->getTypeId().getName();

        bool isSuppressed = false;
        auto* ext = obj->getExtensionByType<App::SuppressibleExtension>();
        if (ext) {
            isSuppressed = ext->Suppressed.getValue();
        }

        TimelineOperation op;
        op.id = obj->getNameInDocument();
        op.name = obj->Label.getStrValue();
        op.type = objType;
        op.icon = getIconForOperation(objType);
        op.index = index;
        op.isCurrent = (index == m_currentPosition);
        op.isSuppressed = isSuppressed;
        op.isRolledBack = (index > m_currentPosition);
        op.documentName = m_document->getName();
        op.objectName = obj->getNameInDocument();
        op.groupIndex = -1;

        m_operations.push_back(op);

        auto* widget = new TimelineWidgetItem(op, m_operationsContainer);
        widget->setCurrent(op.isCurrent);
        widget->setRolledBack(op.isRolledBack);
        connect(widget, &TimelineWidgetItem::editRequested, this,
                &Timeline::onOperationEdited);
        connect(widget, &TimelineWidgetItem::deleteRequested, this,
                &Timeline::onOperationDeleted);
        connect(widget, &TimelineWidgetItem::suppressToggled, this,
                &Timeline::onOperationSuppressToggled);
        connect(widget, &TimelineWidgetItem::rollbackHere, this,
                &Timeline::onOperationRollbackHere);
        connect(widget, &TimelineWidgetItem::dragDropped, this,
                &Timeline::onDragDropped);

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
        auto* widget = m_operationWidgets[i];
        widget->setCurrent(i == static_cast<size_t>(position));
        widget->setRolledBack(i > static_cast<size_t>(position));
    }

    m_positionLabel->setText(
        QString::number(position + 1) + QStringLiteral(" / ")
        + QString::number(m_operations.size())
    );

    applyStateAtPosition(position);

    if (!m_isDragging) {
        Q_EMIT positionChanged(position);
    }
}

int Timeline::operationCount() const
{
    return static_cast<int>(m_operations.size());
}

bool Timeline::isPlaying() const
{
    return m_isPlaying;
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
    restoreFullState();
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

void Timeline::startPlayback()
{
    if (m_isPlaying || !m_document) {
        return;
    }

    m_isPlaying = true;
    m_btnPlay->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-pause")));
    m_btnPlay->setToolTip(tr("Stop playback"));

    // If at end, start from beginning
    if (m_currentPosition >= static_cast<int>(m_operations.size()) - 1) {
        goToStart();
    }

    m_playbackTimer = new QTimer(this);
    connect(m_playbackTimer, &QTimer::timeout, this, [this]() {
        if (m_currentPosition < static_cast<int>(m_operations.size()) - 1) {
            setCurrentPosition(m_currentPosition + 1);
        }
        else {
            stopPlayback();
        }
    });
    m_playbackTimer->start(500);
}

void Timeline::stopPlayback()
{
    if (!m_isPlaying) {
        return;
    }

    m_isPlaying = false;
    if (m_playbackTimer) {
        m_playbackTimer->stop();
        m_playbackTimer->deleteLater();
        m_playbackTimer = nullptr;
    }
    m_btnPlay->setIcon(QIcon::fromTheme(QStringLiteral("media-playback-start")));
    m_btnPlay->setToolTip(tr("Play timeline"));
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
                if (vp) {
                    guiDoc->setEdit(vp);
                }
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

    if (reply == QMessageBox::Yes && m_document) {
        // Check for downstream dependencies
        App::DocumentObject* obj = m_document->getObject(op.objectName.c_str());
        if (obj) {
            std::vector<App::DocumentObject*> dependents = obj->getInList();
            if (!dependents.empty()) {
                QMessageBox::warning(
                    this,
                    tr("Cannot Delete"),
                    tr("'%1' has downstream dependencies and cannot be deleted.")
                        .arg(QString::fromStdString(op.name))
                );
                return;
            }
        }

        m_document->openTransaction("Delete timeline operation");
        if (obj) {
            m_document->removeObject(op.objectName.c_str());
        }
        m_document->commitTransaction();
        m_document->recompute();
        refresh();
    }
}

void Timeline::onOperationSuppressToggled(const TimelineOperation& op, bool suppressed)
{
    if (!m_document) {
        return;
    }

    App::DocumentObject* obj = m_document->getObject(op.objectName.c_str());
    if (!obj) {
        return;
    }

    auto* ext = obj->getExtensionByType<App::SuppressibleExtension>();
    if (!ext) {
        return;
    }

    m_document->openTransaction("Toggle suppression");
    ext->Suppressed.setValue(suppressed);

    // Also suppress/unsuppress downstream dependencies
    if (suppressed) {
        std::vector<App::DocumentObject*> dependents = obj->getInListRecursive();
        for (auto* dep : dependents) {
            auto* depExt = dep->getExtensionByType<App::SuppressibleExtension>();
            if (depExt && !depExt->Suppressed.getValue()) {
                depExt->Suppressed.setValue(true);
                m_timelineSuppressedObjects.insert(dep);
            }
        }
    }
    else {
        // Unsuppress objects that were suppressed by timeline
        for (auto it = m_timelineSuppressedObjects.begin();
             it != m_timelineSuppressedObjects.end();) {
            auto* dep = *it;
            auto* depExt = dep->getExtensionByType<App::SuppressibleExtension>();
            if (depExt && depExt->Suppressed.getValue()) {
                // Only unsuppress if no other suppressed parent requires it
                bool stillNeeded = false;
                std::vector<App::DocumentObject*> depParents = dep->getOutList();
                for (auto* parent : depParents) {
                    if (parent == obj) {
                        continue;
                    }
                    auto* parentExt = parent->getExtensionByType<App::SuppressibleExtension>();
                    if (parentExt && parentExt->Suppressed.getValue()) {
                        stillNeeded = true;
                        break;
                    }
                }
                if (!stillNeeded) {
                    depExt->Suppressed.setValue(false);
                    it = m_timelineSuppressedObjects.erase(it);
                }
                else {
                    ++it;
                }
            }
            else {
                it = m_timelineSuppressedObjects.erase(it);
            }
        }
    }

    m_document->commitTransaction();
    m_document->recompute();
    refresh();
}

void Timeline::onOperationRollbackHere(const TimelineOperation& op)
{
    setCurrentPosition(op.index);
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

void Timeline::onDragDropped(int fromIndex, int toIndex)
{
    if (fromIndex == toIndex || fromIndex < 0 || toIndex < 0
        || fromIndex >= static_cast<int>(m_operations.size())
        || toIndex >= static_cast<int>(m_operations.size())) {
        return;
    }

    if (!canMoveOperation(fromIndex, toIndex)) {
        QMessageBox::warning(
            this,
            tr("Cannot Reorder"),
            tr("Cannot move '%1' past its dependencies.")
                .arg(QString::fromStdString(m_operations[fromIndex].name))
        );
        return;
    }

    performReorder(fromIndex, toIndex);
}

bool Timeline::canMoveOperation(int fromIndex, int toIndex) const
{
    if (!m_document) {
        return false;
    }

    const std::vector<App::DocumentObject*>& objects = m_document->getObjects();
    if (fromIndex < 0 || fromIndex >= static_cast<int>(objects.size())
        || toIndex < 0 || toIndex >= static_cast<int>(objects.size())) {
        return false;
    }

    App::DocumentObject* obj = objects[fromIndex];
    if (!obj) {
        return false;
    }

    // Get all downstream dependencies
    std::vector<App::DocumentObject*> dependents = obj->getInListRecursive();

    // Find the range of indices that would be affected
    int minIndex = std::min(fromIndex, toIndex);
    int maxIndex = std::max(fromIndex, toIndex);

    // Check if any dependent falls within the affected range
    for (auto* dep : dependents) {
        // Find index of this dependent
        for (int i = 0; i < static_cast<int>(objects.size()); ++i) {
            if (objects[i] == dep) {
                if (i >= minIndex && i <= maxIndex) {
                    return false; // Cannot move past a dependent
                }
                break;
            }
        }
    }

    return true;
}

void Timeline::performReorder(int fromIndex, int toIndex)
{
    if (!m_document) {
        return;
    }

    const std::vector<App::DocumentObject*>& objects = m_document->getObjects();
    if (fromIndex < 0 || fromIndex >= static_cast<int>(objects.size())
        || toIndex < 0 || toIndex >= static_cast<int>(objects.size())) {
        return;
    }

    App::DocumentObject* obj = objects[fromIndex];
    if (!obj) {
        return;
    }

    // Find the target object to reorder relative to
    App::DocumentObject* targetObj = objects[toIndex];
    if (!targetObj) {
        return;
    }

    // Use Python command to perform the reorder through the GUI
    // This uses the same mechanism as Tree view drag-and-drop
    try {
        std::string pyCmd =
            "import FreeCADGui\n"
            "doc = FreeCADGui.ActiveDocument\n"
            "obj = doc.getObject('" + std::string(obj->getNameInDocument()) + "')\n"
            "target = doc.getObject('" + std::string(targetObj->getNameInDocument()) + "')\n"
            "if obj and target:\n"
            "    import App\n"
            "    obj_group = App.GeoFeatureGroupExtension.getGroupOfObject(obj)\n"
            "    target_group = App.GeoFeatureGroupExtension.getGroupOfObject(target)\n"
            "    if obj_group and target_group and obj_group == target_group:\n"
            "        group_obj = obj_group\n"
            "        prop = group_obj.getPropertyByName('Group')\n"
            "        if prop:\n"
            "            items = prop.getValue()\n"
            "            if obj in items and target in items:\n"
            "                items.remove(obj)\n"
            "                target_idx = items.index(target)\n"
            "                items.insert(target_idx, obj)\n"
            "                prop.setValue(items)\n"
            "                FreeCADGui.ActiveDocument.recompute()\n";
        Gui::Command::doCommand(Gui::Command::Gui, "%s", pyCmd.c_str());
    }
    catch (const Base::Exception& e) {
        Base::Console().warning("Timeline reorder failed: %s\n", e.what());
        QMessageBox::warning(
            this,
            tr("Reorder Failed"),
            tr("Could not reorder operations. Use the Tree view to reorder objects.")
        );
    }

    // Update position if needed
    if (m_currentPosition == fromIndex) {
        m_currentPosition = toIndex;
    }
    else if (fromIndex < m_currentPosition && toIndex >= m_currentPosition) {
        m_currentPosition--;
    }
    else if (fromIndex > m_currentPosition && toIndex <= m_currentPosition) {
        m_currentPosition++;
    }

    refresh();
}

void Timeline::applyStateAtPosition(int position)
{
    if (!m_document || position < 0
        || position >= static_cast<int>(m_operations.size())) {
        return;
    }

    const std::vector<App::DocumentObject*>& objects = m_document->getObjects();

    // Suppress all objects after the rollback position
    for (int i = position + 1; i < static_cast<int>(objects.size()); ++i) {
        App::DocumentObject* obj = objects[i];
        if (!obj || !obj->isAttachedToDocument()) {
            continue;
        }

        auto* ext = obj->getExtensionByType<App::SuppressibleExtension>();
        if (ext && !ext->Suppressed.getValue()) {
            ext->Suppressed.setValue(true);
            m_timelineSuppressedObjects.insert(obj);
        }
    }

    // Unsuppress objects up to and including the rollback position
    // (only if they were suppressed by the timeline, not by the user)
    for (int i = 0; i <= position; ++i) {
        App::DocumentObject* obj = objects[i];
        if (!obj || !obj->isAttachedToDocument()) {
            continue;
        }

        auto* ext = obj->getExtensionByType<App::SuppressibleExtension>();
        if (ext && ext->Suppressed.getValue() && m_timelineSuppressedObjects.contains(obj)) {
            ext->Suppressed.setValue(false);
            m_timelineSuppressedObjects.remove(obj);
        }
    }

    m_document->recompute();
}

void Timeline::restoreFullState()
{
    if (!m_document) {
        return;
    }

    // Unsuppress all objects that were suppressed by the timeline
    const std::vector<App::DocumentObject*>& objects = m_document->getObjects();
    for (auto* obj : objects) {
        if (!obj || !obj->isAttachedToDocument()) {
            continue;
        }

        auto* ext = obj->getExtensionByType<App::SuppressibleExtension>();
        if (ext && ext->Suppressed.getValue() && m_timelineSuppressedObjects.contains(obj)) {
            ext->Suppressed.setValue(false);
        }
    }
    m_timelineSuppressedObjects.clear();

    m_document->recompute();
}

#include "moc_Timeline.cpp"
