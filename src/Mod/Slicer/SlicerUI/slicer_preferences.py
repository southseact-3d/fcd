# SPDX-License-Identifier: LGPL-2.1-or-later

"""Preferences dialog for the SlicerWorkbench."""

from __future__ import annotations

import json
import os
from typing import Any, Dict, List, Optional

from PySide6 import QtCore, QtGui, QtWidgets

try:
    import FreeCAD

    _HAS_FREECAD = True
except ImportError:
    _HAS_FREECAD = False


SETTINGS_FILE = "SlicerWorkbench_prefs.json"

DEFAULT_SETTINGS: Dict[str, Any] = {
    "general": {
        "output_directory": "",
        "temp_directory": "",
        "auto_save_sliced": True,
        "show_advanced_settings": False,
    },
    "fdm_engine": {
        "engine": "builtin",
        "default_printer": "",
        "default_material": "",
    },
    "printers": [],
    "materials": [],
    "resin": {
        "default_printer": "",
        "default_material": "",
        "anti_aliasing": True,
    },
}


# ---------------------------------------------------------------------------
# Sub-dialogs for editing individual profiles
# ---------------------------------------------------------------------------


class PrinterEditDialog(QtWidgets.QDialog):
    """Dialog for adding or editing a printer profile."""

    def __init__(
        self,
        parent: Optional[QtWidgets.QWidget] = None,
        profile: Optional[Dict[str, Any]] = None,
    ) -> None:
        super().__init__(parent)
        self.setWindowTitle("Edit Printer" if profile else "Add Printer")
        self.setMinimumWidth(380)
        self._profile = profile or {}
        self._setup_ui()
        if profile:
            self._load_profile(profile)

    def _setup_ui(self) -> None:
        layout = QtWidgets.QFormLayout(self)
        layout.setLabelAlignment(QtCore.Qt.AlignmentFlag.AlignRight)

        self._name_edit = QtWidgets.QLineEdit()
        self._name_edit.setPlaceholderText("e.g. Ender 3 V2")
        layout.addRow("Name:", self._name_edit)

        self._type_combo = QtWidgets.QComboBox()
        self._type_combo.addItems(["FDM", "Resin"])
        layout.addRow("Type:", self._type_combo)

        self._manufacturer_edit = QtWidgets.QLineEdit()
        self._manufacturer_edit.setPlaceholderText("e.g. Creality")
        layout.addRow("Manufacturer:", self._manufacturer_edit)

        self._bed_width_spin = QtWidgets.QDoubleSpinBox()
        self._bed_width_spin.setRange(10, 2000)
        self._bed_width_spin.setSuffix(" mm")
        self._bed_width_spin.setValue(220)
        layout.addRow("Bed Width:", self._bed_width_spin)

        self._bed_depth_spin = QtWidgets.QDoubleSpinBox()
        self._bed_depth_spin.setRange(10, 2000)
        self._bed_depth_spin.setSuffix(" mm")
        self._bed_depth_spin.setValue(220)
        layout.addRow("Bed Depth:", self._bed_depth_spin)

        self._bed_height_spin = QtWidgets.QDoubleSpinBox()
        self._bed_height_spin.setRange(10, 2000)
        self._bed_height_spin.setSuffix(" mm")
        self._bed_height_spin.setValue(250)
        layout.addRow("Bed Height:", self._bed_height_spin)

        self._max_speed_spin = QtWidgets.QDoubleSpinBox()
        self._max_speed_spin.setRange(10, 5000)
        self._max_speed_spin.setSuffix(" mm/s")
        self._max_speed_spin.setValue(150)
        layout.addRow("Max Speed:", self._max_speed_spin)

        self._nozzle_spin = QtWidgets.QDoubleSpinBox()
        self._nozzle_spin.setRange(0.1, 2.0)
        self._nozzle_spin.setSuffix(" mm")
        self._nozzle_spin.setSingleStep(0.1)
        self._nozzle_spin.setValue(0.4)
        layout.addRow("Nozzle Dia:", self._nozzle_spin)

        self._resolution_spin = QtWidgets.QDoubleSpinBox()
        self._resolution_spin.setRange(0.001, 1.0)
        self._resolution_spin.setSuffix(" mm")
        self._resolution_spin.setDecimals(3)
        self._resolution_spin.setSingleStep(0.005)
        self._resolution_spin.setValue(0.01)
        layout.addRow("Layer Res:", self._resolution_spin)

        btn_row = QtWidgets.QHBoxLayout()
        self._ok_btn = QtWidgets.QPushButton("OK")
        self._cancel_btn = QtWidgets.QPushButton("Cancel")
        btn_row.addStretch()
        btn_row.addWidget(self._ok_btn)
        btn_row.addWidget(self._cancel_btn)
        layout.addRow(btn_row)

        self._ok_btn.clicked.connect(self.accept)
        self._cancel_btn.clicked.connect(self.reject)

    def _load_profile(self, profile: Dict[str, Any]) -> None:
        self._name_edit.setText(profile.get("name", ""))
        idx = self._type_combo.findText(profile.get("printer_type", "FDM"))
        if idx >= 0:
            self._type_combo.setCurrentIndex(idx)
        self._manufacturer_edit.setText(profile.get("manufacturer", ""))
        self._bed_width_spin.setValue(profile.get("bed_width", 220))
        self._bed_depth_spin.setValue(profile.get("bed_depth", 220))
        self._bed_height_spin.setValue(profile.get("bed_height", 250))
        self._max_speed_spin.setValue(profile.get("max_speed", 150))
        self._nozzle_spin.setValue(profile.get("nozzle_diameter", 0.4))
        self._resolution_spin.setValue(profile.get("layer_resolution", 0.01))

    def get_profile(self) -> Dict[str, Any]:
        return {
            "name": self._name_edit.text().strip(),
            "printer_type": self._type_combo.currentText(),
            "manufacturer": self._manufacturer_edit.text().strip(),
            "bed_width": self._bed_width_spin.value(),
            "bed_depth": self._bed_depth_spin.value(),
            "bed_height": self._bed_height_spin.value(),
            "max_speed": self._max_speed_spin.value(),
            "nozzle_diameter": self._nozzle_spin.value(),
            "layer_resolution": self._resolution_spin.value(),
        }


