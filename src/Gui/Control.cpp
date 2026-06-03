/***************************************************************************
 *   Copyright 2011 (c) Jürgen Riegel <juergen.riegel@web.de>              *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
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

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QPointer>

#include <App/AutoTransaction.h>
#include <Gui/ComboView.h>
#include <Gui/DockWindowManager.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>

#include "Control.h"
#include "BitmapFactory.h"
#include "Tree.h"
#include "ToolSettingsPanel.h"
#include "TaskView/TaskView.h"


using namespace Gui;
using namespace std;

/* TRANSLATOR Gui::ControlSingleton */

ControlSingleton* ControlSingleton::_pcSingleton = nullptr;

ControlSingleton::ControlSingleton()
    : ActiveDialog(nullptr)
    , oldTabIndex(-1)
    , _toolSettingsPanel(nullptr)
{}

ControlSingleton::~ControlSingleton() = default;

Gui::TaskView::TaskView* ControlSingleton::taskPanel() const
{
    // TaskView dock widget has been removed; always return nullptr.
    return nullptr;
}

Gui::ToolSettingsPanel* ControlSingleton::toolSettingsPanel() const
{
    return _toolSettingsPanel;
}

void ControlSingleton::ensureToolSettingsPanel()
{
    if (_toolSettingsPanel) {
        return;
    }

    // Find the active 3D view to parent the panel to
    auto* mainWin = getMainWindow();
    if (!mainWin) {
        return;
    }

    auto* view = qobject_cast<View3DInventor*>(mainWin->activeWindow());
    if (!view) {
        return;
    }

    _toolSettingsPanel = new ToolSettingsPanel(view->getViewer()->getWidget());

    // Connect signals so that accept/reject routes through ControlSingleton
    QObject::connect(_toolSettingsPanel, &ToolSettingsPanel::dialogAccepted,
                     this, &ControlSingleton::onDialogAccepted);
    QObject::connect(_toolSettingsPanel, &ToolSettingsPanel::dialogRejected,
                     this, &ControlSingleton::onDialogRejected);
}

void ControlSingleton::showDockWidget(QWidget* widget)
{
    QWidget* parent = widget->parentWidget();
    if (parent) {
        parent->show();
        parent->raise();
    }
}

QTabBar* ControlSingleton::findTabBar(QDockWidget* widget) const
{
    int count = getMainWindow()->tabifiedDockWidgets(widget).size() + 1;
    if (count > 1) {
        QList<QTabBar*> bars = getMainWindow()->findChildren<QTabBar*>();
        for (auto it : bars) {
            if (it->count() <= count) {
                for (int i = 0; i < count; i++) {
                    if (it->tabText(i) == widget->windowTitle()) {
                        return it;
                    }
                }
            }
        }
    }

    return nullptr;
}

void ControlSingleton::aboutToShowDialog(QDockWidget* widget)
{
    Q_UNUSED(widget);
}

void ControlSingleton::aboutToHideDialog(QDockWidget* widget)
{
    Q_UNUSED(widget);
}

void ControlSingleton::showTaskView()
{
    // No-op: Tasks dock widget has been removed.
}

void ControlSingleton::showModelView()
{
    auto treeView = qobject_cast<Gui::TreeDockWidget*>(
        Gui::DockWindowManager::instance()->getDockWindow("Tree view")
    );
    if (treeView) {
        showDockWidget(treeView);
    }
    else {
        auto comboView = qobject_cast<Gui::DockWnd::ComboView*>(
            Gui::DockWindowManager::instance()->getDockWindow("Model")
        );
        if (comboView) {
            showDockWidget(comboView);
        }
    }
}

void ControlSingleton::showDialog(Gui::TaskView::TaskDialog* dlg)
{
    // only one dialog at a time, print a warning instead of raising an assert
    if (ActiveDialog && ActiveDialog != dlg) {
        if (dlg) {
            qWarning() << "ControlSingleton::showDialog: Can't show "
                       << dlg->metaObject()->className()
                       << " since there is already an active task dialog";
        }
        else {
            qWarning() << "ControlSingleton::showDialog: Task dialog is null";
        }
        return;
    }

    // Since the caller sets up a modeless task panel, it indicates intention
    // for prolonged editing. So disable auto transaction in the current call
    // stack.
    // Do this before showing the dialog because its open() function is called
    // which may open a transaction but fails when auto transaction is still active.
    App::AutoTransaction::setEnable(false);

    // Ensure we have a tool settings panel
    ensureToolSettingsPanel();

    if (_toolSettingsPanel) {
        _toolSettingsPanel->showDialog(dlg);
    }

    if (ActiveDialog == dlg) {
        return;  // dialog is already defined
    }
    ActiveDialog = dlg;
    connect(dlg, &TaskView::TaskDialog::aboutToBeDestroyed, this, &ControlSingleton::closedDialog);
}

Gui::TaskView::TaskDialog* ControlSingleton::activeDialog() const
{
    return ActiveDialog;
}

void ControlSingleton::accept()
{
    if (_toolSettingsPanel && _toolSettingsPanel->hasActiveDialog()) {
        // Route through the task dialog's own accept logic
        if (ActiveDialog) {
            ActiveDialog->accept();
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
        }
    }
}

void ControlSingleton::reject()
{
    if (_toolSettingsPanel && _toolSettingsPanel->hasActiveDialog()) {
        if (ActiveDialog) {
            ActiveDialog->reject();
            qApp->processEvents(QEventLoop::ExcludeUserInputEvents | QEventLoop::ExcludeSocketNotifiers);
        }
    }
}

void ControlSingleton::closeDialog()
{
    if (_toolSettingsPanel) {
        _toolSettingsPanel->hidePanel();
    }
    ActiveDialog = nullptr;
}

void ControlSingleton::onDialogAccepted()
{
    if (ActiveDialog) {
        accept();
    }
}

void ControlSingleton::onDialogRejected()
{
    if (ActiveDialog) {
        reject();
    }
}

void ControlSingleton::closedDialog()
{
    ActiveDialog = nullptr;

    if (_toolSettingsPanel) {
        _toolSettingsPanel->hidePanel();
    }
}

bool ControlSingleton::isAllowedAlterDocument() const
{
    if (ActiveDialog) {
        return ActiveDialog->isAllowedAlterDocument();
    }
    return true;
}


bool ControlSingleton::isAllowedAlterView() const
{
    if (ActiveDialog) {
        return ActiveDialog->isAllowedAlterView();
    }
    return true;
}

bool ControlSingleton::isAllowedAlterSelection() const
{
    if (ActiveDialog) {
        return ActiveDialog->isAllowedAlterSelection();
    }
    return true;
}

// -------------------------------------------

ControlSingleton& ControlSingleton::instance()
{
    if (!_pcSingleton) {
        _pcSingleton = new ControlSingleton;
    }
    return *_pcSingleton;
}

void ControlSingleton::destruct()
{
    if (_pcSingleton) {
        delete _pcSingleton;
    }
    _pcSingleton = nullptr;
}


// -------------------------------------------


#include "moc_Control.cpp"
