# SPDX-License-Identifier: LGPL-2.1-or-later

"""Output format and export options widget for SlicerWorkbench."""

from __future__ import annotations

import os
import platform
import subprocess
from datetime import datetime

from PySide6 import QtCore, QtGui, QtWidgets

try:
    import FreeCADGui

    _HAS_FREECAD = True
except ImportError:
    _HAS_FREECAD = False


# ---------------------------------------------------------------------------
# Format definitions
# ---------------------------------------------------------------------------

_FDM_FORMATS = [
    ("G-code (.gcode)", ".gcode"),
]

_RESIN_FORMATS = [
    ("CTB (.ctb)", ".ctb"),
    ("GOO (.goo)", ".goo"),
    ("SL1 (.sl1)", ".sl1"),
    ("Photon (.photon)", ".photon"),
]

_POST_SLICE_ACTIONS = [
    ("Open output folder", "open_folder"),
    ("Send to printer (OctoPrint)", "octoprint"),
    ("Do nothing", "none"),
]


# ---------------------------------------------------------------------------
# Widget
# ---------------------------------------------------------------------------


class SlicerOutputWidget(QtWidgets.QWidget):
    """Widget for output format selection and export configuration.

    Signals
    -------
    export_requested(dict)
        Emitted when *Slice & Export* is clicked.  Payload is a dict with
        keys ``format``, ``output_path``, ``filename``, ``post_action``.
    """

    export_requested = QtCore.Signal(dict)

    def __init__(self, parent: QtWidgets.QWidget | None = None) -> None:
        super().__init__(parent)

        self._mode: str = "FDM"  # "FDM" or "Resin"
        self._output_dir: str = os.path.expanduser("~/Documents/SlicerOutput")
        self._filename_template: str = "{model}_{printer}_{material}_{date}"

        self._ui_setup()

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def set_mode(self, mode: str) -> None:
        """Switch between *'FDM'* and *'Resin'* to update available formats."""
        self._mode = mode
        self._rebuild_format_combo()

    def get_export_config(self) -> dict:
        """Collect the current export configuration into a dict."""
        fmt_label = self._combo_format.currentText()
        fmt_ext = self._combo_format.currentData()
        return {
            "format": fmt_label,
            "extension": fmt_ext,
            "output_dir": self._line_dir.text(),
            "filename_template": self._line_template.text(),
            "filename": self._generate_filename(),
            "post_action": self._combo_post_action.currentData(),
        }

    # ------------------------------------------------------------------
    # UI construction
    # ------------------------------------------------------------------

    def _ui_setup(self) -> None:
        root = QtWidgets.QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)

        # --- Output format -----------------------------------------------
        fmt_group = QtWidgets.QGroupBox("Output Format")
        fmt_lay = QtWidgets.QFormLayout(fmt_group)

        self._combo_format = QtWidgets.QComboBox()
        self._combo_format.setToolTip("Select output file format")
        self._combo_format.currentIndexChanged.connect(self._on_format_changed)
        fmt_lay.addRow("Format:", self._combo_format)

        root.addWidget(fmt_group)

        # --- Output directory --------------------------------------------
        dir_group = QtWidgets.QGroupBox("Output Directory")
        dir_lay = QtWidgets.QHBoxLayout(dir_group)

        self._line_dir = QtWidgets.QLineEdit(self._output_dir)
        self._line_dir.setToolTip("Directory where sliced files will be saved")
        self._line_dir.setPlaceholderText("Select output directory…")
        self._line_dir.textChanged.connect(self._on_dir_changed)
        dir_lay.addWidget(self._line_dir, 1)

        self._btn_browse = QtWidgets.QPushButton("Browse…")
        self._btn_browse.setToolTip("Choose a folder for sliced output files")
        self._btn_browse.clicked.connect(self._on_browse_dir)
        dir_lay.addWidget(self._btn_browse)

        self._btn_open_folder = QtWidgets.QPushButton("Open Folder")
        self._btn_open_folder.setToolTip(
            "Open the output directory in the file manager"
        )
        self._btn_open_folder.clicked.connect(self._on_open_folder)
        dir_lay.addWidget(self._btn_open_folder)

        root.addWidget(dir_group)

        # --- Filename template -------------------------------------------
        tpl_group = QtWidgets.QGroupBox("Filename Template")
        tpl_lay = QtWidgets.QVBoxLayout(tpl_group)

        self._line_template = QtWidgets.QLineEdit(self._filename_template)
        self._line_template.setToolTip(
            "Template for output filenames.\n"
            "Variables: {model}, {printer}, {material}, {date}, {time}"
        )
        tpl_lay.addWidget(self._line_template)

        hint = QtWidgets.QLabel(
            "Available variables: {model}  {printer}  {material}  {date}  {time}"
        )
        hint.setStyleSheet("color: palette(mid); font-size: 10px;")
        tpl_lay.addWidget(hint)

        preview_row = QtWidgets.QHBoxLayout()
        preview_row.addWidget(QtWidgets.QLabel("Preview:"))
        self._lbl_preview = QtWidgets.QLabel(self._generate_filename())
        self._lbl_preview.setStyleSheet("font-weight: bold;")
        preview_row.addWidget(self._lbl_preview)
        preview_row.addStretch(1)
        tpl_lay.addLayout(preview_row)

        root.addWidget(tpl_group)

        # --- Post-slice action -------------------------------------------
        post_group = QtWidgets.QGroupBox("After Slicing")
        post_lay = QtWidgets.QFormLayout(post_group)

        self._combo_post_action = QtWidgets.QComboBox()
        self._combo_post_action.setToolTip("Action to perform after slicing completes")
        for label, value in _POST_SLICE_ACTIONS:
            self._combo_post_action.addItem(label, userData=value)
        post_lay.addRow("Action:", self._combo_post_action)

        root.addWidget(post_group)

        # --- File size estimate -------------------------------------------
        estimate_group = QtWidgets.QGroupBox("Estimate")
        estimate_lay = QtWidgets.QFormLayout(estimate_group)

        self._lbl_filesize = QtWidgets.QLabel("—")
        estimate_lay.addRow("Est. File Size:", self._lbl_filesize)

        root.addWidget(estimate_group)

        # --- Export button -----------------------------------------------
        self._btn_export = QtWidgets.QPushButton("Slice && Export")
        self._btn_export.setToolTip("Run the slicer and save the output file")
        self._btn_export.setMinimumHeight(36)
        font = self._btn_export.font()
        font.setBold(True)
        self._btn_export.setFont(font)
        self._btn_export.clicked.connect(self._on_export_clicked)
        root.addWidget(self._btn_export)

        # Initialize format list
        self._rebuild_format_combo()

    # ------------------------------------------------------------------
    # Format management
    # ------------------------------------------------------------------

    def _rebuild_format_combo(self) -> None:
        self._combo_format.blockSignals(True)
        self._combo_format.clear()

        formats = _FDM_FORMATS if self._mode == "FDM" else _RESIN_FORMATS
        for label, ext in formats:
            self._combo_format.addItem(label, userData=ext)

        self._combo_format.setCurrentIndex(0)
        self._combo_format.blockSignals(False)
        self._update_preview()

    # ------------------------------------------------------------------
    # Filename helpers
    # ------------------------------------------------------------------

    def _generate_filename(self) -> str:
        now = datetime.now()
        replacements = {
            "model": "model",
            "printer": "printer",
            "material": "material",
            "date": now.strftime("%Y%m%d"),
            "time": now.strftime("%H%M%S"),
        }
        tpl = (
            self._line_template.text()
            if hasattr(self, "_line_template")
            else self._filename_template
        )
        ext = (
            self._combo_format.currentData()
            if hasattr(self, "_combo_format")
            else ".gcode"
        )
        name = tpl
        for key, val in replacements.items():
            name = name.replace("{" + key + "}", val)
        return name + ext

    def _update_preview(self) -> None:
        if hasattr(self, "_lbl_preview"):
            self._lbl_preview.setText(self._generate_filename())

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------

    def _on_format_changed(self, _index: int) -> None:
        self._update_preview()

    def _on_dir_changed(self, text: str) -> None:
        self._output_dir = text

    def _on_browse_dir(self) -> None:
        d = QtWidgets.QFileDialog.getExistingDirectory(
            self, "Select Output Directory", self._output_dir
        )
        if d:
            self._line_dir.setText(d)
            self._output_dir = d

    def _on_open_folder(self) -> None:
        path = self._line_dir.text()
        if not path or not os.path.isdir(path):
            QtWidgets.QMessageBox.warning(
                self, "Folder Not Found", f"Directory does not exist:\n{path}"
            )
            return

        if platform.system() == "Windows":
            os.startfile(path)
        elif platform.system() == "Darwin":
            subprocess.Popen(["open", path])
        else:
            subprocess.Popen(["xdg-open", path])

    def _on_export_clicked(self) -> None:
        config = self.get_export_config()

        # Validate output dir
        out_dir = config["output_dir"]
        if not out_dir:
            QtWidgets.QMessageBox.warning(
                self, "No Output Directory", "Please select an output directory."
            )
            return

        if not os.path.isdir(out_dir):
            reply = QtWidgets.QMessageBox.question(
                self,
                "Create Directory?",
                f"Output directory does not exist:\n{out_dir}\n\nCreate it?",
                QtWidgets.QMessageBox.StandardButton.Yes
                | QtWidgets.QMessageBox.StandardButton.No,
            )
            if reply == QtWidgets.QMessageBox.StandardButton.Yes:
                os.makedirs(out_dir, exist_ok=True)
            else:
                return

        config["output_path"] = os.path.join(out_dir, config["filename"])
        self.export_requested.emit(config)
