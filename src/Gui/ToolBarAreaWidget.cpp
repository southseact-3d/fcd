#include <QHBoxLayout>
#include <QTimer>


#include "MainWindow.h"
#include "ToolBarAreaWidget.h"
#include "ToolBarManager.h"
#include <Base/Tools.h>

using namespace Gui;

ToolBarAreaWidget::ToolBarAreaWidget(
    QWidget* parent,
    ToolBarArea area,
    const ParameterGrp::handle& hParam,
    fastsignals::advanced_scoped_connection& conn,
    QTimer* timer
)
    : QWidget(parent)
    , _sizingTimer(timer)
    , _hParam(hParam)
    , _conn(conn)
    , _area(area)
{
    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(QMargins());
}

void ToolBarAreaWidget::addWidget(QWidget* widget)
{
    // if widget already exist don't do anything
    if (_layout->indexOf(widget) >= 0) {
        return;
    }

    if (auto toolbar = qobject_cast<ToolBar*>(widget)) {
        toolbar->updateCustomGripVisibility();
    }

    _layout->addWidget(widget);
    adjustParent();

    QString name = widget->objectName();

    if (!name.isEmpty()) {
        Base::ConnectionBlocker block(_conn);
        _hParam->SetInt(widget->objectName().toUtf8().constData(), _layout->count() - 1);
    }
}

void ToolBarAreaWidget::insertWidget(int index, QWidget* widget)
{
    int currentIndex = _layout->indexOf(widget);

    // we are inserting widget at the same place, this is no-op
    if (currentIndex == index) {
        return;
    }

    // widget already exists in the area, we need to first remove it and then recreate
    if (currentIndex > 0) {
        _layout->removeWidget(widget);
    }

    _layout->insertWidget(index, widget);

    if (auto toolbar = qobject_cast<ToolBar*>(widget)) {
        toolbar->updateCustomGripVisibility();
    }

    adjustParent();
    saveState();
}

void ToolBarAreaWidget::removeWidget(QWidget* widget)
{
    _layout->removeWidget(widget);

    if (auto toolbar = qobject_cast<ToolBar*>(widget)) {
        toolbar->updateCustomGripVisibility();
    }

    QString name = widget->objectName();
    if (!name.isEmpty()) {
        Base::ConnectionBlocker block(_conn);
        _hParam->RemoveInt(name.toUtf8().constData());
    }

    adjustParent();
}

void ToolBarAreaWidget::adjustParent()
{
    if (_sizingTimer) {
        _sizingTimer->start(10);
    }
}

void ToolBarAreaWidget::saveState()
{
    Base::ConnectionBlocker block(_conn);

    for (auto& v : _hParam->GetIntMap()) {
        _hParam->RemoveInt(v.first.c_str());
    }

    foreachToolBar([this](QToolBar* toolbar, int idx, ToolBarAreaWidget*) {
        _hParam->SetInt(toolbar->objectName().toUtf8().constData(), idx);
    });
}

void ToolBarAreaWidget::restoreState(const std::map<int, QToolBar*>& toolbars)
{
    for (const auto& [index, toolbar] : toolbars) {
        bool visible = toolbar->isVisible();
        getMainWindow()->removeToolBar(toolbar);
        toolbar->setOrientation(Qt::Horizontal);
        insertWidget(index, toolbar);
        toolbar->setVisible(visible);
    }

    for (const auto& [name, visible] : _hParam->GetBoolMap()) {
        auto widget = findChild<QWidget*>(QString::fromUtf8(name.c_str()));

        if (widget) {
            widget->setVisible(visible);
        }
    }
}