class MaterialEditDialog(QtWidgets.QDialog):
    """Dialog for adding or editing a material profile."""

    def __init__(
        self,
        parent: Optional[QtWidgets.QWidget] = None,
        profile: Optional[Dict[str, Any]] = None,
    ) -> None:
        super().__init__(parent)
        self.setWindowTitle("Edit Material" if profile else "Add Material")
        self.setMinimumWidth(360)
        self._profile = profile or {}
        self._setup_ui()
        if profile:
            self._load_profile(profile)

    def _setup_ui(self) -> None:
        layout = QtWidgets.QFormLayout(self)
        layout.setLabelAlignment(QtCore.Qt.AlignmentFlag.AlignRight)

        self._name_edit = QtWidgets.QLineEdit()
        self._name_edit.setPlaceholderText("e.g. PLA White")
        layout.addRow("Name:", self._name_edit)

        self._type_edit = QtWidgets.QLineEdit()
        self._type_edit.setPlaceholderText("e.g. PLA, PETG, ABS, Resin")
        layout.addRow("Type:", self._type_edit)

        self._manufacturer_edit = QtWidgets.QLineEdit()
        self._manufacturer_edit.setPlaceholderText("e.g. eSUN")
        layout.addRow("Manufacturer:", self._manufacturer_edit)

        self._color_edit = QtWidgets.QLineEdit("#CCCCCC")
        self._color_btn = QtWidgets.QPushButton("Pick...")
        self._color_btn.setFixedWidth(60)
        color_row = QtWidgets.QHBoxLayout()
        color_row.addWidget(self._color_edit, 1)
        color_row.addWidget(self._color_btn)
        layout.addRow("Color:", color_row)

        self._print_temp_spin = QtWidgets.QDoubleSpinBox()
        self._print_temp_spin.setRange(0, 400)
        self._print_temp_spin.setSuffix(" C")
        self._print_temp_spin.setValue(210)
        layout.addRow("Print Temp:", self._print_temp_spin)

        self._bed_temp_spin = QtWidgets.QDoubleSpinBox()
        self._bed_temp_spin.setRange(0, 200)
        self._bed_temp_spin.setSuffix(" C")
        self._bed_temp_spin.setValue(60)
        layout.addRow("Bed Temp:", self._bed_temp_spin)

        self._density_spin = QtWidgets.QDoubleSpinBox()
        self._density_spin.setRange(0.1, 5.0)
        self._density_spin.setSuffix(" g/cm3")
        self._density_spin.setDecimals(2)
        self._density_spin.setValue(1.24)
        layout.addRow("Density:", self._density_spin)

        self._price_spin = QtWidgets.QDoubleSpinBox()
        self._price_spin.setRange(0, 500)
        self._price_spin.setPrefix("$ ")
        self._price_spin.setSuffix(" / kg")
        self._price_spin.setDecimals(2)
        self._price_spin.setValue(20.0)
        layout.addRow("Price:", self._price_spin)

        self._resin_check = QtWidgets.QCheckBox("Resin material")
        layout.addRow(self._resin_check)

        btn_row = QtWidgets.QHBoxLayout()
        self._ok_btn = QtWidgets.QPushButton("OK")
        self._cancel_btn = QtWidgets.QPushButton("Cancel")
        btn_row.addStretch()
        btn_row.addWidget(self._ok_btn)
        btn_row.addWidget(self._cancel_btn)
        layout.addRow(btn_row)

        self._ok_btn.clicked.connect(self.accept)
        self._cancel_btn.clicked.connect(self.reject)
        self._color_btn.clicked.connect(self._pick_color)

    def _pick_color(self) -> None:
        color = QtWidgets.QColorDialog.getColor(
            QtGui.QColor(self._color_edit.text()), self, "Select Material Color"
        )
        if color.isValid():
            self._color_edit.setText(color.name())

    def _load_profile(self, profile: Dict[str, Any]) -> None:
        self._name_edit.setText(profile.get("name", ""))
        self._type_edit.setText(profile.get("material_type", ""))
        self._manufacturer_edit.setText(profile.get("manufacturer", ""))
        self._color_edit.setText(profile.get("color", "#CCCCCC"))
        self._print_temp_spin.setValue(profile.get("print_temp", 210))
        self._bed_temp_spin.setValue(profile.get("bed_temp", 60))
        self._density_spin.setValue(profile.get("density", 1.24))
        self._price_spin.setValue(profile.get("price_per_kg", 20.0))
        self._resin_check.setChecked(profile.get("is_resin", False))

    def get_profile(self) -> Dict[str, Any]:
        return {
            "name": self._name_edit.text().strip(),
            "material_type": self._type_edit.text().strip(),
            "manufacturer": self._manufacturer_edit.text().strip(),
            "color": self._color_edit.text().strip(),
            "print_temp": self._print_temp_spin.value(),
            "bed_temp": self._bed_temp_spin.value(),
            "density": self._density_spin.value(),
            "price_per_kg": self._price_spin.value(),
            "is_resin": self._resin_check.isChecked(),
        }


