#include <QApplication>
#include <QStyle>
#include <QToolBar>
#include <QToolButton>


#include "ToolBoxManager.h"
#include "Application.h"
#include "Command.h"
#include "ToolBarManager.h"
#include "ToolBox.h"


using namespace Gui;
using DockWnd::ToolBox;

ToolBoxManager* ToolBoxManager::_instance = nullptr;

ToolBoxManager* ToolBoxManager::getInstance()
{
    if (!_instance) {
        _instance = new ToolBoxManager;
    }
    return _instance;
}

void ToolBoxManager::destruct()
{
    delete _instance;
    _instance = nullptr;
}

ToolBoxManager::ToolBoxManager() = default;

ToolBoxManager::~ToolBoxManager() = default;

void ToolBoxManager::setToolBox(DockWnd::ToolBox* tb)
{
    _toolBox = tb;
}

void ToolBoxManager::setup(ToolBarItem* toolBar) const
{
    if (!toolBar || !_toolBox) {
        return;  // empty tool bar
    }

    int ct = _toolBox->count();
    for (int i = 0; i < ct; i++) {
        // get always the first item widget
        QWidget* w = _toolBox->widget(0);
        _toolBox->removeItem(0);
        delete w;
    }

    CommandManager& mgr = Application::Instance->commandManager();
    QList<ToolBarItem*> items = toolBar->getItems();

    for (auto item : items) {
        auto bar = new QToolBar();
        bar->setOrientation(Qt::Vertical);
        bar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        std::string toolbarName = item->command();
        bar->setObjectName(QString::fromLatin1(item->command().c_str()));
        bar->setWindowTitle(QObject::tr(toolbarName.c_str()));  // i18n
        _toolBox->addItem(bar, bar->windowTitle());

        QList<ToolBarItem*> subitems = item->getItems();
        for (auto subitem : subitems) {
            if (subitem->command() == "Separator") {
                // bar->addSeparator();
            }
            else {
                mgr.addTo(subitem->command().c_str(), bar);
            }
        }

        // Now set the right size policy for each tool button
        QList<QToolButton*> tool = bar->findChildren<QToolButton*>();
        for (auto it : tool) {
            it->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            // When setting the horizontal size policy but no icon is set we use the following trick
            // to make the button text left aligned.
            QIcon icon = it->icon();
            if (icon.isNull()) {
                // Create an icon filled with the button color
                int size = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
                QPixmap p(size, size);
                p.fill(Qt::transparent);
                it->setIcon(p);
            }
        }
    }
}

void ToolBoxManager::retranslate() const
{
    int ct = _toolBox->count();
    for (int i = 0; i < ct; i++) {
        // get always the first item widget
        QWidget* w = _toolBox->widget(i);
        QByteArray toolbarName = w->objectName().toUtf8();
        w->setWindowTitle(QObject::tr(toolbarName.constData()));
        _toolBox->setItemText(i, w->windowTitle());
    }
}
