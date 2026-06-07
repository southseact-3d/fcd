# SPDX-License-Identifier: LGPL-2.1-or-later

"""Printer selection widget for SlicerWorkbench."""

from __future__ import annotations

import typing
from datetime import datetime

from PySide6 import QtCore, QtGui, QtWidgets

try:
    import FreeCADGui

    _HAS_FREECAD = True
except ImportError:
    _HAS_FREECAD = False


# ---------------------------------------------------------------------------
# Data model helpers
# ---------------------------------------------------------------------------


class _PrinterProfile:
    """Lightweight container for a single printer profile."""

    def __init__(
        self,
        name: str,
        printer_type: str = "FDM",
        bed_x: float = 220.0,
        bed_y: float = 220.0,
        bed_z: float = 250.0,
        max_speed: float = 150.0,
        nozzle_diameter: float = 0.4,
        layer_height_res: float = 0.05,
        icon_path: str = "",
        custom_data: dict | None = None,
    ) -> None:
        self.name = name
        self.printer_type = printer_type  # "FDM" or "Resin"
        self.bed_x = bed_x
        self.bed_y = bed_y
        self.bed_z = bed_z
        self.max_speed = max_speed
        self.nozzle_diameter = nozzle_diameter
        self.layer_height_res = layer_height_res
        self.icon_path = icon_path
        self.custom_data = custom_data or {}


# ---------------------------------------------------------------------------
# Sample / default profiles used when no FreeCAD param store is available
# ---------------------------------------------------------------------------

_DEFAULT_FDM_PRINTERS = [
    _PrinterProfile("Generic FDM Printer", "FDM", 220, 220, 250, 150, 0.4),
    _PrinterProfile("Prusa i3 MK3S+", "FDM", 250, 210, 210, 200, 0.4),
    _PrinterProfile("Ender 3 V2", "FDM", 220, 220, 250, 150, 0.4),
    _PrinterProfile("Bambu Lab P1S", "FDM", 256, 256, 256, 500, 0.4),
]

_DEFAULT_RESIN_PRINTERS = [
    _PrinterProfile("Generic Resin Printer", "Resin", 120, 68, 150, 40, 0.0, 0.05),
    _PrinterProfile("Elegoo Mars 3", "Resin", 133, 80, 165, 40, 0.0, 0.05),
    _PrinterProfile("Anycubic Photon Mono X", "Resin", 192, 120, 245, 40, 0.0, 0.05),
]


# ---------------------------------------------------------------------------
# Widget
# ---------------------------------------------------------------------------