# ---------------------------------------------------------------------------
# Tab: General
# ---------------------------------------------------------------------------


class _GeneralTab(QtWidgets.QWidget):
    def __init__(
        self, settings: Dict[str, Any], parent: Optional[QtWidgets.QWidget] = None
    ):
        super().__init__(parent)
        self._settings = settings.get("general", {})
        layout = QtWidgets.QFormLayout(self)
        layout.setLabelAlignment(QtCore.Qt.AlignmentFlag.AlignRight)

        self._output_dir = QtWidgets.QLineEdit(
            self._settings.get("output_directory", "")
        )
        self._output_dir.setPlaceholderText("Default output directory for sliced files")
        self._output_dir_btn = QtWidgets.QPushButton("Browse...")
        out_row = QtWidgets.QHBoxLayout()
        out_row.addWidget(self._output_dir, 1)
        out_row.addWidget(self._output_dir_btn)
        layout.addRow("Output Dir:", out_row)

        self._temp_dir = QtWidgets.QLineEdit(self._settings.get("temp_directory", ""))
        self._temp_dir.setPlaceholderText("Temporary directory for intermediate files")
        self._temp_dir_btn = QtWidgets.QPushButton("Browse...")
        temp_row = QtWidgets.QHBoxLayout()
        temp_row.addWidget(self._temp_dir, 1)
        temp_row.addWidget(self._temp_dir_btn)
        layout.addRow("Temp Dir:", temp_row)

        self._auto_save = QtWidgets.QCheckBox("Auto-save sliced files")
        self._auto_save.setChecked(self._settings.get("auto_save_sliced", True))
        self._auto_save.setToolTip("Automatically save output when slicing completes")
        layout.addRow(self._auto_save)

        self._show_advanced = QtWidgets.QCheckBox("Show advanced settings")
        self._show_advanced.setChecked(
            self._settings.get("show_advanced_settings", False)
        )
        self._show_advanced.setToolTip(
            "Reveals extra parameters in print/material profiles"
        )
        layout.addRow(self._show_advanced)

        self._output_dir_btn.clicked.connect(self._browse_output)
        self._temp_dir_btn.clicked.connect(self._browse_temp)

    def _browse_output(self) -> None:
        d = QtWidgets.QFileDialog.getExistingDirectory(self, "Output Directory")
        if d:
            self._output_dir.setText(d)

    def _browse_temp(self) -> None:
        d = QtWidgets.QFileDialog.getExistingDirectory(self, "Temp Directory")
        if d:
            self._temp_dir.setText(d)

    def save(self) -> None:
        self._settings["output_directory"] = self._output_dir.text().strip()
        self._settings["temp_directory"] = self._temp_dir.text().strip()
        self._settings["auto_save_sliced"] = self._auto_save.isChecked()
        self._settings["show_advanced_settings"] = self._show_advanced.isChecked()


