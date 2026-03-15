# SPDX-License-Identifier: LGPL-2.1-or-later


class ColourWorkbench(Workbench):
    "Colour paint workbench object"

    def __init__(self):
        self.__class__.Icon = (
            FreeCAD.getResourceDir() + "Mod/Colour/Resources/icons/ColourWorkbench.svg"
        )
        self.__class__.MenuText = "Colour Paint"
        self.__class__.ToolTip = "Colour paint and textured export workbench"

    def Initialize(self):
        import ColourGui

    def GetClassName(self):
        return "ColourGui::Workbench"


Gui.addWorkbench(ColourWorkbench())
