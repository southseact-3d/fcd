# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2024 liamh <liamh[at]users.sourceforge.net>            *
# *                                                                         *
# *   This file is part of the FreeCAD CAx development system.              *
# *                                                                         *
# *   This library is free software; you can redistribute it and/or         *
# *   modify it under the terms of the GNU Library General Public           *
# *   License as published by the Free Software Foundation; either          *
# *   version 2 of the License, or (at your option) any later version.      *
# *                                                                         *
# *   This library  is distributed in the hope that it will be useful,      *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this library; see the file COPYING.LIB. If not,    *
# *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
# *   Suite 330, Boston, MA  02111-1307, USA                                *
# *                                                                         *
# ***************************************************************************

"""CAM Part Tools Widget — Python port of the PartDesign PartToolsWidget pattern.

Provides a compact toolbar widget with grouped tool buttons for the CAM workbench.
Groups: Setup, Operations, Modify. Each group shows icon-only buttons with a
dropdown menu label for overflow access.
"""

import FreeCAD
import FreeCADGui
from PySide import QtCore, QtGui, QtWidgets

_MinButtonWidth = 36
_IconSize = 20


def _isDarkTheme():
    """Return True if FreeCAD is currently using a dark theme palette."""
    try:
        palette = FreeCADGui.getMainWindow().palette()
        return palette.color(QtGui.QPalette.Window).lightness() < 128
    except Exception:
        return False


def _labelSheet():
    return (
        "QToolButton {"
        "  border: none;"
        "  border-top: 1px solid #ccc;"
        "  padding: 2px 4px;"
        "  font-size: 10px;"
        "  color: #555;"
        "  text-transform: uppercase;"
        "  letter-spacing: 0.5px;"
        "}"
        "QToolButton:hover { color: #0078d4; }"
    )


def _groupToolSheet():
    return (
        "QToolButton {"
        "  border: 1px solid transparent;"
        "  border-radius: 3px;"
        "  padding: 2px;"
        "}"
        "QToolButton:hover {"
        "  background: #d0d0d0;"
        "  border-color: #aaa;"
        "}"
        "QToolButton:pressed {"
        "  background: #b8b8b8;"
        "}"
    )


# ---------------------------------------------------------------------------
# GroupWidget — a labeled tool group (tools on top, label below)
# ---------------------------------------------------------------------------

