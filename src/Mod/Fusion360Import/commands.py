# SPDX-License-Identifier: LGPL-2.1-or-later
"""GUI commands for the Fusion360Import workbench.

Three commands are registered:

- ``Fusion360_Connect``        - open the connection settings dialog.
- ``Fusion360_ImportDesign``   - extract the open Fusion design and
                                  rebuild it in the current document.
- ``Fusion360_OpenUserGuide``  - show the README in a Qt dialog.

The commands are intentionally thin: they marshal user input (active
document, settings) and delegate the actual work to the
:mod:`importer` module.
"""

from __future__ import annotations

import traceback

import FreeCAD
import FreeCADGui

from PySide import QtWidgets


def _ensure_document() -> FreeCAD.Document | None:
    doc = FreeCAD.ActiveDocument
    if doc is None:
        doc = FreeCAD.newDocument("FusionImport")
    return doc


def _show_error(title: str, message: str) -> None:
    box = QtWidgets.QMessageBox()
    box.setIcon(QtWidgets.QMessageBox.Critical)
    box.setWindowTitle(title)
    box.setText(message)
    box.exec_()


def _show_info(title: str, message: str) -> None:
    box = QtWidgets.QMessageBox()
    box.setIcon(QtWidgets.QMessageBox.Information)
    box.setWindowTitle(title)
    box.setText(message)
    box.exec_()


class Fusion360ConnectCommand:
    """Show the connection settings dialog and persist the result."""

    def GetResources(self):
        return {
            "Pixmap": "preferences-system",
            "MenuText": "Connect to Fusion 360...",
            "ToolTip": "Configure the host and port of the Fusion 360 MCP add-in",
        }

    def IsActive(self):
        return True

    def Activated(self):
        # Defer import so FreeCAD can finish loading the workbench
        # before we touch any of our modules.
        from . import connection_dialog
        connection_dialog.show(parent=FreeCADGui.getMainWindow())


class Fusion360ImportDesignCommand:
    """Extract the open Fusion design and rebuild it parametrically."""

    def GetResources(self):
        return {
            "Pixmap": "PartDesign_AdditiveBox",
            "MenuText": "Import design from Fusion 360",
            "ToolTip": (
                "Connect to Fusion 360, extract the parametric design "
                "history, and rebuild it as Tungsten CAD features"
            ),
        }

    def IsActive(self):
        return FreeCADGui.activeDocument() is not None or True

    def Activated(self):
        try:
            self._run()
        except Exception as exc:
            _show_error(
                "Fusion 360 import failed",
                f"{exc}\n\n{traceback.format_exc()}",
            )

    def _run(self) -> None:
        from . import connection_dialog, progress_dialog, results_dialog
        from . import importer
        from .extractor import (
            FusionExtractionError,
            FusionExtractor,
        )

        settings = connection_dialog.load_settings()
        host = settings["host"]
        port = int(settings["port"])

        # Probe the connection before bothering the user.
        try:
            extractor = FusionExtractor(host=host, port=port)
            if not extractor.is_connected():
                # Open the settings dialog so the user can correct the
                # host/port and re-try.
                result = connection_dialog.show(parent=FreeCADGui.getMainWindow())
                if not result:
                    return
                host, port, _ = result
                extractor = FusionExtractor(host=host, port=port)
                if not extractor.is_connected():
                    _show_error(
                        "Cannot reach Fusion 360",
                        f"Could not connect to the Fusion 360 MCP add-in "
                        f"at {host}:{port}. Make sure Fusion 360 is "
                        f"running with the Fusion360MCP add-in enabled "
                        f"(Shift+S > Add-Ins > Fusion360MCP > Run on "
                        f"Startup).",
                    )
                    return
        except FusionExtractionError as exc:
            _show_error("Fusion 360 connection error", str(exc))
            return

        doc = _ensure_document()
        if doc is None:
            _show_error("No document", "Could not create or open a FreeCAD document.")
            return

        progress = progress_dialog.ProgressDialog(FreeCADGui.getMainWindow())
        progress.show()
        QtWidgets.QApplication.processEvents()

        try:
            body, history, result = importer.import_design_from_fusion(
                doc,
                host=host,
                port=port,
                progress=progress_dialog.make_callback(progress),
            )
        except FusionExtractionError as exc:
            progress.close()
            _show_error("Fusion 360 extraction failed", str(exc))
            return
        finally:
            progress.close()

        FreeCADGui.activeDocument().activeView().viewAxonometric()
        FreeCADGui.SendMsgToActiveView("ViewFit")
        FreeCAD.Console.PrintMessage(
            f"[Fusion360Import] Imported {result.feature_count} features "
            f"and {result.sketch_count} sketches into '{result.body_name}'.\n"
        )
        results_dialog.show(
            body_name=result.body_name,
            feature_count=result.feature_count,
            sketch_count=result.sketch_count,
            skipped=result.skipped,
            warnings=result.warnings,
            parent=FreeCADGui.getMainWindow(),
        )


class Fusion360HelpCommand:
    """Show the README so the user has the setup instructions handy."""

    def GetResources(self):
        return {
            "Pixmap": "help-browser",
            "MenuText": "Fusion 360 import - help",
            "ToolTip": "Show setup and usage instructions for the Fusion 360 import",
        }

    def IsActive(self):
        return True

    def Activated(self):
        from . import help_dialog
        help_dialog.show(parent=FreeCADGui.getMainWindow())


# ----------------------------------------------------------------------
# Registration helpers
# ----------------------------------------------------------------------

def register_commands() -> None:
    FreeCADGui.addCommand("Fusion360_Connect", Fusion360ConnectCommand())
    FreeCADGui.addCommand("Fusion360_ImportDesign", Fusion360ImportDesignCommand())
    FreeCADGui.addCommand("Fusion360_Help", Fusion360HelpCommand())