# ---------------------------------------------------------------------------
# Tab: FDM Engine
# ---------------------------------------------------------------------------


class _FDMEngineTab(QtWidgets.QWidget):
    def __init__(
        self, settings: Dict[str, Any], parent: Optional[QtWidgets.QWidget] = None
    ):
        super().__init__(parent)
        self._settings = settings.get("fdm_engine", {})
        layout = QtWidgets.QVBoxLayout(self)

        info_group = QtWidgets.QGroupBox("Slicing Engine")
        info_layout = QtWidgets.QVBoxLayout(info_group)
        info_label = QtWidgets.QLabel(
            "Tungsten CAD uses the built-in Python slicing engine for both FDM "
            "and Resin workflows. No external binaries (e.g. PrusaSlicer) are "
            "required or probed at startup."
        )
        info_label.setWordWrap(True)
        info_layout.addWidget(info_label)
        layout.addWidget(info_group)

        defaults_group = QtWidgets.QGroupBox("Defaults")
        defaults_layout = QtWidgets.QFormLayout(defaults_group)
        defaults_layout.setLabelAlignment(QtCore.Qt.AlignmentFlag.AlignRight)

        self._default_printer = QtWidgets.QLineEdit(
            self._settings.get("default_printer", "")
        )
        self._default_printer.setPlaceholderText("Default FDM printer name")
        defaults_layout.addRow("Printer:", self._default_printer)

        self._default_material = QtWidgets.QLineEdit(
            self._settings.get("default_material", "")
        )
        self._default_material.setPlaceholderText("Default filament name")
        defaults_layout.addRow("Material:", self._default_material)

        layout.addWidget(defaults_group)
        layout.addStretch()

    def save(self) -> None:
        self._settings["default_printer"] = self._default_printer.text().strip()
        self._settings["default_material"] = self._default_material.text().strip()


