# SPDX-License-Identifier: LGPL-2.1-or-later

# Fusion360Import workbench GUI init.
# Registers the workbench, the toolbar and the menu, then wires up the
# three commands (connect, import, help) by importing commands.py.

import os

import FreeCAD
import FreeCADGui as Gui


__title__ = "Fusion 360 Import Workbench"
__author__ = "Tungsten CAD contributors"
__url__ = "https://github.com/southseact-3d/fcd"

# Workbench icon path - falls back to a built-in icon if the SVG
# cannot be located (e.g. before a clean build).
ICON_DIR = os.path.join(FreeCAD.getResourceDir(), "Mod", "Fusion360Import", "Resources", "icons")
ICON_PATH = os.path.join(ICON_DIR, "Fusion360Import.svg")


class Fusion360ImportWorkbench(Gui.Workbench):
    """The Fusion 360 Import workbench."""

    def __init__(self):
        # QT translation shim (matches the pattern used by Draft).
        def QT_TRANSLATE_NOOP(_ctx, text):
            return text

        self.__class__.MenuText = QT_TRANSLATE_NOOP(
            "Fusion360Import", "&Fusion 360 Import"
        )
        self.__class__.ToolTip = QT_TRANSLATE_NOOP(
            "Fusion360Import",
            "Import parametric designs from a running Fusion 360 instance",
        )
        if os.path.exists(ICON_PATH):
            self.__class__.Icon = ICON_PATH
        else:
            self.__class__.Icon = "PartDesign_AdditiveBox"

    def Initialize(self):
        """Register toolbars, menus and commands when the workbench loads."""
        # Importing the commands module registers them with FreeCADGui.
        from Fusion360Import import commands
        commands.register_commands()

        self.appendToolbar("Fusion 360 Import", [
            "Fusion360_Connect",
            "Fusion360_ImportDesign",
            "Fusion360_Help",
        ])

        self.appendMenu("&Fusion 360 Import", [
            "Fusion360_Connect",
            "Fusion360_ImportDesign",
            "Separator",
            "Fusion360_Help",
        ])

        FreeCAD.Console.PrintLog("[Fusion360Import] workbench initialised\n")

    def Activated(self):
        FreeCAD.Console.PrintLog("[Fusion360Import] activated\n")

    def Deactivated(self):
        FreeCAD.Console.PrintLog("[Fusion360Import] deactivated\n")


Gui.addWorkbench(Fusion360ImportWorkbench())