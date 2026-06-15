# SPDX-License-Identifier: LGPL-2.1-or-later

"""
PhotogrammetryGui - GUI Commands Module
Registers all photogrammetry workbench commands with FreeCAD.
"""

import os
import sys
import traceback

try:
    import FreeCAD
except ImportError:
    FreeCAD = None

try:
    import FreeCADGui as Gui
except ImportError:
    try:
        import FreeCAD
        Gui = FreeCAD
    except ImportError:
        Gui = None

try:
    from PySide6 import QtCore, QtGui, QtWidgets
except ImportError:
    try:
        from PySide2 import QtCore, QtGui, QtWidgets
    except ImportError:
        QtCore = QtGui = QtWidgets = None


# ---------------------------------------------------------------------------
#  Photogrammetry_Import command
# ---------------------------------------------------------------------------

class Photogrammetry_Import:
    """Import a photogrammetry scan zip file."""

    MenuText = "Import Scan..."
    ToolTip = "Import a photogrammetry scan zip file"
    Icon = os.path.join(
        os.path.dirname(__file__), "resources", "icons", "Photogrammetry_Import.svg"
    )

    def GetResources(self):
        return {
            "Pixmap": self.Icon,
            "MenuText": self.MenuText,
            "ToolTip": self.ToolTip,
        }

    def Activated(self):
        if QtWidgets is None:
            FreeCAD.Console.PrintError(
                "[Photogrammetry] PySide not available, cannot open file dialog.\n"
            )
            return

        filters = "Photogrammetry Scan (*.zip);;All Files (*)"
        path, _ = QtWidgets.QFileDialog.getOpenFileName(
            None, "Import Photogrammetry Scan", "", filters
        )
        if not path:
            return

        FreeCAD.Console.PrintLog(f"[Photogrammetry] Importing scan from: {path}\n")

        try:
            from ui import import_dialog
            import_dialog.show(path)
        except ImportError:
            FreeCAD.Console.PrintWarning(
                "[Photogrammetry] import_dialog module not yet available.\n"
            )

    def IsActive(self):
        return True


# ---------------------------------------------------------------------------
#  Photogrammetry_Process command
# ---------------------------------------------------------------------------

class Photogrammetry_Process:
    """Run the full photogrammetry pipeline on imported data."""

    MenuText = "Run Pipeline"
    ToolTip = "Run the full photogrammetry pipeline on imported data"
    Icon = os.path.join(
        os.path.dirname(__file__), "resources", "icons", "Photogrammetry_Process.svg"
    )

    def GetResources(self):
        return {
            "Pixmap": self.Icon,
            "MenuText": self.MenuText,
            "ToolTip": self.ToolTip,
        }

    def Activated(self):
        if QtWidgets is None:
            FreeCAD.Console.PrintError(
                "[Photogrammetry] PySide not available, cannot start pipeline.\n"
            )
            return

        FreeCAD.Console.PrintLog("[Photogrammetry] Starting processing pipeline.\n")

        try:
            from ui import task_panel
            task_panel.show_pipeline_panel()
        except ImportError:
            FreeCAD.Console.PrintWarning(
                "[Photogrammetry] task_panel module not yet available.\n"
            )

    def IsActive(self):
        return True


# ---------------------------------------------------------------------------
#  Photogrammetry_MeshToBREP command
# ---------------------------------------------------------------------------

class Photogrammetry_MeshToBREP:
    """Convert scanned mesh to parametric B-REP solid."""

    MenuText = "Mesh to B-REP..."
    ToolTip = "Convert scanned mesh to parametric B-REP solid"
    Icon = os.path.join(
        os.path.dirname(__file__), "resources", "icons", "Photogrammetry_MeshToBREP.svg"
    )

    def GetResources(self):
        return {
            "Pixmap": self.Icon,
            "MenuText": self.MenuText,
            "ToolTip": self.ToolTip,
        }

    def Activated(self):
        if QtWidgets is None:
            FreeCAD.Console.PrintError(
                "[Photogrammetry] PySide not available, cannot open B-REP wizard.\n"
            )
            return

        FreeCAD.Console.PrintLog("[Photogrammetry] Opening B-REP conversion wizard.\n")

        try:
            from ui import brep_wizard
            brep_wizard.show()
        except ImportError:
            FreeCAD.Console.PrintWarning(
                "[Photogrammetry] brep_wizard module not yet available.\n"
            )

    def IsActive(self):
        if FreeCAD is None:
            return False
        try:
            sel = FreeCADGui.Selection.getSelection()
            for obj in sel:
                if hasattr(obj, "Mesh"):
                    return True
        except Exception:
            pass
        return False


# ---------------------------------------------------------------------------
#  Register commands
# ---------------------------------------------------------------------------

if Gui is not None and Gui != FreeCAD:
    Gui.addCommand("Photogrammetry_Import", Photogrammetry_Import)
    Gui.addCommand("Photogrammetry_Process", Photogrammetry_Process)
    Gui.addCommand("Photogrammetry_MeshToBREP", Photogrammetry_MeshToBREP)