# ---------------------------------------------------------------------------
# Tab: Printer Management
# ---------------------------------------------------------------------------


class _PrintersTab(QtWidgets.QWidget):
    def __init__(
        self, settings: Dict[str, Any], parent: Optional[QtWidgets.QWidget] = None
    ):
        super().__init__(parent)
        self._printers: List[Dict[str, Any]] = list(settings.get("printers", []))
        self._setup_ui()

    def _setup_ui(self) -> None:
        layout = QtWidgets.QVBoxLayout(self)

        self._list = QtWidgets.QListWidget()
        self._list.setAlternatingRowColors(True)
        self._list.setToolTip("Registered printer profiles")
        layout.addWidget(self._list)

        btn_row = QtWidgets.QHBoxLayout()
        self._add_btn = QtWidgets.QPushButton("Add...")
        self._edit_btn = QtWidgets.QPushButton("Edit...")
        self._delete_btn = QtWidgets.QPushButton("Delete")
        self._import_btn = QtWidgets.QPushButton("Import...")
        self._export_btn = QtWidgets.QPushButton("Export...")
        btn_row.addWidget(self._add_btn)
        btn_row.addWidget(self._edit_btn)
        btn_row.addWidget(self._delete_btn)
        btn_row.addStretch()
        btn_row.addWidget(self._import_btn)
        btn_row.addWidget(self._export_btn)
        layout.addLayout(btn_row)

        self._add_btn.clicked.connect(self._add_printer)
        self._edit_btn.clicked.connect(self._edit_printer)
        self._delete_btn.clicked.connect(self._delete_printer)
        self._import_btn.clicked.connect(self._import_printer)
        self._export_btn.clicked.connect(self._export_printer)
        self._list.itemDoubleClicked.connect(lambda _: self._edit_printer())

        self._rebuild_list()

    def _rebuild_list(self) -> None:
        self._list.clear()
        for p in self._printers:
            ptype = p.get("printer_type", "FDM")
            name = p.get("name", "Unnamed")
            mfr = p.get("manufacturer", "")
            text = f"[{ptype}] {name}" + (f" -- {mfr}" if mfr else "")
            self._list.addItem(text)

    def _add_printer(self) -> None:
        dlg = PrinterEditDialog(self)
        if dlg.exec() == QtWidgets.QDialog.DialogCode.Accepted:
            profile = dlg.get_profile()
            if profile["name"]:
                self._printers.append(profile)
                self._rebuild_list()

    def _edit_printer(self) -> None:
        row = self._list.currentRow()
        if row < 0 or row >= len(self._printers):
            return
        dlg = PrinterEditDialog(self, self._printers[row])
        if dlg.exec() == QtWidgets.QDialog.DialogCode.Accepted:
            self._printers[row] = dlg.get_profile()
            self._rebuild_list()

    def _delete_printer(self) -> None:
        row = self._list.currentRow()
        if row < 0 or row >= len(self._printers):
            return
        name = self._printers[row].get("name", "this printer")
        reply = QtWidgets.QMessageBox.question(
            self,
            "Delete Printer",
            f"Remove '{name}' from the printer list?",
            QtWidgets.QMessageBox.StandardButton.Yes
            | QtWidgets.QMessageBox.StandardButton.No,
        )
        if reply == QtWidgets.QMessageBox.StandardButton.Yes:
            self._printers.pop(row)
            self._rebuild_list()

    def _import_printer(self) -> None:
        path, _ = QtWidgets.QFileDialog.getOpenFileName(
            self, "Import Printer Profile", "", "JSON Files (*.json);;All Files (*)"
        )
        if path:
            try:
                with open(path, "r", encoding="utf-8") as fh:
                    data = json.load(fh)
                if isinstance(data, dict):
                    self._printers.append(data)
                elif isinstance(data, list):
                    self._printers.extend(data)
                self._rebuild_list()
            except Exception as exc:
                QtWidgets.QMessageBox.warning(self, "Import Error", str(exc))

    def _export_printer(self) -> None:
        row = self._list.currentRow()
        if row < 0 or row >= len(self._printers):
            return
        path, _ = QtWidgets.QFileDialog.getSaveFileName(
            self, "Export Printer Profile", "", "JSON Files (*.json)"
        )
        if path:
            try:
                with open(path, "w", encoding="utf-8") as fh:
                    json.dump(self._printers[row], fh, indent=2)
            except Exception as exc:
                QtWidgets.QMessageBox.warning(self, "Export Error", str(exc))

    def save(self, settings: Dict[str, Any]) -> None:
        settings["printers"] = self._printers


