# SPDX-License-Identifier: LGPL-2.1-or-later

"""Material / filament / resin selection widget for SlicerWorkbench."""

from __future__ import annotations

from PySide6 import QtCore, QtGui, QtWidgets

try:
    import FreeCADGui

    _HAS_FREECAD = True
except ImportError:
    _HAS_FREECAD = False


# ---------------------------------------------------------------------------
# Data model
# ---------------------------------------------------------------------------


class _MaterialProfile:
    """Lightweight container for a material profile."""

    def __init__(
        self,
        name: str,
        material_type: str = "PLA",
        category: str = "FDM",
        color: str = "#CCCCCC",
        print_temp: float = 210.0,
        bed_temp: float = 60.0,
        density: float = 1.24,
        price_per_kg: float = 20.0,
        custom_data: dict | None = None,
    ) -> None:
        self.name = name
        self.material_type = material_type
        self.category = category  # "FDM" or "Resin"
        self.color = color
        self.print_temp = print_temp
        self.bed_temp = bed_temp
        self.density = density
        self.price_per_kg = price_per_kg
        self.custom_data = custom_data or {}


# ---------------------------------------------------------------------------
# Built-in defaults
# ---------------------------------------------------------------------------

_DEFAULT_FDM_MATERIALS = [
    _MaterialProfile("PLA", "PLA", "FDM", "#00FF00", 210, 60, 1.24, 20),
    _MaterialProfile("PETG", "PETG", "FDM", "#0088FF", 230, 80, 1.27, 25),
    _MaterialProfile("ABS", "ABS", "FDM", "#FF4444", 240, 100, 1.04, 22),
    _MaterialProfile("TPU", "TPU", "FDM", "#FFAA00", 220, 50, 1.21, 30),
    _MaterialProfile("Nylon", "Nylon", "FDM", "#FFFFFF", 250, 70, 1.14, 35),
    _MaterialProfile("ASA", "ASA", "FDM", "#888888", 240, 100, 1.07, 28),
    _MaterialProfile("PC", "PC", "FDM", "#DDDDDD", 270, 110, 1.20, 32),
]

_DEFAULT_RESIN_MATERIALS = [
    _MaterialProfile("Standard Resin", "Standard", "Resin", "#DDDDDD", 0, 0, 1.10, 30),
    _MaterialProfile("Tough Resin", "Tough", "Resin", "#888888", 0, 0, 1.15, 45),
    _MaterialProfile("Flexible Resin", "Flexible", "Resin", "#0000FF", 0, 0, 1.05, 50),
    _MaterialProfile("Clear Resin", "Clear", "Resin", "#E8E8FF", 0, 0, 1.10, 40),
    _MaterialProfile("Dental Resin", "Dental", "Resin", "#FFEEDD", 0, 0, 1.20, 60),
]


# ---------------------------------------------------------------------------
# Widget
# ---------------------------------------------------------------------------


