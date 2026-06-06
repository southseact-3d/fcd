# SPDX-License-Identifier: LGPL-2.1-or-later

import FreeCAD

FreeCAD.addImportType("Slicer Mesh (*.stl *.STL *.obj *.OBJ)", "Slicer")
FreeCAD.addExportType("Slicer G-code (*.gcode)", "Slicer")
FreeCAD.addExportType("Slicer Resin (*.ctb *.goo *.sl1)", "Slicer")