# ---------------------------------------------------------------------------
# Tab: Material Management
# ---------------------------------------------------------------------------


class _MaterialsTab(QtWidgets.QWidget):
    def __init__(
        self, settings: Dict[str, Any], parent: Optional[QtWidgets.QWidget] = None
    ):
        super().__init__(parent)
        self._materials: List[Dict[str, Any]] = list(settings.get("materials", []))
        self._setup_ui()

    def _setup_ui(self) -> None:
        layout = QtWidgets.QVBoxLayout(self)

        self._list = QtWidgets.QListWidget()
        self._list.setAlternatingRowColors(True)
        self._list.setToolTip("Registered material profiles")
        layout.addWidget(self._list)

        btn_row = QtWidgets.QHBoxLayout()
        self._add_btn = QtWidgets.QPushButton("Add...")
        self._edit_btn = QtWidgets.QPushButton("Edit...")
        self._delete_btn = QtWidgets.QPushButton("Delete")
        self._import_btn = QtWidgets.QPushButton("Import...")
        self._export_btn = QtWidgets.QPushButton("Export...")
        btn_row.addWidget(self._add_btn)
        btn_row.addWidget(self._edit_btn)
        btn_row.addWidget(self._delete_btn)
        btn_row.addStretch()
        btn_row.addWidget(self._import_btn)
        btn_row.addWidget(self._export_btn)
        layout.addLayout(btn_row)

        self._add_btn.clicked.connect(self._add_material)
        self._edit_btn.clicked.connect(self._edit_material)
        self._delete_btn.clicked.connect(self._delete_material)
        self._import_btn.clicked.connect(self._import_material)
        self._export_btn.clicked.connect(self._export_material)
        self._list.itemDoubleClicked.connect(lambda _: self._edit_material())

        self._rebuild_list()

    def _rebuild_list(self) -> None:
        self._list.clear()
        for m in self._materials:
            mtype = m.get("material_type", "?")
            name = m.get("name", "Unnamed")
            mfr = m.get("manufacturer", "")
            text = f"[{mtype}] {name}" + (f" -- {mfr}" if mfr else "")
            self._list.addItem(text)

    def _add_material(self) -> None:
        dlg = MaterialEditDialog(self)
        if dlg.exec() == QtWidgets.QDialog.DialogCode.Accepted:
            profile = dlg.get_profile()
            if profile["name"]:
                self._materials.append(profile)
                self._rebuild_list()

    def _edit_material(self) -> None:
        row = self._list.currentRow()
        if row < 0 or row >= len(self._materials):
            return
        dlg = MaterialEditDialog(self, self._materials[row])
        if dlg.exec() == QtWidgets.QDialog.DialogCode.Accepted:
            self._materials[row] = dlg.get_profile()
            self._rebuild_list()

    def _delete_material(self) -> None:
        row = self._list.currentRow()
        if row < 0 or row >= len(self._materials):
            return
        name = self._materials[row].get("name", "this material")
        reply = QtWidgets.QMessageBox.question(
            self,
            "Delete Material",
            f"Remove '{name}' from the material list?",
            QtWidgets.QMessageBox.StandardButton.Yes
            | QtWidgets.QMessageBox.StandardButton.No,
        )
        if reply == QtWidgets.QMessageBox.StandardButton.Yes:
            self._materials.pop(row)
            self._rebuild_list()

    def _import_material(self) -> None:
        path, _ = QtWidgets.QFileDialog.getOpenFileName(
            self, "Import Material Profile", "", "JSON Files (*.json);;All Files (*)"
        )
        if path:
            try:
                with open(path, "r", encoding="utf-8") as fh:
                    data = json.load(fh)
                if isinstance(data, dict):
                    self._materials.append(data)
                elif isinstance(data, list):
                    self._materials.extend(data)
                self._rebuild_list()
            except Exception as exc:
                QtWidgets.QMessageBox.warning(self, "Import Error", str(exc))

    def _export_material(self) -> None:
        row = self._list.currentRow()
        if row < 0 or row >= len(self._materials):
            return
        path, _ = QtWidgets.QFileDialog.getSaveFileName(
            self, "Export Material Profile", "", "JSON Files (*.json)"
        )
        if path:
            try:
                with open(path, "w", encoding="utf-8") as fh:
                    json.dump(self._materials[row], fh, indent=2)
            except Exception as exc:
                QtWidgets.QMessageBox.warning(self, "Export Error", str(exc))

    def save(self, settings: Dict[str, Any]) -> None:
        settings["materials"] = self._materials