class _GroupWidget(QtWidgets.QWidget):
    """A compact group of tool buttons with a dropdown label at the bottom."""

    def __init__(self, parent=None):
        super().__init__(parent)
        self._buttons = []
        self._menu = None

        mainLayout = QtWidgets.QVBoxLayout(self)
        mainLayout.setContentsMargins(0, 0, 0, 0)
        mainLayout.setSpacing(0)

        self._toolsContainer = QtWidgets.QWidget(self)
        self._toolsLayout = QtWidgets.QHBoxLayout(self._toolsContainer)
        self._toolsLayout.setContentsMargins(2, 2, 2, 0)
        self._toolsLayout.setSpacing(1)
        self._toolsLayout.addStretch()

        self._labelButton = QtWidgets.QToolButton(self)
        self._labelButton.setToolButtonStyle(QtCore.Qt.ToolButtonTextBesideIcon)
        self._labelButton.setAutoRaise(True)
        self._labelButton.setStyleSheet(_labelSheet())
        self._labelButton.setCursor(QtCore.Qt.PointingHandCursor)
        self._labelButton.setFixedHeight(20)
        self._labelButton.clicked.connect(self._showMenu)

        mainLayout.addWidget(self._toolsContainer, 1)
        mainLayout.addWidget(self._labelButton, 0)

    def addButton(self, btn):
        """Add a QToolButton to this group."""
        btn.setParent(self._toolsContainer)
        btn.setIconSize(QtCore.QSize(_IconSize, _IconSize))
        btn.setToolButtonStyle(QtCore.Qt.ToolButtonIconOnly)
        btn.setAutoRaise(True)
        btn.setStyleSheet(_groupToolSheet())
        btn.setMinimumWidth(_MinButtonWidth)
        btn.setMaximumWidth(_MinButtonWidth + 8)
        btn.setFixedHeight(_IconSize + 8)

        self._buttons.append(btn)
        count = self._toolsLayout.count()
        self._toolsLayout.insertWidget(count - 1, btn)
        return btn

    def setLabelText(self, text):
        self._labelButton.setText(text + "  \u25BC")

    def setMenu(self, menu):
        self._menu = menu

    def updateVisibility(self, availableWidth):
        """Show as many buttons as will fit, hide the rest."""
        maxVisible = max(1, availableWidth // _MinButtonWidth)
        for i, btn in enumerate(self._buttons):
            btn.show() if i < maxVisible else btn.hide()

    def _showMenu(self):
        if self._menu:
            pos = self._labelButton.mapToGlobal(QtCore.QPoint(0, self._labelButton.height()))
            self._menu.exec_(pos)


# ---------------------------------------------------------------------------
# CAMPartToolsWidget
# ---------------------------------------------------------------------------

class CAMPartToolsWidget(QtWidgets.QWidget):
    """Compact CAM toolbar widget with Setup / Operations / Modify groups."""

    def __init__(self, parent=None):
        super().__init__(parent)

        layout = QtWidgets.QHBoxLayout(self)
        layout.setContentsMargins(2, 0, 2, 0)
        layout.setSpacing(4)

        self._setupGroup = self._createSetupGroup()
        self._opsGroup = self._createOperationsGroup()
        self._modGroup = self._createModifyGroup()

        layout.addWidget(self._setupGroup, 1)
        layout.addWidget(self._opsGroup, 1)
        layout.addWidget(self._modGroup, 1)

    # -- group factories ----------------------------------------------------

    def _createSetupGroup(self):
        commands = ["CAM_Job", "CAM_Sanity", "CAM_PostTools"]
        menu = self._buildMenu(commands)
        return self._buildGroup("Setup", commands, menu)

    def _createOperationsGroup(self):
        commands = ["CAM_Profile", "CAM_Pocket_Shape", "CAM_MillFacing", "CAM_Drilling", "CAM_EngraveTools"]
        menu = self._buildMenu(commands)
        return self._buildGroup("Operations", commands, menu)

    def _createModifyGroup(self):
        commands = ["CAM_OperationCopy", "CAM_Array", "CAM_DressupTools"]
        menu = self._buildMenu(commands)
        return self._buildGroup("Modify", commands, menu)

    # -- helpers ------------------------------------------------------------

    def _buildGroup(self, label, cmdNames, menu):
        group = _GroupWidget(self)
        group.setLabelText(label)
        group.setMenu(menu)

        for cmdName in cmdNames:
            btn = self._makeButton(cmdName)
            if btn:
                group.addButton(btn)
        return group

    def _makeButton(self, cmdName):
        """Create a QToolButton backed by a registered FreeCAD command."""
        cmdMgr = FreeCADGui.commandManager()
        cmd = cmdMgr.getCommandByName(cmdName)
        if not cmd:
            return None

        action = cmd.getAction()
        if not action:
            return None

        btn = QtWidgets.QToolButton(self)
        btn.setDefaultAction(action)
        btn.setIcon(action.icon())
        btn.setToolTip(cmd.getToolTipText())
        btn.setText(cmd.getMenuText())
        btn.setToolButtonStyle(QtCore.Qt.ToolButtonIconOnly)
        btn.setIconSize(QtCore.QSize(_IconSize, _IconSize))
        btn.setAutoRaise(True)
        btn.setStyleSheet(_groupToolSheet())
        return btn

    def _buildMenu(self, cmdNames):
        """Build a QMenu containing actions for all listed commands."""
        menu = QtWidgets.QMenu(self)
        cmdMgr = FreeCADGui.commandManager()

        for cmdName in cmdNames:
            cmd = cmdMgr.getCommandByName(cmdName)
            if cmd:
                action = cmd.getAction()
                if action:
                    menu.addAction(action)
        return menu

    # -- responsive resize --------------------------------------------------

    def resizeEvent(self, event):
        super().resizeEvent(event)
        totalWidth = event.size().width()
        groupWidth = totalWidth // 3
        self._setupGroup.updateVisibility(groupWidth)
        self._opsGroup.updateVisibility(groupWidth)
        self._modGroup.updateVisibility(groupWidth)
