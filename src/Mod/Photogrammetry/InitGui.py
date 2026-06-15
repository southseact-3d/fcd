# SPDX-License-Identifier: LGPL-2.1-or-later

import PhotogrammetryGui


class PhotogrammetryWorkbench(Workbench):
    """Photogrammetry workbench for scan import, processing, and B-REP conversion."""

    Icon = FreeCAD.getResourceDir() + "Mod/Photogrammetry/resources/icons/Photogrammetry_Workbench.svg"
    MenuText = "Photogrammetry"
    ToolTip = "Photogrammetry Scan Processing - Import, Process, and Convert to B-REP"

    def Initialize(self):
        self.appendToolbar(
            "Photogrammetry",
            [
                "Photogrammetry_Import",
                "Separator",
                "Photogrammetry_Process",
                "Separator",
                "Photogrammetry_MeshToBREP",
            ],
        )
        self.appendMenu(
            "Photogrammetry",
            [
                "Photogrammetry_Import",
                "Separator",
                "Photogrammetry_Process",
                "Separator",
                "Photogrammetry_MeshToBREP",
            ],
        )

    def GetClassName(self):
        return "Gui::PythonWorkbench"


Gui.addWorkbench(PhotogrammetryWorkbench())