# ---------------------------------------------------------------------------
# Tab: Resin
# ---------------------------------------------------------------------------


class _ResinTab(QtWidgets.QWidget):
    def __init__(
        self, settings: Dict[str, Any], parent: Optional[QtWidgets.QWidget] = None
    ):
        super().__init__(parent)
        self._settings = settings.get("resin", {})
        self._setup_ui()

    def _setup_ui(self) -> None:
        layout = QtWidgets.QFormLayout(self)
        layout.setLabelAlignment(QtCore.Qt.AlignmentFlag.AlignRight)

        self._default_printer = QtWidgets.QLineEdit(
            self._settings.get("default_printer", "")
        )
        self._default_printer.setPlaceholderText("Default resin printer name")
        layout.addRow("Default Printer:", self._default_printer)

        self._default_material = QtWidgets.QLineEdit(
            self._settings.get("default_material", "")
        )
        self._default_material.setPlaceholderText("Default resin material name")
        layout.addRow("Default Material:", self._default_material)

        self._anti_aliasing = QtWidgets.QCheckBox("Enable anti-aliasing by default")
        self._anti_aliasing.setChecked(self._settings.get("anti_aliasing", True))
        self._anti_aliasing.setToolTip(
            "Applies sub-pixel smoothing to reduce visible layer lines on resin prints"
        )
        layout.addRow(self._anti_aliasing)

    def save(self) -> None:
        self._settings["default_printer"] = self._default_printer.text().strip()
        self._settings["default_material"] = self._default_material.text().strip()
        self._settings["anti_aliasing"] = self._anti_aliasing.isChecked()


# ---------------------------------------------------------------------------
# Main preferences dialog
# ---------------------------------------------------------------------------


