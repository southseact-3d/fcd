# SPDX-License-Identifier: LGPL-2.1-or-later

import traceback

try:
    import Slicer_Gui
except Exception:
    FreeCAD.Console.PrintError(
        f"[Slicer] Failed to load Slicer_Gui module:\n{traceback.format_exc()}\n"
    )


class SlicerWorkbench(Workbench):
    """Slicer workbench for 3D printing - FDM and Resin."""

    Icon = FreeCAD.getResourceDir() + "Mod/Slicer/Resources/icons/Slicer_Workbench.svg"
    MenuText = "Slicer"
    ToolTip = "3D Print Slicer - FDM and Resin (MSLA/DLP)"

    def Initialize(self):
        self.appendToolbar(
            "Slicer",
            [
                "Slicer_Slice",
                "Slicer_ResinSlice",
                "Separator",
                "Slicer_PreviewLayers",
                "Slicer_PreviewGcode",
                "Separator",
                "Slicer_GenerateSupports",
                "Slicer_PaintSupports",
                "Separator",
                "Slicer_HollowModel",
                "Slicer_DetectCavities",
                "Separator",
                "Slicer_ImportModel",
                "Slicer_ExportSliced",
                "Separator",
                "Slicer_Preferences",
            ],
        )
        self.appendMenu(
            "Slicer",
            [
                "Slicer_Slice",
                "Slicer_ResinSlice",
                "Separator",
                "Slicer_PreviewLayers",
                "Slicer_PreviewGcode",
                "Separator",
                "Slicer_GenerateSupports",
                "Slicer_PaintSupports",
                "Separator",
                "Slicer_HollowModel",
                "Slicer_DetectCavities",
                "Separator",
                "Slicer_ImportModel",
                "Slicer_ExportSliced",
                "Separator",
                "Slicer_Preferences",
            ],
        )

    def GetClassName(self):
        return "Gui::PythonWorkbench"


Gui.addWorkbench(SlicerWorkbench())
