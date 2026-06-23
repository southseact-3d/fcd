# SPDX-License-Identifier: LGPL-2.1-or-later
"""
Main task panel widget for the SlicerWorkbench.

Provides the right-sidebar UI for configuring and controlling FDM / resin
slicing operations inside FreeCAD.  Integrates with FreeCAD's TaskPanel
system via ``Gui.TaskPanel.makeTaskPanel``.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

import os
from typing import Any, Dict, List, Optional

from PySide6 import QtCore, QtGui, QtWidgets

try:
    import FreeCADGui as Gui
except ImportError:
    Gui = None

from SlicerCore.printer_profiles import (
    FDMProfile,
    FDMaterialProfile,
    Profile,
    ProfileManager,
    ResinMaterialProfile,
    ResinProfile,
)
from SlicerCore.libslic3r_wrapper import LibSlic3rWrapper

# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

_FDM_INFILL_PATTERNS = [
    "grid",
    "lines",
    "gyroid",
    "honeycomb",
    "cubic",
    "concentric",
    "rectilinear",
]

_FDM_OUTPUT_FORMATS = ["G-code"]
_RESIN_OUTPUT_FORMATS = ["CTB", "GOO", "SL1", "Photon"]

_MODE_FDM = "FDM"
_MODE_RESIN = "Resin"


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------


def _make_spinbox(
    min_val: float,
    max_val: float,
    value: float,
    step: float = 1.0,
    decimals: int = 2,
    tooltip: str = "",
    suffix: str = "",
) -> QtWidgets.QDoubleSpinBox:
    sb = QtWidgets.QDoubleSpinBox()
    sb.setRange(min_val, max_val)
    sb.setDecimals(decimals)
    sb.setSingleStep(step)
    sb.setValue(value)
    if suffix:
        sb.setSuffix(suffix)
    if tooltip:
        sb.setToolTip(tooltip)
    return sb


def _make_int_spinbox(
    min_val: int,
    max_val: int,
    value: int,
    step: int = 1,
    tooltip: str = "",
) -> QtWidgets.QSpinBox:
    sb = QtWidgets.QSpinBox()
    sb.setRange(min_val, max_val)
    sb.setSingleStep(step)
    sb.setValue(value)
    if tooltip:
        sb.setToolTip(tooltip)
    return sb


def _make_checkbox(checked: bool = False, tooltip: str = "") -> QtWidgets.QCheckBox:
    cb = QtWidgets.QCheckBox()
    cb.setChecked(checked)
    if tooltip:
        cb.setToolTip(tooltip)
    return cb


def _make_combobox(
    items: List[str],
    current: int = 0,
    tooltip: str = "",
) -> QtWidgets.QComboBox:
    combo = QtWidgets.QComboBox()
    combo.addItems(items)
    combo.setCurrentIndex(current)
    if tooltip:
        combo.setToolTip(tooltip)
    return combo


def _make_group_box(title: str, checkable: bool = False) -> QtWidgets.QGroupBox:
    gb = QtWidgets.QGroupBox(title)
    if checkable:
        gb.setCheckable(True)
    return gb


def _make_h_row(*widgets: QtWidgets.QWidget) -> QtWidgets.QHBoxLayout:
    row = QtWidgets.QHBoxLayout()
    for w in widgets:
        row.addWidget(w)
    return row


def _make_label(text: str, tooltip: str = "") -> QtWidgets.QLabel:
    lbl = QtWidgets.QLabel(text)
    if tooltip:
        lbl.setToolTip(tooltip)
    return lbl


def _browse_directory(
    parent: QtWidgets.QWidget, title: str = "Select Output Directory"
) -> Optional[str]:
    return QtWidgets.QFileDialog.getExistingDirectory(parent, title)


def _browse_file(
    parent: QtWidgets.QWidget,
    title: str,
    filter_str: str = "All Files (*)",
) -> Optional[str]:
    path, _ = QtWidgets.QFileDialog.getOpenFileName(parent, title, "", filter_str)
    return path if path else None


# ---------------------------------------------------------------------------
# Main task panel
# ---------------------------------------------------------------------------


class SlicerTaskPanel(QtWidgets.QWidget):
    """Primary slicer settings widget displayed in the FreeCAD task panel.

    Collects all FDM / Resin settings into an organised, scrollable layout
    and exposes a clean API for reading, writing, and reacting to changes.
    """

    slice_requested = QtCore.Signal(dict)
    export_requested = QtCore.Signal(str)
    settings_changed = QtCore.Signal(dict)

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self.setObjectName("SlicerTaskPanel")
        self.setMinimumWidth(320)

        self._profile_manager = ProfileManager()
        self._prusa_wrapper = LibSlic3rWrapper()
        self._current_mode: str = _MODE_FDM
        self._suppress_signals: bool = False

        self._build_ui()
        self._connect_signals()
        self._load_profiles()

    # ------------------------------------------------------------------
    # UI construction -- top level
    # ------------------------------------------------------------------

    def _build_ui(self) -> None:
        root = QtWidgets.QVBoxLayout(self)
        root.setContentsMargins(4, 4, 4, 4)
        root.setSpacing(4)

        scroll = QtWidgets.QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        root.addWidget(scroll, 1)

        container = QtWidgets.QWidget()
        self._main_layout = QtWidgets.QVBoxLayout(container)
        self._main_layout.setContentsMargins(0, 0, 0, 0)
        self._main_layout.setSpacing(4)
        scroll.setWidget(container)

        self._main_layout.addWidget(self._build_mode_section())
        self._main_layout.addWidget(self._build_printer_section())
        self._main_layout.addWidget(self._build_material_section())
        self._main_layout.addWidget(self._build_engine_section())

        self._fdm_settings_widget = self._build_fdm_settings()
        self._main_layout.addWidget(self._fdm_settings_widget)

        self._resin_settings_widget = self._build_resin_settings()
        self._main_layout.addWidget(self._resin_settings_widget)

        self._main_layout.addWidget(self._build_output_section())
        self._main_layout.addWidget(self._build_action_buttons())
        self._main_layout.addWidget(self._build_progress_section())
        self._main_layout.addStretch(1)

        self._update_mode_visibility()

    # ------------------------------------------------------------------
    # Mode section
    # ------------------------------------------------------------------

    def _build_mode_section(self) -> QtWidgets.QGroupBox:
        gb = _make_group_box("Mode")
        layout = QtWidgets.QHBoxLayout()

        self._fdm_radio = QtWidgets.QRadioButton(_MODE_FDM)
        self._fdm_radio.setToolTip(
            "Fused Deposition Modeling -- filament-based printing"
        )
        self._fdm_radio.setChecked(True)

        self._resin_radio = QtWidgets.QRadioButton(_MODE_RESIN)
        self._resin_radio.setToolTip("MSLA / DLP / SLA -- resin-based printing")

        mode_group = QtWidgets.QButtonGroup(self)
        mode_group.addButton(self._fdm_radio)
        mode_group.addButton(self._resin_radio)
        self._mode_group = mode_group

        layout.addWidget(self._fdm_radio)
        layout.addWidget(self._resin_radio)
        layout.addStretch()
        gb.setLayout(layout)
        return gb

    # ------------------------------------------------------------------
    # Printer section
    # ------------------------------------------------------------------

    def _build_printer_section(self) -> QtWidgets.QGroupBox:
        gb = _make_group_box("Printer")
        layout = QtWidgets.QVBoxLayout()

        row = QtWidgets.QHBoxLayout()
        row.addWidget(_make_label("Printer:"))
        self._printer_combo = _make_combobox([], tooltip="Select a printer profile")
        row.addWidget(self._printer_combo, 1)
        layout.addLayout(row)

        self._bed_info_label = _make_label("")
        self._bed_info_label.setStyleSheet("color: grey; font-size: 10px;")
        layout.addWidget(self._bed_info_label)

        gb.setLayout(layout)
        return gb

    # ------------------------------------------------------------------
    # Material section
    # ------------------------------------------------------------------

    def _build_material_section(self) -> QtWidgets.QGroupBox:
        gb = _make_group_box("Material")
        layout = QtWidgets.QVBoxLayout()

        row = QtWidgets.QHBoxLayout()
        row.addWidget(_make_label("Material:"))
        self._material_combo = _make_combobox(
            [], tooltip="Select a material / filament profile"
        )
        row.addWidget(self._material_combo, 1)
        layout.addLayout(row)

        gb.setLayout(layout)
        return gb

    # ------------------------------------------------------------------
    # Engine selector (FDM only)
    # ------------------------------------------------------------------

    def _build_engine_section(self) -> QtWidgets.QGroupBox:
        gb = _make_group_box("Slicing Engine")
        self._engine_group_box = gb
        layout = QtWidgets.QVBoxLayout()

        self._builtin_radio = QtWidgets.QRadioButton("Built-in Slicer")
        self._builtin_radio.setToolTip("Use the built-in Python slicing engine")
        self._builtin_radio.setChecked(True)

        self._prusa_radio = QtWidgets.QRadioButton("PrusaSlicer (libslic3r)")
        self._prusa_radio.setToolTip(
            "Use PrusaSlicer CLI as the slicing backend "
            "(requires PrusaSlicer to be installed)"
        )

        engine_btn_group = QtWidgets.QButtonGroup(self)
        engine_btn_group.addButton(self._builtin_radio)
        engine_btn_group.addButton(self._prusa_radio)
        self._engine_group = engine_btn_group

        layout.addWidget(self._builtin_radio)
        layout.addWidget(self._prusa_radio)

        self._prusa_path_widget = QtWidgets.QWidget()
        prusa_layout = QtWidgets.QHBoxLayout(self._prusa_path_widget)
        prusa_layout.setContentsMargins(20, 0, 0, 0)
        self._prusa_path_edit = QtWidgets.QLineEdit()
        self._prusa_path_edit.setPlaceholderText("Path to PrusaSlicer binary")
        self._prusa_path_edit.setToolTip("Full path to the PrusaSlicer executable")
        self._prusa_browse_btn = QtWidgets.QPushButton("Browse...")
        self._prusa_browse_btn.setMaximumWidth(60)
        prusa_layout.addWidget(self._prusa_path_edit, 1)
        prusa_layout.addWidget(self._prusa_browse_btn)
        layout.addWidget(self._prusa_path_widget)
        self._prusa_path_widget.setVisible(False)

        self._prusa_status_label = QtWidgets.QLabel("")
        self._prusa_status_label.setStyleSheet("font-size: 10px;")
        layout.addWidget(self._prusa_status_label)

        if self._prusa_wrapper.is_available():
            version = self._prusa_wrapper.get_version()
            self._prusa_status_label.setText(f"PrusaSlicer detected -- {version}")
            self._prusa_status_label.setStyleSheet("color: green; font-size: 10px;")
        else:
            self._prusa_status_label.setText("PrusaSlicer not found on this system")
            self._prusa_status_label.setStyleSheet("color: red; font-size: 10px;")
            self._prusa_radio.setEnabled(False)

        gb.setLayout(layout)
        return gb

    # ------------------------------------------------------------------
    # FDM settings (tabbed)
    # ------------------------------------------------------------------

    def _build_fdm_settings(self) -> QtWidgets.QGroupBox:
        gb = _make_group_box("FDM Print Settings")
        self._fdm_group = gb

        self._fdm_tabs = QtWidgets.QTabWidget()
        self._fdm_tabs.setTabPosition(QtWidgets.QTabWidget.North)

        self._fdm_tabs.addTab(self._build_quality_tab(), "Quality")
        self._fdm_tabs.addTab(self._build_walls_tab(), "Walls")
        self._fdm_tabs.addTab(self._build_infill_tab(), "Infill")
        self._fdm_tabs.addTab(self._build_speed_tab(), "Speed")
        self._fdm_tabs.addTab(self._build_fdm_support_tab(), "Support")
        self._fdm_tabs.addTab(self._build_advanced_tab(), "Advanced")

        gb_layout = QtWidgets.QVBoxLayout()
        gb_layout.addWidget(self._fdm_tabs)
        gb.setLayout(gb_layout)
        return gb

    def _build_quality_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._fdm_layer_height = _make_spinbox(
            0.05,
            1.0,
            0.2,
            0.05,
            tooltip="Layer height in mm (0.05-1.0)",
        )
        self._fdm_first_layer_height = _make_spinbox(
            0.1,
            1.0,
            0.3,
            0.05,
            tooltip=(
                "First layer height in mm (0.1-1.0). "
                "A thicker first layer improves bed adhesion."
            ),
        )
        self._fdm_nozzle_diameter = _make_spinbox(
            0.2,
            1.2,
            0.4,
            0.1,
            decimals=2,
            tooltip=(
                "Nozzle diameter in mm. "
                "Must match the installed nozzle on your printer."
            ),
        )

        layout.addRow("Layer height (mm):", self._fdm_layer_height)
        layout.addRow("First layer height (mm):", self._fdm_first_layer_height)
        layout.addRow("Nozzle diameter (mm):", self._fdm_nozzle_diameter)
        return w

    def _build_walls_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._fdm_wall_count = _make_int_spinbox(
            1,
            10,
            3,
            tooltip="Number of perimeter walls (1-10)",
        )
        self._fdm_wall_line_width = _make_spinbox(
            0.2,
            1.0,
            0.4,
            0.05,
            decimals=2,
            tooltip="Width of each wall extrusion line in mm",
        )
        self._fdm_top_layers = _make_int_spinbox(
            1,
            20,
            4,
            tooltip="Number of solid top layers",
        )
        self._fdm_bottom_layers = _make_int_spinbox(
            1,
            20,
            4,
            tooltip="Number of solid bottom layers",
        )

        layout.addRow("Wall count:", self._fdm_wall_count)
        layout.addRow("Wall line width (mm):", self._fdm_wall_line_width)
        layout.addRow("Top layers:", self._fdm_top_layers)
        layout.addRow("Bottom layers:", self._fdm_bottom_layers)
        return w

    def _build_infill_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._fdm_infill_percentage = _make_spinbox(
            0.0,
            100.0,
            20.0,
            5.0,
            decimals=0,
            suffix="%",
            tooltip="Infill density as a percentage (0-100%)",
        )
        self._fdm_infill_pattern = _make_combobox(
            _FDM_INFILL_PATTERNS,
            tooltip="Pattern used for internal infill",
        )

        layout.addRow("Infill density:", self._fdm_infill_percentage)
        layout.addRow("Infill pattern:", self._fdm_infill_pattern)
        return w

    def _build_speed_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._fdm_print_speed = _make_spinbox(
            10.0,
            300.0,
            60.0,
            5.0,
            decimals=0,
            suffix=" mm/s",
            tooltip="Printing speed in mm/s (10-300)",
        )
        self._fdm_travel_speed = _make_spinbox(
            10.0,
            500.0,
            150.0,
            10.0,
            decimals=0,
            suffix=" mm/s",
            tooltip="Travel (non-print) move speed in mm/s (10-500)",
        )
        self._fdm_first_layer_speed = _make_spinbox(
            5.0,
            100.0,
            20.0,
            5.0,
            decimals=0,
            suffix=" mm/s",
            tooltip=(
                "Speed for the first layer in mm/s (5-100). "
                "Lower values improve adhesion."
            ),
        )

        layout.addRow("Print speed:", self._fdm_print_speed)
        layout.addRow("Travel speed:", self._fdm_travel_speed)
        layout.addRow("First layer speed:", self._fdm_first_layer_speed)
        return w

    def _build_fdm_support_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._fdm_support_enabled = _make_checkbox(
            tooltip="Enable automatic support generation",
        )
        self._fdm_support_angle = _make_spinbox(
            30.0,
            70.0,
            45.0,
            5.0,
            decimals=0,
            suffix=" deg",
            tooltip=(
                "Overhang angle threshold for supports (30-70 deg). "
                "Lower values add more support."
            ),
        )
        self._fdm_support_density = _make_spinbox(
            5.0,
            50.0,
            15.0,
            5.0,
            decimals=0,
            suffix="%",
            tooltip="Support structure density (5-50%)",
        )
        self._fdm_tree_support = _make_checkbox(
            tooltip="Use tree-style supports instead of grid columns",
        )

        layout.addRow("Enable support:", self._fdm_support_enabled)
        layout.addRow("Support angle:", self._fdm_support_angle)
        layout.addRow("Support density:", self._fdm_support_density)
        layout.addRow("Tree support:", self._fdm_tree_support)
        return w

    def _build_advanced_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._fdm_temperature = _make_spinbox(
            150.0,
            300.0,
            210.0,
            5.0,
            decimals=0,
            suffix=" C",
            tooltip="Nozzle temperature in degrees C (150-300)",
        )
        self._fdm_bed_temperature = _make_spinbox(
            0.0,
            120.0,
            60.0,
            5.0,
            decimals=0,
            suffix=" C",
            tooltip=(
                "Heated bed temperature in degrees C (0-120). "
                "Set to 0 for no heated bed."
            ),
        )
        self._fdm_retraction_enabled = _make_checkbox(
            checked=True,
            tooltip="Enable filament retraction to reduce stringing",
        )
        self._fdm_retraction_distance = _make_spinbox(
            0.0,
            30.0,
            5.0,
            0.5,
            decimals=1,
            suffix=" mm",
            tooltip="Retraction distance in mm",
        )
        self._fdm_retraction_speed = _make_spinbox(
            5.0,
            100.0,
            45.0,
            5.0,
            decimals=0,
            suffix=" mm/s",
            tooltip="Retraction speed in mm/s",
        )
        self._fdm_fan_speed = _make_spinbox(
            0.0,
            100.0,
            100.0,
            5.0,
            decimals=0,
            suffix="%",
            tooltip="Part cooling fan speed (0-100%)",
        )

        layout.addRow("Temperature (C):", self._fdm_temperature)
        layout.addRow("Bed temp (C):", self._fdm_bed_temperature)
        layout.addRow("Retraction:", self._fdm_retraction_enabled)
        layout.addRow("Retraction dist (mm):", self._fdm_retraction_distance)
        layout.addRow("Retraction speed:", self._fdm_retraction_speed)
        layout.addRow("Fan speed:", self._fdm_fan_speed)
        return w

    # ------------------------------------------------------------------
    # Resin settings (tabbed)
    # ------------------------------------------------------------------

    def _build_resin_settings(self) -> QtWidgets.QGroupBox:
        gb = _make_group_box("Resin Print Settings")
        self._resin_group = gb

        self._resin_tabs = QtWidgets.QTabWidget()
        self._resin_tabs.setTabPosition(QtWidgets.QTabWidget.North)

        self._resin_tabs.addTab(self._build_resin_layer_tab(), "Layer")
        self._resin_tabs.addTab(self._build_resin_exposure_tab(), "Exposure")
        self._resin_tabs.addTab(self._build_resin_lift_tab(), "Lift")
        self._resin_tabs.addTab(self._build_resin_quality_tab(), "Quality")
        self._resin_tabs.addTab(self._build_resin_support_tab(), "Support")

        gb_layout = QtWidgets.QVBoxLayout()
        gb_layout.addWidget(self._resin_tabs)
        gb.setLayout(gb_layout)
        return gb

    def _build_resin_layer_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._resin_layer_height = _make_spinbox(
            0.01,
            0.2,
            0.05,
            0.01,
            decimals=3,
            suffix=" mm",
            tooltip=(
                "Layer height in mm (0.01-0.2). "
                "Thinner layers give finer detail but take longer."
            ),
        )
        self._resin_bottom_layer_count = _make_int_spinbox(
            1,
            20,
            5,
            tooltip=(
                "Number of bottom (burn-in) layers (1-20). "
                "These use longer exposure for bed adhesion."
            ),
        )
        self._resin_bottom_layer_height = _make_spinbox(
            0.01,
            0.2,
            0.05,
            0.01,
            decimals=3,
            suffix=" mm",
            tooltip="Height of each bottom layer in mm (0.01-0.2)",
        )

        layout.addRow("Layer height:", self._resin_layer_height)
        layout.addRow("Bottom layer count:", self._resin_bottom_layer_count)
        layout.addRow("Bottom layer height:", self._resin_bottom_layer_height)
        return w

    def _build_resin_exposure_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._resin_exposure_time = _make_spinbox(
            0.5,
            30.0,
            2.5,
            0.5,
            suffix=" s",
            tooltip=(
                "Per-layer exposure time in seconds (0.5-30). "
                "Depends on resin and printer power."
            ),
        )
        self._resin_bottom_exposure_time = _make_spinbox(
            5.0,
            120.0,
            30.0,
            1.0,
            suffix=" s",
            tooltip=(
                "Bottom layer exposure time in seconds (5-120). "
                "Usually much longer than normal exposure."
            ),
        )
        self._resin_light_off_delay = _make_spinbox(
            0.0,
            10.0,
            1.0,
            0.5,
            suffix=" s",
            tooltip="Delay after the light turns off before lifting (0-10 s)",
        )

        layout.addRow("Exposure time:", self._resin_exposure_time)
        layout.addRow("Bottom exposure:", self._resin_bottom_exposure_time)
        layout.addRow("Light-off delay:", self._resin_light_off_delay)
        return w

    def _build_resin_lift_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._resin_lift_height = _make_spinbox(
            1.0,
            10.0,
            5.0,
            0.5,
            suffix=" mm",
            tooltip="How far the build plate lifts after each layer (1-10 mm)",
        )
        self._resin_lift_speed = _make_spinbox(
            0.5,
            10.0,
            1.0,
            0.5,
            suffix=" mm/s",
            tooltip="Speed at which the build plate lifts (0.5-10 mm/s)",
        )
        self._resin_retract_speed = _make_spinbox(
            0.5,
            10.0,
            1.0,
            0.5,
            suffix=" mm/s",
            tooltip="Speed at which the build plate lowers back (0.5-10 mm/s)",
        )

        layout.addRow("Lift height:", self._resin_lift_height)
        layout.addRow("Lift speed:", self._resin_lift_speed)
        layout.addRow("Retract speed:", self._resin_retract_speed)
        return w

    def _build_resin_quality_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._resin_anti_aliasing = _make_checkbox(
            checked=True,
            tooltip="Enable anti-aliasing to smooth layer edges",
        )
        self._resin_anti_aliasing_level = _make_int_spinbox(
            1,
            8,
            4,
            tooltip="Anti-aliasing level (1=off, 2-8 higher = smoother)",
        )

        layout.addRow("Anti-aliasing:", self._resin_anti_aliasing)
        layout.addRow("AA level:", self._resin_anti_aliasing_level)
        return w

    def _build_resin_support_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._resin_support_enabled = _make_checkbox(
            checked=True,
            tooltip="Enable auto-generated support structures",
        )
        self._resin_support_point_density = _make_spinbox(
            0.1,
            2.0,
            0.7,
            0.1,
            decimals=1,
            tooltip=(
                "Density of support touch-points (0.1-2.0). "
                "Higher values give more contact points."
            ),
        )
        self._resin_support_tip_diameter = _make_spinbox(
            0.1,
            1.0,
            0.4,
            0.05,
            decimals=2,
            suffix=" mm",
            tooltip="Diameter of each support tip where it contacts the model (0.1-1.0 mm)",
        )
        self._resin_support_base_diameter = _make_spinbox(
            0.5,
            3.0,
            1.0,
            0.1,
            decimals=1,
            suffix=" mm",
            tooltip="Diameter of the support base on the build plate (0.5-3.0 mm)",
        )

        layout.addRow("Enable support:", self._resin_support_enabled)
        layout.addRow("Point density:", self._resin_support_point_density)
        layout.addRow("Tip diameter:", self._resin_support_tip_diameter)
        layout.addRow("Base diameter:", self._resin_support_base_diameter)
        return w

    # ------------------------------------------------------------------
    # Output section
    # ------------------------------------------------------------------

    def _build_output_section(self) -> QtWidgets.QGroupBox:
        gb = _make_group_box("Output")
        layout = QtWidgets.QFormLayout(gb)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._output_format_combo = _make_combobox(
            _FDM_OUTPUT_FORMATS,
            tooltip="Output file format",
        )

        path_row = QtWidgets.QWidget()
        path_layout = QtWidgets.QHBoxLayout(path_row)
        path_layout.setContentsMargins(0, 0, 0, 0)
        self._output_dir_edit = QtWidgets.QLineEdit()
        self._output_dir_edit.setPlaceholderText("Output directory")
        self._output_dir_edit.setToolTip("Directory where sliced files will be saved")
        self._output_browse_btn = QtWidgets.QPushButton("Browse...")
        self._output_browse_btn.setMaximumWidth(60)
        path_layout.addWidget(self._output_dir_edit, 1)
        path_layout.addWidget(self._output_browse_btn)

        self._estimate_time_label = _make_label("--")
        self._estimate_time_label.setToolTip("Estimated total print time")
        self._estimate_material_label = _make_label("--")
        self._estimate_material_label.setToolTip("Estimated material usage")

        layout.addRow("Format:", self._output_format_combo)
        layout.addRow("Output dir:", path_row)
        layout.addRow("Est. time:", self._estimate_time_label)
        layout.addRow("Est. material:", self._estimate_material_label)
        return gb

    # ------------------------------------------------------------------
    # Action buttons
    # ------------------------------------------------------------------

    def _build_action_buttons(self) -> QtWidgets.QGroupBox:
        gb = _make_group_box("Actions")
        layout = QtWidgets.QHBoxLayout()

        self._slice_btn = QtWidgets.QPushButton("Slice")
        self._slice_btn.setToolTip("Start slicing the selected mesh")
        self._slice_btn.setMinimumHeight(32)
        self._slice_btn.setStyleSheet(
            "QPushButton { font-weight: bold; background-color: #4a90d9; color: white; }"
            "QPushButton:hover { background-color: #5aa0e9; }"
        )

        self._preview_btn = QtWidgets.QPushButton("Preview")
        self._preview_btn.setToolTip("Open layer / G-code preview (after slicing)")
        self._preview_btn.setEnabled(False)

        self._export_btn = QtWidgets.QPushButton("Export")
        self._export_btn.setToolTip("Export sliced output to the selected directory")
        self._export_btn.setEnabled(False)

        layout.addWidget(self._slice_btn)
        layout.addWidget(self._preview_btn)
        layout.addWidget(self._export_btn)
        gb.setLayout(layout)
        return gb

    # ------------------------------------------------------------------
    # Progress bar
    # ------------------------------------------------------------------

    def _build_progress_section(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QVBoxLayout(w)
        layout.setContentsMargins(0, 0, 0, 0)

        self._progress_bar = QtWidgets.QProgressBar()
        self._progress_bar.setRange(0, 100)
        self._progress_bar.setValue(0)
        self._progress_bar.setVisible(False)
        self._progress_bar.setTextVisible(True)

        self._progress_label = QtWidgets.QLabel("")
        self._progress_label.setStyleSheet("color: grey; font-size: 10px;")
        self._progress_label.setVisible(False)

        layout.addWidget(self._progress_bar)
        layout.addWidget(self._progress_label)
        return w

    # ------------------------------------------------------------------
    # Profile loading
    # ------------------------------------------------------------------

    def _load_profiles(self) -> None:
        self._suppress_signals = True
        self._printer_combo.clear()
        self._material_combo.clear()

        if self._current_mode == _MODE_FDM:
            printers = self._profile_manager.get_fdm_printers()
            materials = self._profile_manager.get_fdm_materials()
        else:
            printers = self._profile_manager.get_resin_printers()
            materials = self._profile_manager.get_resin_materials()

        self._fdm_printers: List[Profile] = printers
        self._fdm_materials: List[Profile] = materials

        for p in printers:
            self._printer_combo.addItem(p.name)
        for m in materials:
            self._material_combo.addItem(m.name)

        self._suppress_signals = False

        if printers:
            self._on_printer_changed(0)
        if materials:
            self._on_material_changed(0)

    # ------------------------------------------------------------------
    # Signal wiring
    # ------------------------------------------------------------------

    def _connect_signals(self) -> None:
        self._fdm_radio.toggled.connect(self._on_mode_changed)
        self._prusa_radio.toggled.connect(self._on_engine_changed)
        self._prusa_browse_btn.clicked.connect(self._on_prusa_browse)
        self._printer_combo.currentIndexChanged.connect(self._on_printer_changed)
        self._material_combo.currentIndexChanged.connect(self._on_material_changed)
        self._output_browse_btn.clicked.connect(self._on_browse_output)
        self._slice_btn.clicked.connect(self._on_slice_clicked)
        self._preview_btn.clicked.connect(self._on_preview_clicked)
        self._export_btn.clicked.connect(self._on_export_clicked)
        self._resin_anti_aliasing.toggled.connect(self._on_resin_aa_toggled)

        # Wire every value-changed signal to _on_settings_changed
        for sb_name in (
            "_fdm_layer_height",
            "_fdm_first_layer_height",
            "_fdm_nozzle_diameter",
            "_fdm_wall_count",
            "_fdm_wall_line_width",
            "_fdm_top_layers",
            "_fdm_bottom_layers",
            "_fdm_infill_percentage",
            "_fdm_print_speed",
            "_fdm_travel_speed",
            "_fdm_first_layer_speed",
            "_fdm_support_angle",
            "_fdm_support_density",
            "_fdm_temperature",
            "_fdm_bed_temperature",
            "_fdm_retraction_distance",
            "_fdm_retraction_speed",
            "_fdm_fan_speed",
            "_resin_layer_height",
            "_resin_bottom_layer_count",
            "_resin_bottom_layer_height",
            "_resin_exposure_time",
            "_resin_bottom_exposure_time",
            "_resin_light_off_delay",
            "_resin_lift_height",
            "_resin_lift_speed",
            "_resin_retract_speed",
            "_resin_anti_aliasing_level",
            "_resin_support_point_density",
            "_resin_support_tip_diameter",
            "_resin_support_base_diameter",
        ):
            sb = getattr(self, sb_name, None)
            if sb is not None:
                sb.valueChanged.connect(self._on_settings_changed)

        for cb_name in (
            "_fdm_support_enabled",
            "_fdm_tree_support",
            "_fdm_retraction_enabled",
            "_resin_support_enabled",
            "_resin_anti_aliasing",
        ):
            cb = getattr(self, cb_name, None)
            if cb is not None:
                cb.stateChanged.connect(self._on_settings_changed)

        self._fdm_infill_pattern.currentIndexChanged.connect(self._on_settings_changed)
        self._output_format_combo.currentIndexChanged.connect(self._on_settings_changed)

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------

    def _on_mode_changed(self, checked: bool) -> None:
        if self._suppress_signals:
            return
        self._current_mode = _MODE_RESIN if self._resin_radio.isChecked() else _MODE_FDM
        self._update_mode_visibility()
        self._load_profiles()
        self._on_settings_changed()

    def _update_mode_visibility(self) -> None:
        is_fdm = self._current_mode == _MODE_FDM
        self._fdm_settings_widget.setVisible(is_fdm)
        self._resin_settings_widget.setVisible(not is_fdm)
        self._engine_group_box.setVisible(is_fdm)
        self._update_output_formats()

    def _update_output_formats(self) -> None:
        self._suppress_signals = True
        current = self._output_format_combo.currentText()
        self._output_format_combo.clear()
        if self._current_mode == _MODE_FDM:
            self._output_format_combo.addItems(_FDM_OUTPUT_FORMATS)
        else:
            self._output_format_combo.addItems(_RESIN_OUTPUT_FORMATS)
        idx = self._output_format_combo.findText(current)
        if idx >= 0:
            self._output_format_combo.setCurrentIndex(idx)
        self._suppress_signals = False

    def _on_printer_changed(self, index: int) -> None:
        if self._suppress_signals or index < 0:
            return
        if index >= len(self._fdm_printers):
            return
        profile = self._fdm_printers[index]
        if isinstance(profile, FDMProfile):
            bed = profile.build_volume
            info = (
                f"{profile.manufacturer} {profile.model}  |  "
                f"Bed: {bed[0]:.0f} x {bed[1]:.0f} x {bed[2]:.0f} mm  |  "
                f"Nozzle: {profile.nozzle_diameter} mm"
            )
            self._bed_info_label.setText(info)
            self._fdm_nozzle_diameter.setValue(profile.nozzle_diameter)
        elif isinstance(profile, ResinProfile):
            area = profile.build_area
            info = (
                f"{profile.manufacturer} {profile.model}  |  "
                f"Area: {area[0]:.1f} x {area[1]:.1f} mm  |  "
                f"Height: {profile.build_height:.0f} mm"
            )
            self._bed_info_label.setText(info)
        self._on_settings_changed()

    def _on_material_changed(self, index: int) -> None:
        if self._suppress_signals or index < 0:
            return
        if index >= len(self._fdm_materials):
            return
        profile = self._fdm_materials[index]
        if isinstance(profile, FDMaterialProfile):
            mid = max(profile.temperature_range)
            self._fdm_temperature.setValue(mid)
            bed_mid = max(profile.bed_temperature_range)
            self._fdm_bed_temperature.setValue(bed_mid)
            self._fdm_retraction_distance.setValue(profile.retract_distance)
            self._fdm_retraction_speed.setValue(profile.retract_speed)
        elif isinstance(profile, ResinMaterialProfile):
            self._resin_exposure_time.setValue(profile.exposure_time)
            self._resin_bottom_exposure_time.setValue(profile.bottom_exposure_time)
            self._resin_bottom_layer_count.setValue(profile.bottom_layer_count)
            self._resin_layer_height.setValue(profile.layer_height)
            self._resin_light_off_delay.setValue(profile.light_off_delay)
            self._resin_lift_height.setValue(profile.lift_height)
            self._resin_lift_speed.setValue(profile.lift_speed)
            self._resin_retract_speed.setValue(profile.retract_speed)
        self._on_settings_changed()

    def _on_engine_changed(self, checked: bool) -> None:
        show_prusa = self._prusa_radio.isChecked()
        self._prusa_path_widget.setVisible(show_prusa)

    def _on_prusa_browse(self) -> None:
        path = _browse_file(
            self,
            "Select PrusaSlicer Executable",
            "Executables (*.exe);;All Files (*)",
        )
        if path:
            self._prusa_path_edit.setText(path)

    def _on_browse_output(self) -> None:
        path = _browse_directory(self, "Select Output Directory")
        if path:
            self._output_dir_edit.setText(path)

    def _on_resin_aa_toggled(self, state: int) -> None:
        self._resin_anti_aliasing_level.setEnabled(
            self._resin_anti_aliasing.isChecked()
        )

    def _on_slice_clicked(self) -> None:
        settings = self.get_settings()
        self.show_progress(0)
        self._slice_btn.setEnabled(False)
        self._preview_btn.setEnabled(False)
        self.slice_requested.emit(settings)

    def _on_preview_clicked(self) -> None:
        if Gui is not None:
            try:
                Gui.Control.showDialog("SlicerLayerPreview")
            except Exception:
                pass

    def _on_export_clicked(self) -> None:
        output_dir = self._output_dir_edit.text().strip()
        if not output_dir:
            QtWidgets.QMessageBox.warning(
                self,
                "No Output Directory",
                "Please select an output directory first.",
            )
            return
        self.export_requested.emit(output_dir)

    def _on_settings_changed(self) -> None:
        if self._suppress_signals:
            return
        self.settings_changed.emit(self.get_settings())

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def get_settings(self) -> Dict[str, Any]:
        """Collect all current UI values into a flat settings dictionary."""
        settings: Dict[str, Any] = {
            "mode": self._current_mode,
            "printer_index": self._printer_combo.currentIndex(),
            "material_index": self._material_combo.currentIndex(),
            "output_format": self._output_format_combo.currentText(),
            "output_dir": self._output_dir_edit.text().strip(),
            "engine": "prusa" if self._prusa_radio.isChecked() else "builtin",
            "prusa_path": self._prusa_path_edit.text().strip(),
        }

        if self._current_mode == _MODE_FDM:
            settings.update(
                {
                    "layer_height": self._fdm_layer_height.value(),
                    "first_layer_height": self._fdm_first_layer_height.value(),
                    "nozzle_diameter": self._fdm_nozzle_diameter.value(),
                    "wall_count": self._fdm_wall_count.value(),
                    "wall_line_width": self._fdm_wall_line_width.value(),
                    "top_layers": self._fdm_top_layers.value(),
                    "bottom_layers": self._fdm_bottom_layers.value(),
                    "infill_percentage": self._fdm_infill_percentage.value(),
                    "infill_pattern": self._fdm_infill_pattern.currentText(),
                    "print_speed": self._fdm_print_speed.value(),
                    "travel_speed": self._fdm_travel_speed.value(),
                    "first_layer_speed": self._fdm_first_layer_speed.value(),
                    "support_enabled": self._fdm_support_enabled.isChecked(),
                    "support_angle": self._fdm_support_angle.value(),
                    "support_density": self._fdm_support_density.value(),
                    "support_tree": self._fdm_tree_support.isChecked(),
                    "temperature": self._fdm_temperature.value(),
                    "bed_temperature": self._fdm_bed_temperature.value(),
                    "retraction_enabled": self._fdm_retraction_enabled.isChecked(),
                    "retraction_distance": self._fdm_retraction_distance.value(),
                    "retraction_speed": self._fdm_retraction_speed.value(),
                    "fan_speed": self._fdm_fan_speed.value(),
                }
            )
        else:
            settings.update(
                {
                    "layer_height": self._resin_layer_height.value(),
                    "bottom_layer_count": self._resin_bottom_layer_count.value(),
                    "bottom_layer_height": self._resin_bottom_layer_height.value(),
                    "exposure_time": self._resin_exposure_time.value(),
                    "bottom_exposure_time": self._resin_bottom_exposure_time.value(),
                    "light_off_delay": self._resin_light_off_delay.value(),
                    "lift_height": self._resin_lift_height.value(),
                    "lift_speed": self._resin_lift_speed.value(),
                    "retract_speed": self._resin_retract_speed.value(),
                    "anti_aliasing": self._resin_anti_aliasing.isChecked(),
                    "anti_aliasing_level": self._resin_anti_aliasing_level.value(),
                    "support_enabled": self._resin_support_enabled.isChecked(),
                    "support_point_density": self._resin_support_point_density.value(),
                    "support_tip_diameter": self._resin_support_tip_diameter.value(),
                    "support_base_diameter": self._resin_support_base_diameter.value(),
                }
            )

        return settings

    def set_settings(self, settings_dict: Dict[str, Any]) -> None:
        """Populate the UI from a settings dictionary."""
        self._suppress_signals = True
        try:
            mode = settings_dict.get("mode", _MODE_FDM)
            if mode == _MODE_RESIN:
                self._resin_radio.setChecked(True)
            else:
                self._fdm_radio.setChecked(True)
            self._current_mode = mode
            self._update_mode_visibility()
            self._load_profiles()

            pi = settings_dict.get("printer_index", -1)
            if 0 <= pi < self._printer_combo.count():
                self._printer_combo.setCurrentIndex(pi)
            mi = settings_dict.get("material_index", -1)
            if 0 <= mi < self._material_combo.count():
                self._material_combo.setCurrentIndex(mi)

            engine = settings_dict.get("engine", "builtin")
            if engine == "prusa" and self._prusa_radio.isEnabled():
                self._prusa_radio.setChecked(True)
            else:
                self._builtin_radio.setChecked(True)
            self._prusa_path_edit.setText(settings_dict.get("prusa_path", ""))

            fmt = settings_dict.get("output_format", "")
            idx = self._output_format_combo.findText(fmt)
            if idx >= 0:
                self._output_format_combo.setCurrentIndex(idx)
            self._output_dir_edit.setText(settings_dict.get("output_dir", ""))

            if self._current_mode == _MODE_FDM:
                self._fdm_layer_height.setValue(settings_dict.get("layer_height", 0.2))
                self._fdm_first_layer_height.setValue(
                    settings_dict.get("first_layer_height", 0.3)
                )
                self._fdm_nozzle_diameter.setValue(
                    settings_dict.get("nozzle_diameter", 0.4)
                )
                self._fdm_wall_count.setValue(settings_dict.get("wall_count", 3))
                self._fdm_wall_line_width.setValue(
                    settings_dict.get("wall_line_width", 0.4)
                )
                self._fdm_top_layers.setValue(settings_dict.get("top_layers", 4))
                self._fdm_bottom_layers.setValue(settings_dict.get("bottom_layers", 4))
                self._fdm_infill_percentage.setValue(
                    settings_dict.get("infill_percentage", 20.0)
                )
                pat = settings_dict.get("infill_pattern", "grid")
                pi2 = self._fdm_infill_pattern.findText(pat)
                if pi2 >= 0:
                    self._fdm_infill_pattern.setCurrentIndex(pi2)
                self._fdm_print_speed.setValue(settings_dict.get("print_speed", 60.0))
                self._fdm_travel_speed.setValue(
                    settings_dict.get("travel_speed", 150.0)
                )
                self._fdm_first_layer_speed.setValue(
                    settings_dict.get("first_layer_speed", 20.0)
                )
                self._fdm_support_enabled.setChecked(
                    settings_dict.get("support_enabled", False)
                )
                self._fdm_support_angle.setValue(
                    settings_dict.get("support_angle", 45.0)
                )
                self._fdm_support_density.setValue(
                    settings_dict.get("support_density", 15.0)
                )
                self._fdm_tree_support.setChecked(
                    settings_dict.get("support_tree", False)
                )
                self._fdm_temperature.setValue(settings_dict.get("temperature", 210.0))
                self._fdm_bed_temperature.setValue(
                    settings_dict.get("bed_temperature", 60.0)
                )
                self._fdm_retraction_enabled.setChecked(
                    settings_dict.get("retraction_enabled", True)
                )
                self._fdm_retraction_distance.setValue(
                    settings_dict.get("retraction_distance", 5.0)
                )
                self._fdm_retraction_speed.setValue(
                    settings_dict.get("retraction_speed", 45.0)
                )
                self._fdm_fan_speed.setValue(settings_dict.get("fan_speed", 100.0))
            else:
                self._resin_layer_height.setValue(
                    settings_dict.get("layer_height", 0.05)
                )
                self._resin_bottom_layer_count.setValue(
                    settings_dict.get("bottom_layer_count", 5)
                )
                self._resin_bottom_layer_height.setValue(
                    settings_dict.get("bottom_layer_height", 0.05)
                )
                self._resin_exposure_time.setValue(
                    settings_dict.get("exposure_time", 2.5)
                )
                self._resin_bottom_exposure_time.setValue(
                    settings_dict.get("bottom_exposure_time", 30.0)
                )
                self._resin_light_off_delay.setValue(
                    settings_dict.get("light_off_delay", 1.0)
                )
                self._resin_lift_height.setValue(settings_dict.get("lift_height", 5.0))
                self._resin_lift_speed.setValue(settings_dict.get("lift_speed", 1.0))
                self._resin_retract_speed.setValue(
                    settings_dict.get("retract_speed", 1.0)
                )
                self._resin_anti_aliasing.setChecked(
                    settings_dict.get("anti_aliasing", True)
                )
                self._resin_anti_aliasing_level.setValue(
                    settings_dict.get("anti_aliasing_level", 4)
                )
                self._resin_support_enabled.setChecked(
                    settings_dict.get("support_enabled", True)
                )
                self._resin_support_point_density.setValue(
                    settings_dict.get("support_point_density", 0.7)
                )
                self._resin_support_tip_diameter.setValue(
                    settings_dict.get("support_tip_diameter", 0.4)
                )
                self._resin_support_base_diameter.setValue(
                    settings_dict.get("support_base_diameter", 1.0)
                )
        finally:
            self._suppress_signals = False

    def update_estimate(self, slice_result: Any) -> None:
        """Update the estimated time and material labels from a slice result."""
        if slice_result is None:
            return

        if isinstance(slice_result, dict):
            t = slice_result.get("print_time_estimate", 0.0) or slice_result.get(
                "estimated_time", 0.0
            )
            m = (
                slice_result.get("material_estimate", 0.0)
                or slice_result.get("material_weight", 0.0)
                or slice_result.get("estimated_resin_volume", 0.0)
            )
        else:
            t = getattr(slice_result, "print_time_estimate", 0.0) or getattr(
                slice_result, "estimated_time", 0.0
            )
            m = (
                getattr(slice_result, "material_estimate", 0.0)
                or getattr(slice_result, "material_weight", 0.0)
                or getattr(slice_result, "estimated_resin_volume", 0.0)
            )

        self._estimate_time_label.setText(self._format_time(t))
        if self._current_mode == _MODE_RESIN:
            self._estimate_material_label.setText(f"{m:.2f} mL")
        else:
            self._estimate_material_label.setText(f"{m:.2f} g")

    def show_progress(self, percent: float) -> None:
        """Show the progress bar at *percent* (0-100)."""
        self._progress_bar.setVisible(True)
        self._progress_label.setVisible(True)
        self._progress_bar.setValue(int(max(0, min(100, percent))))
        self._progress_label.setText(f"Slicing... {int(percent)}%")

    def hide_progress(self) -> None:
        """Hide the progress bar."""
        self._progress_bar.setVisible(False)
        self._progress_label.setVisible(False)

    def on_slice_complete(self, result: Any) -> None:
        """Called when slicing finishes -- re-enable buttons and update display."""
        self._slice_btn.setEnabled(True)
        self.hide_progress()

        success = True
        if isinstance(result, dict):
            success = result.get("success", True)
        elif hasattr(result, "success"):
            success = result.success

        if success:
            self._preview_btn.setEnabled(True)
            self._export_btn.setEnabled(True)
            self.update_estimate(result)
        else:
            self._preview_btn.setEnabled(False)
            self._export_btn.setEnabled(False)
            errors = []
            if isinstance(result, dict):
                errors = result.get("errors", [])
            elif hasattr(result, "errors"):
                errors = result.errors
            msg = (
                "Slicing failed:\n" + "\n".join(errors) if errors else "Slicing failed."
            )
            QtWidgets.QMessageBox.critical(self, "Slicing Error", msg)

    # ------------------------------------------------------------------
    # Utilities
    # ------------------------------------------------------------------

    @staticmethod
    def _format_time(seconds: float) -> str:
        """Format seconds into a human-readable string."""
        if seconds <= 0:
            return "--"
        h = int(seconds // 3600)
        m = int((seconds % 3600) // 60)
        s = int(seconds % 60)
        parts: List[str] = []
        if h > 0:
            parts.append(f"{h}h")
        if m > 0:
            parts.append(f"{m}m")
        parts.append(f"{s}s")
        return " ".join(parts)

    def get_selected_printer(self) -> Optional[Profile]:
        """Return the currently selected printer profile, or None."""
        idx = self._printer_combo.currentIndex()
        if 0 <= idx < len(self._fdm_printers):
            return self._fdm_printers[idx]
        return None

    def get_selected_material(self) -> Optional[Profile]:
        """Return the currently selected material profile, or None."""
        idx = self._material_combo.currentIndex()
        if 0 <= idx < len(self._fdm_materials):
            return self._fdm_materials[idx]
        return None

    def set_slice_enabled(self, enabled: bool) -> None:
        """Enable or disable the Slice button."""
        self._slice_btn.setEnabled(enabled)


# ---------------------------------------------------------------------------
# FreeCAD TaskPanel integration helper
# ---------------------------------------------------------------------------


def show_slicer_task_panel() -> SlicerTaskPanel:
    """Create and display the SlicerTaskPanel in FreeCAD's task area.

    Returns the panel widget so callers can connect to its signals.
    """
    panel = SlicerTaskPanel()
    if Gui is not None:
        try:
            Gui.TaskPanel.addTaskWidget(panel)
        except Exception:
            try:
                Gui.Control.addTaskWidget(panel)
            except Exception:
                pass
    return panel


def close_slicer_task_panel() -> None:
    """Remove the SlicerTaskPanel from FreeCAD's task area."""
    if Gui is not None:
        try:
            Gui.TaskPanel.removeTaskWidget("SlicerTaskPanel")
        except Exception:
            try:
                Gui.Control.closeTask()
            except Exception:
                pass
