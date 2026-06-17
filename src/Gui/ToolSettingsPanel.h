// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 liamh <liamh[at]users.sourceforge.net>            *
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


#ifndef GUI_TOOLSETTINGSPANEL_H
#define GUI_TOOLSETTINGSPANEL_H

#include <FCGlobal.h>
#include <QWidget>
#include <QScrollArea>

class QAbstractButton;
class QFrame;
class QGridLayout;
class QVBoxLayout;
class QAbstractButton;
class QDialogButtonBox;

namespace Gui
{
namespace TaskView
{
class TaskDialog;
}

/** ToolSettingsPanel
 * A fixed panel that appears in the bottom-right corner of the 3D viewport
 * when a tool dialog (TaskDialog) is active. It replaces the old Tasks dock widget
 * for showing tool parameters.
 *
 * The panel is parented to the View3DInventor widget and repositions itself
 * on resize events to stay in the bottom-right corner.
 */
class GuiExport ToolSettingsPanel: public QWidget
{
    Q_OBJECT

public:
    explicit ToolSettingsPanel(QWidget* viewportWidget);
    ~ToolSettingsPanel() override;

    /** Show the panel with the given task dialog's content.
     * Extracts widgets from the dialog and displays them.
     */
    void showDialog(TaskView::TaskDialog* dlg);

    /** Hide and clean up the panel contents. */
    void hidePanel();

    /** Returns true if a dialog is currently shown. */
    bool hasActiveDialog() const;

    /** Returns the currently active dialog, or nullptr. */
    TaskView::TaskDialog* activeDialog() const;

    /** Update panel position to stay in bottom-right corner of viewport. */
    void updatePosition();

Q_SIGNALS:
    /** Emitted when the user clicks OK. */
    void dialogAccepted();
    /** Emitted when the user clicks Cancel. */
    void dialogRejected();
    /** Emitted when the user clicks a custom button. */
    void dialogClicked(int button);

protected:
    /** Reposition on parent resize. */
    bool eventFilter(QObject* obj, QEvent* event) override;
    /** Handle Escape key to close dialog. */
    void keyPressEvent(QKeyEvent* event) override;

private:
    void setupUi();
    void clearContents();
    void onButtonClicked(QAbstractButton* button);

    QWidget* _viewportWidget;
    TaskView::TaskDialog* _activeDialog;

    QVBoxLayout* _mainLayout;
    QScrollArea* _scrollArea;
    QWidget* _contentWidget;
    QVBoxLayout* _contentLayout;
    QFrame* _buttonBar;
    QDialogButtonBox* _buttonBox;
};

}  // namespace Gui

#endif  // GUI_TOOLSETTINGSPANEL_H