class SlicerPreferencesDialog(QtWidgets.QDialog):
    """Preferences dialog for slicer settings.

    Five tabs: General, FDM Engine, Printer Management,
    Material Management, and Resin.
    """

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self.setWindowTitle("SlicerWorkbench Preferences")
        self.setMinimumSize(520, 480)
        self._settings = self._load_settings()
        self._setup_ui()

    # ------------------------------------------------------------------
    # Settings persistence
    # ------------------------------------------------------------------

    @staticmethod
    def _settings_path() -> str:
        if _HAS_FREECAD:
            try:
                return os.path.join(FreeCAD.getUserAppDataDir(), SETTINGS_FILE)
            except Exception:
                pass
        return os.path.join(os.path.expanduser("~"), SETTINGS_FILE)

    @classmethod
    def _load_settings(cls) -> Dict[str, Any]:
        path = cls._settings_path()
        if os.path.isfile(path):
            try:
                with open(path, "r", encoding="utf-8") as fh:
                    data = json.load(fh)
                merged = dict(DEFAULT_SETTINGS)
                merged.update(data)
                return merged
            except Exception:
                pass
        return dict(DEFAULT_SETTINGS)

    def _save_settings(self) -> None:
        path = self._settings_path()
        try:
            os.makedirs(os.path.dirname(path), exist_ok=True)
            with open(path, "w", encoding="utf-8") as fh:
                json.dump(self._settings, fh, indent=2)
        except Exception as exc:
            QtWidgets.QMessageBox.warning(self, "Save Error", str(exc))

    # ------------------------------------------------------------------
    # UI construction
    # ------------------------------------------------------------------

    def _setup_ui(self) -> None:
        main_layout = QtWidgets.QVBoxLayout(self)

        self._tabs = QtWidgets.QTabWidget()

        self._general_tab = _GeneralTab(self._settings)
        self._fdm_tab = _FDMEngineTab(self._settings)
        self._printers_tab = _PrintersTab(self._settings)
        self._materials_tab = _MaterialsTab(self._settings)
        self._resin_tab = _ResinTab(self._settings)

        self._tabs.addTab(self._general_tab, "General")
        self._tabs.addTab(self._fdm_tab, "FDM Engine")
        self._tabs.addTab(self._printers_tab, "Printer Management")
        self._tabs.addTab(self._materials_tab, "Material Management")
        self._tabs.addTab(self._resin_tab, "Resin")

        main_layout.addWidget(self._tabs)

        btn_row = QtWidgets.QHBoxLayout()
        self._reset_btn = QtWidgets.QPushButton("Reset to Defaults")
        self._reset_btn.setToolTip("Restore all preferences to their default values")
        btn_row.addWidget(self._reset_btn)
        btn_row.addStretch()

        self._cancel_btn = QtWidgets.QPushButton("Cancel")
        self._ok_btn = QtWidgets.QPushButton("OK")
        self._ok_btn.setDefault(True)
        btn_row.addWidget(self._cancel_btn)
        btn_row.addWidget(self._ok_btn)
        main_layout.addLayout(btn_row)

        self._ok_btn.clicked.connect(self._on_ok)
        self._cancel_btn.clicked.connect(self.reject)
        self._reset_btn.clicked.connect(self._on_reset)

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------

    def _on_ok(self) -> None:
        self._general_tab.save()
        self._fdm_tab.save()
        self._printers_tab.save(self._settings)
        self._materials_tab.save(self._settings)
        self._resin_tab.save()
        self._save_settings()
        self.accept()

    def _on_reset(self) -> None:
        reply = QtWidgets.QMessageBox.question(
            self,
            "Reset Preferences",
            "Reset all preferences to defaults? This cannot be undone.",
            QtWidgets.QMessageBox.StandardButton.Yes
            | QtWidgets.QMessageBox.StandardButton.No,
        )
        if reply == QtWidgets.QMessageBox.StandardButton.Yes:
            self._settings = dict(DEFAULT_SETTINGS)
            # Remove all existing tabs
            while self._tabs.count():
                self._tabs.removeTab(0)

            self._general_tab = _GeneralTab(self._settings)
            self._fdm_tab = _FDMEngineTab(self._settings)
            self._printers_tab = _PrintersTab(self._settings)
            self._materials_tab = _MaterialsTab(self._settings)
            self._resin_tab = _ResinTab(self._settings)

            self._tabs.addTab(self._general_tab, "General")
            self._tabs.addTab(self._fdm_tab, "FDM Engine")
            self._tabs.addTab(self._printers_tab, "Printer Management")
            self._tabs.addTab(self._materials_tab, "Material Management")
            self._tabs.addTab(self._resin_tab, "Resin")
