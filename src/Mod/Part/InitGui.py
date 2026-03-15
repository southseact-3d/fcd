# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2002 Juergen Riegel <juergen.riegel@web.de>             *
# *                                                                         *
# *   This file is part of the FreeCAD CAx development system.              *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   FreeCAD is distributed in the hope that it will be useful,            *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Lesser General Public License for more details.                   *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with FreeCAD; if not, write to the Free Software        *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************
"""Initialization of the Part Workbench graphical interface."""

import FreeCAD as App
import FreeCADGui as Gui
import os


class PartWorkbench(Gui.Workbench):
    """Part workbench object."""

    def __init__(self):
        self.__class__.Icon = os.path.join(
            App.getResourceDir(), "Mod", "Part", "Resources", "icons", "PartWorkbench.svg"
        )
        self.__class__.MenuText = "Part"
        self.__class__.ToolTip = "Part workbench"

    def Initialize(self):
        # Part workbench is deprecated and its UI is no longer registered.
        # This module is kept for compatibility to avoid import errors.
        pass

    def GetClassName(self):
        return "PartGui::Workbench"


# Part workbench is intentionally not registered anymore.
# Its functionality is exposed in the Part Design workbench.
App.Console.PrintLog("Part workbench registration disabled: use Part Design workbench.\n")

App.__unit_test__ += ["TestPartGui"]