class PrinterSelectorWidget(QtWidgets.QWidget):
    """Widget for selecting a printer from available profiles.

    Signals
    -------
    printer_changed(str)
        Emitted when the user picks a different printer.  The payload is the
        printer name.
    manage_printers_requested()
        Emitted when the *Manage Printers…* button is clicked.
    """

    printer_changed = QtCore.Signal(str)
    manage_printers_requested = QtCore.Signal()

    # Column indices in the combo box item-data map
    _ROLE_PROFILE = QtCore.Qt.UserRole + 1

    def __init__(self, parent: QtWidgets.QWidget | None = None) -> None:
        super().__init__(parent)

        self._profiles: list[_PrinterProfile] = []
        self._filter_type: str | None = None  # None = show all

        self._ui_setup()
        self._load_profiles()

    # ------------------------------------------------------------------
    # Public helpers
    # ------------------------------------------------------------------

    def set_filter(self, printer_type: str | None) -> None:
        """Filter the dropdown to *'FDM'*, *'Resin'*, or *None* (all)."""
        self._filter_type = printer_type
        self._rebuild_combo()

    def current_printer(self) -> _PrinterProfile | None:
        """Return the currently selected ``_PrinterProfile`` or *None*."""
        idx = self._combo.currentIndex()
        if idx < 0:
            return None
        return self._combo.currentData(self._ROLE_PROFILE)

    def current_printer_name(self) -> str:
        """Return the name of the currently selected printer (empty string if
        nothing is selected)."""
        p = self.current_printer()
        return p.name if p else ""

    def add_printer(self, profile: _PrinterProfile) -> None:
        """Add *profile* to the list and refresh the combo."""
        self._profiles.append(profile)
        self._rebuild_combo()

    def remove_printer(self, name: str) -> None:
        """Remove the first printer matching *name*."""
        self._profiles = [p for p in self._profiles if p.name != name]
        self._rebuild_combo()

    # ------------------------------------------------------------------
    # Internal UI construction
    # ------------------------------------------------------------------

    def _ui_setup(self) -> None:
        root = QtWidgets.QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)

        # --- Combo row ---------------------------------------------------
        combo_row = QtWidgets.QHBoxLayout()
        self._combo = QtWidgets.QComboBox()
        self._combo.setSizePolicy(
            QtWidgets.QSizePolicy.Policy.Expanding,
            QtWidgets.QSizePolicy.Policy.Fixed,
        )
        self._combo.setToolTip("Select a printer profile")
        self._combo.currentIndexChanged.connect(self._on_combo_changed)
        combo_row.addWidget(self._combo, 1)

        self._btn_manage = QtWidgets.QPushButton("Manage Printers…")
        self._btn_manage.setToolTip("Open preferences to add / edit / remove printers")
        self._btn_manage.clicked.connect(self.manage_printers_requested)
        combo_row.addWidget(self._btn_manage)

        root.addLayout(combo_row)

        # --- Details group box -------------------------------------------
        details_group = QtWidgets.QGroupBox("Printer Details")
        details_lay = QtWidgets.QFormLayout(details_group)

        self._lbl_icon = QtWidgets.QLabel()
        self._lbl_icon.setFixedSize(64, 64)
        self._lbl_icon.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
        self._lbl_icon.setStyleSheet("border: 1px solid gray; background: #f0f0f0;")
        details_lay.addRow(self._lbl_icon)

        self._lbl_type = QtWidgets.QLabel("—")
        details_lay.addRow("Type:", self._lbl_type)

        self._lbl_bed = QtWidgets.QLabel("—")
        details_lay.addRow("Bed Size (mm):", self._lbl_bed)

        self._lbl_speed = QtWidgets.QLabel("—")
        details_lay.addRow("Max Speed (mm/s):", self._lbl_speed)

        self._lbl_nozzle = QtWidgets.QLabel("—")
        details_lay.addRow("Nozzle ∅ (mm):", self._lbl_nozzle)

        self._lbl_resolution = QtWidgets.QLabel("—")
        details_lay.addRow("Layer Res. (mm):", self._lbl_resolution)

        root.addWidget(details_group)

    # ------------------------------------------------------------------
    # Profile loading
    # ------------------------------------------------------------------

    def _load_profiles(self) -> None:
        """Attempt to load profiles from FreeCAD param store; fall back to
        built-in defaults."""
        loaded = False
        if _HAS_FREECAD:
            loaded = self._load_profiles_from_freecad()
        if not loaded:
            self._profiles = list(_DEFAULT_FDM_PRINTERS) + list(_DEFAULT_RESIN_PRINTERS)
        self._rebuild_combo()

    def _load_profiles_from_freecad(self) -> bool:
        """Read ``SlicerWorkbench/Printers/<name>`` param groups."""
        try:
            param = FreeCAD.ParamGet(
                "User parameter:BaseApp/Preferences/SlicerWorkbench/Printers"
            )
        except Exception:
            return False

        groups = param.GetGroups()
        if not groups:
            return False

        for grp_name in groups:
            g = param.GetGroup(grp_name)
            profile = _PrinterProfile(
                name=grp_name,
                printer_type=g.GetString("Type", "FDM"),
                bed_x=g.GetFloat("BedX", 220.0),
                bed_y=g.GetFloat("BedY", 220.0),
                bed_z=g.GetFloat("BedZ", 250.0),
                max_speed=g.GetFloat("MaxSpeed", 150.0),
                nozzle_diameter=g.GetFloat("Nozzle", 0.4),
                layer_height_res=g.GetFloat("Resolution", 0.05),
                icon_path=g.GetString("Icon", ""),
            )
            self._profiles.append(profile)
        return True

    # ------------------------------------------------------------------
    # Combo helpers
    # ------------------------------------------------------------------

    def _rebuild_combo(self) -> None:
        self._combo.blockSignals(True)
        self._combo.clear()

        fdm = [p for p in self._profiles if p.printer_type == "FDM"]
        resin = [p for p in self._profiles if p.printer_type == "Resin"]

        show_fdm = self._filter_type in (None, "FDM")
        show_resin = self._filter_type in (None, "Resin")

        if show_fdm and fdm:
            self._combo.insertSeparator(self._combo.count())
            for p in fdm:
                self._add_combo_item(p)

        if show_resin and resin:
            self._combo.insertSeparator(self._combo.count())
            for p in resin:
                self._add_combo_item(p)

        self._combo.setCurrentIndex(0)
        self._combo.blockSignals(False)
        self._refresh_details()

    def _add_combo_item(self, profile: _PrinterProfile) -> None:
        prefix = "🖨" if profile.printer_type == "FDM" else "💡"
        self._combo.addItem(f"{prefix}  {profile.name}", userData=profile)

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------

    def _on_combo_changed(self, _index: int) -> None:
        self._refresh_details()
        name = self.current_printer_name()
        if name:
            self.printer_changed.emit(name)

    def _refresh_details(self) -> None:
        p = self.current_printer()
        if p is None:
            self._lbl_type.setText("—")
            self._lbl_bed.setText("—")
            self._lbl_speed.setText("—")
            self._lbl_nozzle.setText("—")
            self._lbl_resolution.setText("—")
            self._lbl_icon.clear()
            return

        self._lbl_type.setText(p.printer_type)
        self._lbl_bed.setText(f"{p.bed_x:.0f} × {p.bed_y:.0f} × {p.bed_z:.0f}")
        self._lbl_speed.setText(f"{p.max_speed:.0f}")

        if p.printer_type == "FDM":
            self._lbl_nozzle.setText(f"{p.nozzle_diameter:.2f}")
            self._lbl_resolution.setText("—")
        else:
            self._lbl_nozzle.setText("—")
            self._lbl_resolution.setText(f"{p.layer_height_res:.3f}")

        if p.icon_path:
            pixmap = QtGui.QPixmap(p.icon_path).scaled(
                64,
                64,
                QtCore.Qt.AspectRatioMode.KeepAspectRatio,
                QtCore.Qt.TransformationMode.SmoothTransformation,
            )
            self._lbl_icon.setPixmap(pixmap)
        else:
            self._lbl_icon.clear()
            self._lbl_icon.setText("🖨" if p.printer_type == "FDM" else "💡")