class MaterialSelectorWidget(QtWidgets.QWidget):
    """Widget for selecting material / filament / resin.

    Signals
    -------
    material_changed(str)
        Emitted when the user picks a different material.
    manage_materials_requested()
        Emitted when *Manage Materials…* is clicked.
    """

    material_changed = QtCore.Signal(str)
    manage_materials_requested = QtCore.Signal()

    _ROLE_MATERIAL = QtCore.Qt.UserRole + 1

    def __init__(self, parent: QtWidgets.QWidget | None = None) -> None:
        super().__init__(parent)

        self._materials: list[_MaterialProfile] = []
        self._filter_category: str | None = None

        self._ui_setup()
        self._load_materials()

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def set_filter(self, category: str | None) -> None:
        """Filter to *'FDM'*, *'Resin'*, or *None* (all)."""
        self._filter_category = category
        self._rebuild_combo()

    def current_material(self) -> _MaterialProfile | None:
        idx = self._combo.currentIndex()
        if idx < 0:
            return None
        return self._combo.currentData(self._ROLE_MATERIAL)

    def current_material_name(self) -> str:
        m = self.current_material()
        return m.name if m else ""

    def add_material(self, profile: _MaterialProfile) -> None:
        self._materials.append(profile)
        self._rebuild_combo()

    def remove_material(self, name: str) -> None:
        self._materials = [m for m in self._materials if m.name != name]
        self._rebuild_combo()

    # ------------------------------------------------------------------
    # UI construction
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
        self._combo.setToolTip("Select a material / filament / resin")
        self._combo.currentIndexChanged.connect(self._on_combo_changed)
        combo_row.addWidget(self._combo, 1)

        self._btn_manage = QtWidgets.QPushButton("Manage Materials…")
        self._btn_manage.setToolTip("Open preferences to add / edit / remove materials")
        self._btn_manage.clicked.connect(self.manage_materials_requested)
        combo_row.addWidget(self._btn_manage)

        root.addLayout(combo_row)

        # --- Properties group -------------------------------------------
        props_group = QtWidgets.QGroupBox("Material Properties")
        props_lay = QtWidgets.QFormLayout(props_group)

        # Color swatch (prominent for resin)
        self._color_swatch = QtWidgets.QLabel()
        self._color_swatch.setFixedSize(48, 48)
        self._color_swatch.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
        self._color_swatch.setStyleSheet(
            "border: 1px solid gray; border-radius: 6px; background: #cccccc;"
        )
        props_lay.addRow("Color:", self._color_swatch)

        self._lbl_type = QtWidgets.QLabel("—")
        props_lay.addRow("Type:", self._lbl_type)

        self._lbl_category = QtWidgets.QLabel("—")
        props_lay.addRow("Category:", self._lbl_category)

        self._lbl_print_temp = QtWidgets.QLabel("—")
        props_lay.addRow("Print Temp (°C):", self._lbl_print_temp)

        self._lbl_bed_temp = QtWidgets.QLabel("—")
        props_lay.addRow("Bed Temp (°C):", self._lbl_bed_temp)

        self._lbl_density = QtWidgets.QLabel("—")
        props_lay.addRow("Density (g/cm³):", self._lbl_density)

        self._lbl_price = QtWidgets.QLabel("—")
        props_lay.addRow("Price ($/kg):", self._lbl_price)

        root.addWidget(props_group)

    # ------------------------------------------------------------------
    # Material loading
    # ------------------------------------------------------------------

    def _load_materials(self) -> None:
        loaded = False
        if _HAS_FREECAD:
            loaded = self._load_from_freecad()
        if not loaded:
            self._materials = list(_DEFAULT_FDM_MATERIALS) + list(
                _DEFAULT_RESIN_MATERIALS
            )
        self._rebuild_combo()

    def _load_from_freecad(self) -> bool:
        try:
            param = FreeCAD.ParamGet(
                "User parameter:BaseApp/Preferences/SlicerWorkbench/Materials"
            )
        except Exception:
            return False

        groups = param.GetGroups()
        if not groups:
            return False

        for grp_name in groups:
            g = param.GetGroup(grp_name)
            mat = _MaterialProfile(
                name=grp_name,
                material_type=g.GetString("Type", "PLA"),
                category=g.GetString("Category", "FDM"),
                color=g.GetString("Color", "#CCCCCC"),
                print_temp=g.GetFloat("PrintTemp", 210.0),
                bed_temp=g.GetFloat("BedTemp", 60.0),
                density=g.GetFloat("Density", 1.24),
                price_per_kg=g.GetFloat("Price", 20.0),
            )
            self._materials.append(mat)
        return True

    # ------------------------------------------------------------------
    # Combo management
    # ------------------------------------------------------------------

    def _rebuild_combo(self) -> None:
        self._combo.blockSignals(True)
        self._combo.clear()

        fdm = [m for m in self._materials if m.category == "FDM"]
        resin = [m for m in self._materials if m.category == "Resin"]

        show_fdm = self._filter_category in (None, "FDM")
        show_resin = self._filter_category in (None, "Resin")

        if show_fdm and fdm:
            self._combo.insertSeparator(self._combo.count())
            for m in fdm:
                self._add_item(m)

        if show_resin and resin:
            self._combo.insertSeparator(self._combo.count())
            for m in resin:
                self._add_item(m)

        self._combo.setCurrentIndex(0)
        self._combo.blockSignals(False)
        self._refresh_details()

    def _add_item(self, mat: _MaterialProfile) -> None:
        icon = self._make_color_icon(mat.color)
        self._combo.addItem(icon, mat.name, userData=mat)

    @staticmethod
    def _make_color_icon(hex_color: str, size: int = 16) -> QtGui.QIcon:
        pixmap = QtGui.QPixmap(size, size)
        pixmap.fill(QtGui.QColor(hex_color))
        painter = QtGui.QPainter(pixmap)
        painter.setPen(QtGui.QPen(QtCore.Qt.GlobalColor.gray, 1))
        painter.drawRect(0, 0, size - 1, size - 1)
        painter.end()
        return QtGui.QIcon(pixmap)

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------

    def _on_combo_changed(self, _index: int) -> None:
        self._refresh_details()
        name = self.current_material_name()
        if name:
            self.material_changed.emit(name)

    def _refresh_details(self) -> None:
        m = self.current_material()
        if m is None:
            self._lbl_type.setText("—")
            self._lbl_category.setText("—")
            self._lbl_print_temp.setText("—")
            self._lbl_bed_temp.setText("—")
            self._lbl_density.setText("—")
            self._lbl_price.setText("—")
            self._color_swatch.setStyleSheet(
                "border: 1px solid gray; border-radius: 6px; background: #cccccc;"
            )
            return

        self._lbl_type.setText(m.material_type)
        self._lbl_category.setText(m.category)
        self._lbl_print_temp.setText(f"{m.print_temp:.0f}" if m.print_temp else "N/A")
        self._lbl_bed_temp.setText(f"{m.bed_temp:.0f}" if m.bed_temp else "N/A")
        self._lbl_density.setText(f"{m.density:.2f}")
        self._lbl_price.setText(f"{m.price_per_kg:.2f}")

        self._color_swatch.setStyleSheet(
            f"border: 1px solid gray; border-radius: 6px; background: {m.color};"
        )
