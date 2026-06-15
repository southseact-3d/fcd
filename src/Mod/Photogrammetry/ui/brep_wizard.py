# SPDX-License-Identifier: LGPL-2.1-or-later
"""
Multi-step wizard dialog for converting a mesh to B-REP.

Guides the user through mesh simplification, region detection, surface
fitting, and solid creation with a QStackedWidget-based workflow.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

from typing import Any, Dict, List, Optional

from PySide6 import QtCore, QtGui, QtWidgets


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def _make_label(text: str, tooltip: str = "", bold: bool = False) -> QtWidgets.QLabel:
    """Create a styled QLabel."""
    lbl = QtWidgets.QLabel(text)
    if tooltip:
        lbl.setToolTip(tooltip)
    if bold:
        font = lbl.font()
        font.setBold(True)
        lbl.setFont(font)
    return lbl


def _make_push_button(
    text: str,
    tooltip: str = "",
    checkable: bool = False,
) -> QtWidgets.QPushButton:
    """Create a QPushButton with standard styling."""
    btn = QtWidgets.QPushButton(text)
    if tooltip:
        btn.setToolTip(tooltip)
    btn.setMinimumHeight(28)
    btn.setCheckable(checkable)
    return btn


def _make_spinbox(
    min_val: float,
    max_val: float,
    value: float,
    step: float = 1.0,
    decimals: int = 2,
    tooltip: str = "",
    suffix: str = "",
) -> QtWidgets.QDoubleSpinBox:
    """Create a QDoubleSpinBox with standard settings."""
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
    """Create a QSpinBox with standard settings."""
    sb = QtWidgets.QSpinBox()
    sb.setRange(min_val, max_val)
    sb.setSingleStep(step)
    sb.setValue(value)
    if tooltip:
        sb.setToolTip(tooltip)
    return sb


# ---------------------------------------------------------------------------
# Step indicator widget
# ---------------------------------------------------------------------------

class _StepIndicator(QtWidgets.QWidget):
    """Horizontal row of numbered circles indicating the current wizard step."""

    def __init__(
        self,
        labels: List[str],
        parent: Optional[QtWidgets.QWidget] = None,
    ) -> None:
        super().__init__(parent)
        self._labels = labels
        self._circles: List[QtWidgets.QLabel] = []
        self._active = 0
        self._build_ui()

    def _build_ui(self) -> None:
        layout = QtWidgets.QHBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)

        for idx, label in enumerate(self._labels):
            if idx > 0:
                line = QtWidgets.QFrame()
                line.setFrameShape(QtWidgets.QFrame.HLine)
                line.setStyleSheet("color: #555;")
                line.setFixedHeight(2)
                layout.addWidget(line, 1)

            col = QtWidgets.QVBoxLayout()
            col.setAlignment(QtCore.Qt.AlignCenter)

            circle = QtWidgets.QLabel(str(idx + 1))
            circle.setFixedSize(28, 28)
            circle.setAlignment(QtCore.Qt.AlignCenter)
            circle.setStyleSheet(
                "border-radius: 14px; border: 2px solid #555; color: #888; font-size: 11px;"
            )
            self._circles.append(circle)
            col.addWidget(circle, 0, QtCore.Qt.AlignCenter)

            lbl = _make_label(label)
            lbl.setStyleSheet("font-size: 9px; color: #888;")
            lbl.setAlignment(QtCore.Qt.AlignCenter)
            col.addWidget(lbl, 0, QtCore.Qt.AlignCenter)

            layout.addLayout(col)

        self.set_step(0)

    def set_step(self, index: int) -> None:
        """Highlight the given step index and mark earlier ones complete."""
        self._active = index
        for idx, circle in enumerate(self._circles):
            if idx < index:
                circle.setText("✓")
                circle.setStyleSheet(
                    "border-radius: 14px; border: 2px solid #2ecc71;"
                    "color: white; background-color: #2ecc71; font-size: 11px;"
                )
            elif idx == index:
                circle.setText(str(idx + 1))
                circle.setStyleSheet(
                    "border-radius: 14px; border: 2px solid #3498db;"
                    "color: white; background-color: #3498db; font-size: 11px;"
                )
            else:
                circle.setText(str(idx + 1))
                circle.setStyleSheet(
                    "border-radius: 14px; border: 2px solid #555;"
                    "color: #888; font-size: 11px;"
                )


# ---------------------------------------------------------------------------
# Step 1: Mesh Selection
# ---------------------------------------------------------------------------

class _MeshSelectionPage(QtWidgets.QWidget):
    """First wizard page -- mesh info and simplification controls."""

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self._build_ui()

    def _build_ui(self) -> None:
        layout = QtWidgets.QVBoxLayout(self)
        layout.setContentsMargins(16, 16, 16, 16)
        layout.setSpacing(12)

        layout.addWidget(_make_label("Mesh Selection", bold=True))

        info_group = QtWidgets.QGroupBox("Current Mesh")
        info_form = QtWidgets.QFormLayout()
        info_form.setLabelAlignment(QtCore.Qt.AlignRight)

        self._vertices_label = _make_label("--")
        self._faces_label = _make_label("--")
        self._bounds_label = _make_label("--")

        info_form.addRow("Vertices:", self._vertices_label)
        info_form.addRow("Faces:", self._faces_label)
        info_form.addRow("Bounds:", self._bounds_label)
        info_group.setLayout(info_form)
        layout.addWidget(info_group)

        simplify_group = QtWidgets.QGroupBox("Simplification")
        simplify_form = QtWidgets.QFormLayout()
        simplify_form.setLabelAlignment(QtCore.Qt.AlignRight)

        self._target_faces = _make_int_spinbox(
            100, 10000000, 100000, 10000,
            tooltip="Target face count after simplification",
        )
        self._simplify_ratio = _make_spinbox(
            0.01, 1.0, 0.5, 0.05,
            tooltip="Ratio of original faces to keep",
        )

        simplify_form.addRow("Target faces:", self._target_faces)
        simplify_form.addRow("Keep ratio:", self._simplify_ratio)
        simplify_group.setLayout(simplify_form)
        layout.addWidget(simplify_group)

        preview_group = QtWidgets.QGroupBox("Preview")
        preview_layout = QtWidgets.QVBoxLayout()

        self._original_info = _make_label("Original: --")
        self._simplified_info = _make_label("Simplified: --")

        preview_layout.addWidget(self._original_info)
        preview_layout.addWidget(self._simplified_info)
        preview_group.setLayout(preview_layout)
        layout.addWidget(preview_group)

        layout.addStretch(1)

    def set_mesh_info(
        self, vertices: int, faces: int, bounds: str
    ) -> None:
        """Populate mesh info labels."""
        self._vertices_label.setText(f"{vertices:,}")
        self._faces_label.setText(f"{faces:,}")
        self._bounds_label.setText(bounds)
        self._original_info.setText(f"Original: {faces:,} faces")
        self._target_faces.setMaximum(faces)
        self._simplify_ratio.setValue(1.0)
        self._on_ratio_changed()
        self._target_faces.valueChanged.connect(self._on_target_changed)
        self._simplify_ratio.valueChanged.connect(self._on_ratio_changed)

    def _on_target_changed(self) -> None:
        total = int(self._vertices_label.text().replace(",", "") or "0")
        if total > 0:
            self._simplify_ratio.setValue(self._target_faces.value() / total)

    def _on_ratio_changed(self) -> None:
        total = int(self._faces_label.text().replace(",", "") or "0")
        target = int(total * self._simplify_ratio.value())
        self._target_faces.setValue(target)
        self._simplified_info.setText(f"Simplified: {target:,} faces")

    def get_target_faces(self) -> int:
        """Return the target face count."""
        return self._target_faces.value()


# ---------------------------------------------------------------------------
# Step 2: Region Detection
# ---------------------------------------------------------------------------

class _RegionDetectionPage(QtWidgets.QWidget):
    """Second wizard page -- detect geometric regions on the mesh."""

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self._regions: List[Dict[str, Any]] = []
        self._build_ui()

    def _build_ui(self) -> None:
        layout = QtWidgets.QVBoxLayout(self)
        layout.setContentsMargins(16, 16, 16, 16)
        layout.setSpacing(12)

        layout.addWidget(_make_label("Region Detection", bold=True))

        layout.addWidget(
            _make_label("Detect planar, cylindrical, spherical, and B-spline regions.")
        )

        btn_row = QtWidgets.QHBoxLayout()
        self._detect_btn = _make_push_button(
            "Auto-detect", tooltip="Run automatic region detection"
        )
        self._reset_btn = _make_push_button(
            "Reset", tooltip="Clear detected regions"
        )
        btn_row.addWidget(self._detect_btn)
        btn_row.addWidget(self._reset_btn)
        btn_row.addStretch()
        layout.addLayout(btn_row)

        self._region_table = QtWidgets.QTableWidget()
        self._region_table.setColumnCount(4)
        self._region_table.setHorizontalHeaderLabels(
            ["Type", "Area %", "Fit Error (RMS)", "Select"]
        )
        self._region_table.horizontalHeader().setStretchLastSection(True)
        self._region_table.verticalHeader().setVisible(False)
        self._region_table.setSelectionBehavior(
            QtWidgets.QAbstractItemView.SelectRows
        )
        layout.addWidget(self._region_table, 1)

        merge_row = QtWidgets.QHBoxLayout()
        self._merge_btn = _make_push_button(
            "Merge Selected", tooltip="Merge selected regions into one"
        )
        self._split_btn = _make_push_button(
            "Split Region", tooltip="Split the selected region"
        )
        merge_row.addWidget(self._merge_btn)
        merge_row.addWidget(self._split_btn)
        merge_row.addStretch()
        layout.addLayout(merge_row)

    def set_regions(self, regions: List[Dict[str, Any]]) -> None:
        """Populate the region table.

        Parameters
        ----------
        regions:
            List of dicts with keys ``"type"``, ``"area_pct"``,
            ``"fit_error"``.
        """
        self._regions = regions
        self._region_table.setRowCount(len(regions))
        for row, reg in enumerate(regions):
            type_item = QtWidgets.QTableWidgetItem(reg.get("type", "Unknown"))
            area_item = QtWidgets.QTableWidgetItem(f"{reg.get('area_pct', 0.0):.1f}")
            error_item = QtWidgets.QTableWidgetItem(f"{reg.get('fit_error', 0.0):.4f}")

            cb = QtWidgets.QCheckBox()
            cb.setChecked(True)

            self._region_table.setItem(row, 0, type_item)
            self._region_table.setItem(row, 1, area_item)
            self._region_table.setItem(row, 2, error_item)
            self._region_table.setCellWidget(row, 3, cb)

        self._region_table.resizeColumnsToContents()

    def get_selected_regions(self) -> List[int]:
        """Return indices of selected regions."""
        selected = []
        for row in range(self._region_table.rowCount()):
            cb = self._region_table.cellWidget(row, 3)
            if isinstance(cb, QtWidgets.QCheckBox) and cb.isChecked():
                selected.append(row)
        return selected


# ---------------------------------------------------------------------------
# Step 3: Surface Fitting
# ---------------------------------------------------------------------------

class _SurfaceFittingPage(QtWidgets.QWidget):
    """Third wizard page -- fit primitives to detected regions."""

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self._regions: List[Dict[str, Any]] = []
        self._build_ui()

    def _build_ui(self) -> None:
        layout = QtWidgets.QVBoxLayout(self)
        layout.setContentsMargins(16, 16, 16, 16)
        layout.setSpacing(12)

        layout.addWidget(_make_label("Surface Fitting", bold=True))

        layout.addWidget(
            _make_label("Override primitive type per region and review fit quality.")
        )

        self._fit_table = QtWidgets.QTableWidget()
        self._fit_table.setColumnCount(4)
        self._fit_table.setHorizontalHeaderLabels(
            ["Region", "Type", "Parameters", "Fit Quality"]
        )
        self._fit_table.horizontalHeader().setStretchLastSection(True)
        self._fit_table.verticalHeader().setVisible(False)
        self._fit_table.setSelectionBehavior(QtWidgets.QAbstractItemView.SelectRows)
        layout.addWidget(self._fit_table, 1)

    def set_regions(self, regions: List[Dict[str, Any]]) -> None:
        """Populate fitting data.

        Parameters
        ----------
        regions:
            List of dicts with ``"type"``, ``"params"`` (dict), and
            ``"fit_error"`` (float).
        """
        self._regions = regions
        self._fit_table.setRowCount(len(regions))

        primitive_types = ["Plane", "Cylinder", "Sphere", "B-Spline"]

        for row, reg in enumerate(regions):
            name_item = QtWidgets.QTableWidgetItem(f"Region {row + 1}")

            type_combo = QtWidgets.QComboBox()
            type_combo.addItems(primitive_types)
            idx = type_combo.findText(reg.get("type", "Plane"))
            if idx >= 0:
                type_combo.setCurrentIndex(idx)

            params = reg.get("params", {})
            params_text = self._format_params(reg.get("type", "Plane"), params)
            params_item = QtWidgets.QTableWidgetItem(params_text)

            fit_error = reg.get("fit_error", 0.0)
            if fit_error < 0.005:
                quality = "Good"
                qcolor = "#2ecc71"
            elif fit_error < 0.02:
                quality = "Fair"
                qcolor = "#f39c12"
            else:
                quality = "Poor"
                qcolor = "#e74c3c"
            quality_item = QtWidgets.QTableWidgetItem(quality)
            quality_item.setForeground(QtGui.QColor(qcolor))

            self._fit_table.setItem(row, 0, name_item)
            self._fit_table.setCellWidget(row, 1, type_combo)
            self._fit_table.setItem(row, 2, params_item)
            self._fit_table.setItem(row, 3, quality_item)

        self._fit_table.resizeColumnsToContents()

    @staticmethod
    def _format_params(primitive_type: str, params: Dict[str, Any]) -> str:
        """Format fitted parameters into a readable string."""
        if primitive_type == "Plane":
            n = params.get("normal", [0, 0, 1])
            d = params.get("distance", 0.0)
            return f"n=[{n[0]:.3f}, {n[1]:.3f}, {n[2]:.3f}], d={d:.3f}"
        if primitive_type == "Cylinder":
            pt = params.get("axis_point", [0, 0, 0])
            dr = params.get("axis_direction", [0, 0, 1])
            r = params.get("radius", 1.0)
            return (
                f"pt=[{pt[0]:.2f},{pt[1]:.2f},{pt[2]:.2f}], "
                f"dir=[{dr[0]:.2f},{dr[1]:.2f},{dr[2]:.2f}], r={r:.3f}"
            )
        if primitive_type == "Sphere":
            c = params.get("center", [0, 0, 0])
            r = params.get("radius", 1.0)
            return f"center=[{c[0]:.2f},{c[1]:.2f},{c[2]:.2f}], r={r:.3f}"
        return str(params) if params else "N/A"


# ---------------------------------------------------------------------------
# Step 4: Solid Creation
# ---------------------------------------------------------------------------

class _SolidCreationPage(QtWidgets.QWidget):
    """Fourth wizard page -- create the B-REP solid."""

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self._build_ui()

    def _build_ui(self) -> None:
        layout = QtWidgets.QVBoxLayout(self)
        layout.setContentsMargins(16, 16, 16, 16)
        layout.setSpacing(12)

        layout.addWidget(_make_label("Solid Creation", bold=True))

        preview_group = QtWidgets.QGroupBox("B-REP Preview")
        preview_layout = QtWidgets.QVBoxLayout()

        self._preview_text = QtWidgets.QTextEdit()
        self._preview_text.setReadOnly(True)
        self._preview_text.setPlaceholderText("No preview available")
        preview_layout.addWidget(self._preview_text)
        preview_group.setLayout(preview_layout)
        layout.addWidget(preview_group, 1)

        options_group = QtWidgets.QGroupBox("Options")
        options_layout = QtWidgets.QFormLayout()
        options_layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._solid_radio = QtWidgets.QRadioButton("Create as solid")
        self._solid_radio.setToolTip("Attempt to make a watertight solid")
        self._solid_radio.setChecked(True)

        self._surface_radio = QtWidgets.QRadioButton("Create as surface")
        self._surface_radio.setToolTip("Create just the fitted surfaces")

        type_row = QtWidgets.QHBoxLayout()
        type_row.addWidget(self._solid_radio)
        type_row.addWidget(self._surface_radio)
        type_row.addStretch()
        options_layout.addRow("Output:", type_row)

        options_group.setLayout(options_layout)
        layout.addWidget(options_group)

        validation_group = QtWidgets.QGroupBox("Validation")
        validation_layout = QtWidgets.QVBoxLayout()

        self._closed_label = _make_label("Closed: --")
        self._watertight_label = _make_label("Watertight: --")
        self._self_intersect_label = _make_label("Self-intersecting: --")

        validation_layout.addWidget(self._closed_label)
        validation_layout.addWidget(self._watertight_label)
        validation_layout.addWidget(self._self_intersect_label)
        validation_group.setLayout(validation_layout)
        layout.addWidget(validation_group)

    def set_preview(self, text: str) -> None:
        """Set the preview description text."""
        self._preview_text.setPlainText(text)

    def set_validation(
        self, closed: bool, watertight: bool, self_intersecting: bool
    ) -> None:
        """Update validation labels."""
        self._closed_label.setText(f"Closed: {'Yes' if closed else 'No'}")
        self._closed_label.setStyleSheet(
            "color: #2ecc71;" if closed else "color: #e74c3c;"
        )

        self._watertight_label.setText(f"Watertight: {'Yes' if watertight else 'No'}")
        self._watertight_label.setStyleSheet(
            "color: #2ecc71;" if watertight else "color: #e74c3c;"
        )

        self._self_intersect_label.setText(
            f"Self-intersecting: {'Yes' if self_intersecting else 'No'}"
        )
        self._self_intersect_label.setStyleSheet(
            "color: #e74c3c;" if self_intersecting else "color: #2ecc71;"
        )

    def is_solid_mode(self) -> bool:
        """Return True if the user selected solid creation."""
        return self._solid_radio.isChecked()


# ---------------------------------------------------------------------------
# Wizard dialog
# ---------------------------------------------------------------------------

class BRepWizard(QtWidgets.QDialog):
    """Multi-step wizard for converting a mesh to a B-REP solid.

    Uses a QStackedWidget to present four sequential steps: mesh selection,
    region detection, surface fitting, and solid creation.
    """

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self.setWindowTitle("Mesh to B-REP Conversion Wizard")
        self.setMinimumSize(800, 600)

        self._current_step = 0
        self._step_count = 4

        self._build_ui()
        self._connect_signals()

    def _build_ui(self) -> None:
        root = QtWidgets.QVBoxLayout(self)
        root.setContentsMargins(8, 8, 8, 8)
        root.setSpacing(6)

        self._step_indicator = _StepIndicator(
            ["Mesh Selection", "Region Detection", "Surface Fitting", "Solid Creation"]
        )
        root.addWidget(self._step_indicator)

        self._stack = QtWidgets.QStackedWidget()

        self._page_mesh = _MeshSelectionPage()
        self._page_regions = _RegionDetectionPage()
        self._page_fitting = _SurfaceFittingPage()
        self._page_solid = _SolidCreationPage()

        self._stack.addWidget(self._page_mesh)
        self._stack.addWidget(self._page_regions)
        self._stack.addWidget(self._page_fitting)
        self._stack.addWidget(self._page_solid)

        root.addWidget(self._stack, 1)

        nav_row = QtWidgets.QHBoxLayout()
        self._back_btn = _make_push_button("Back")
        self._back_btn.setEnabled(False)

        self._next_btn = _make_push_button("Next")

        spacer = QtWidgets.QWidget()
        spacer.setSizePolicy(
            QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Fixed
        )

        self._finish_btn = _make_push_button("Finish")
        self._finish_btn.setVisible(False)
        self._finish_btn.setStyleSheet(
            "QPushButton { font-weight: bold; background-color: #2ecc71; color: white; }"
            "QPushButton:hover { background-color: #3ddc84; }"
        )

        nav_row.addWidget(self._back_btn)
        nav_row.addWidget(spacer)
        nav_row.addWidget(self._next_btn)
        nav_row.addWidget(self._finish_btn)
        root.addLayout(nav_row)

    def _connect_signals(self) -> None:
        self._back_btn.clicked.connect(self._go_back)
        self._next_btn.clicked.connect(self._go_next)
        self._finish_btn.clicked.connect(self.accept)
        self._page_regions._detect_btn.clicked.connect(self._on_detect)
        self._page_regions._reset_btn.clicked.connect(self._on_reset_regions)

    # ------------------------------------------------------------------
    # Navigation
    # ------------------------------------------------------------------

    def _go_next(self) -> None:
        if self._current_step >= self._step_count - 1:
            return
        self._current_step += 1
        self._update_ui()

    def _go_back(self) -> None:
        if self._current_step <= 0:
            return
        self._current_step -= 1
        self._update_ui()

    def _update_ui(self) -> None:
        self._stack.setCurrentIndex(self._current_step)
        self._step_indicator.set_step(self._current_step)

        self._back_btn.setEnabled(self._current_step > 0)

        is_last = self._current_step == self._step_count - 1
        self._next_btn.setVisible(not is_last)
        self._finish_btn.setVisible(is_last)

    # ------------------------------------------------------------------
    # Actions
    # ------------------------------------------------------------------

    def _on_detect(self) -> None:
        """Placeholder for region detection -- emits a signal or uses defaults."""
        default_regions = [
            {"type": "Plane", "area_pct": 45.2, "fit_error": 0.003},
            {"type": "Cylinder", "area_pct": 28.7, "fit_error": 0.008},
            {"type": "Sphere", "area_pct": 15.1, "fit_error": 0.012},
            {"type": "B-Spline", "area_pct": 11.0, "fit_error": 0.025},
        ]
        self._page_regions.set_regions(default_regions)
        self._page_fitting.set_regions(default_regions)
        self._page_solid.set_preview(
            "Detected regions:\n"
            "  - Plane: 45.2% of surface area\n"
            "  - Cylinder: 28.7% of surface area\n"
            "  - Sphere: 15.1% of surface area\n"
            "  - B-Spline: 11.0% of surface area\n\n"
            "Ready to create B-REP solid."
        )
        self._page_solid.set_validation(True, True, False)

    def _on_reset_regions(self) -> None:
        self._page_regions.set_regions([])
        self._page_fitting.set_regions([])

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def set_mesh_info(self, vertices: int, faces: int, bounds: str) -> None:
        """Provide mesh information to the first page."""
        self._page_mesh.set_mesh_info(vertices, faces, bounds)

    def set_regions(self, regions: List[Dict[str, Any]]) -> None:
        """Provide pre-detected regions to pages 2 and 3."""
        self._page_regions.set_regions(regions)
        self._page_fitting.set_regions(regions)

    def set_preview(self, text: str) -> None:
        """Set the B-REP preview description."""
        self._page_solid.set_preview(text)

    def set_validation(
        self, closed: bool, watertight: bool, self_intersecting: bool
    ) -> None:
        """Set validation results on the last page."""
        self._page_solid.set_validation(closed, watertight, self_intersecting)

    def get_result(self) -> Dict[str, Any]:
        """Collect and return the conversion parameters from all steps."""
        selected = self._page_regions.get_selected_regions()

        fitted_types: List[str] = []
        for row in range(self._page_fitting._fit_table.rowCount()):
            combo = self._page_fitting._fit_table.cellWidget(row, 1)
            if isinstance(combo, QtWidgets.QComboBox):
                fitted_types.append(combo.currentText())
            else:
                fitted_types.append("Plane")

        return {
            "target_faces": self._page_mesh.get_target_faces(),
            "selected_regions": selected,
            "fitted_types": fitted_types,
            "create_solid": self._page_solid.is_solid_mode(),
        }
