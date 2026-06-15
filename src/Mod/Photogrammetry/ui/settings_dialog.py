# SPDX-License-Identifier: LGPL-2.1-or-later
"""
Settings dialog for the photogrammetry pipeline.

Provides a dialog with configurable parameters for feature extraction,
feature matching, dense reconstruction, mesh reconstruction, and B-REP
conversion stages.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

from typing import Any, Dict, List, Optional

from PySide6 import QtCore, QtWidgets


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


def _make_combobox(
    items: List[str],
    current: int = 0,
    tooltip: str = "",
) -> QtWidgets.QComboBox:
    """Create a QComboBox with standard settings."""
    combo = QtWidgets.QComboBox()
    combo.addItems(items)
    combo.setCurrentIndex(current)
    if tooltip:
        combo.setToolTip(tooltip)
    return combo


# ---------------------------------------------------------------------------
# Settings dialog
# ---------------------------------------------------------------------------

class SettingsDialog(QtWidgets.QDialog):
    """Settings dialog for photogrammetry pipeline configuration.

    Organises settings into logical sections using a QTabWidget and
    provides ``get_settings()`` / ``load_settings()`` for serialisation.
    """

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self.setWindowTitle("Photogrammetry Settings")
        self.setMinimumSize(500, 450)

        self._build_ui()
        self._load_defaults()

    def _build_ui(self) -> None:
        root = QtWidgets.QVBoxLayout(self)
        root.setContentsMargins(8, 8, 8, 8)
        root.setSpacing(6)

        self._tabs = QtWidgets.QTabWidget()

        self._tabs.addTab(self._build_feature_extraction_tab(), "Feature Extraction")
        self._tabs.addTab(self._build_feature_matching_tab(), "Feature Matching")
        self._tabs.addTab(self._build_dense_reconstruction_tab(), "Dense Reconstruction")
        self._tabs.addTab(self._build_mesh_reconstruction_tab(), "Mesh Reconstruction")
        self._tabs.addTab(self._build_brep_conversion_tab(), "B-REP Conversion")

        root.addWidget(self._tabs, 1)

        self._button_box = QtWidgets.QDialogButtonBox(
            QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
        )
        root.addWidget(self._button_box)

        self._button_box.accepted.connect(self.accept)
        self._button_box.rejected.connect(self.reject)

    # ------------------------------------------------------------------
    # Feature Extraction
    # ------------------------------------------------------------------

    def _build_feature_extraction_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._fe_method = _make_combobox(
            ["SIFT", "ORB"],
            tooltip="Feature detection algorithm",
        )
        self._fe_max_features = _make_int_spinbox(
            1000, 50000, 10000, 1000,
            tooltip="Maximum number of features to extract per image",
        )

        layout.addRow("Method:", self._fe_method)
        layout.addRow("Max features:", self._fe_max_features)
        return w

    # ------------------------------------------------------------------
    # Feature Matching
    # ------------------------------------------------------------------

    def _build_feature_matching_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._fm_method = _make_combobox(
            ["FLANN", "BFMatcher"],
            tooltip="Feature matching algorithm",
        )
        self._fm_ratio_threshold = _make_spinbox(
            0.5, 0.9, 0.75, 0.01,
            tooltip="Lowe's ratio test threshold for matching quality",
        )

        layout.addRow("Method:", self._fm_method)
        layout.addRow("Ratio threshold:", self._fm_ratio_threshold)
        return w

    # ------------------------------------------------------------------
    # Dense Reconstruction
    # ------------------------------------------------------------------

    def _build_dense_reconstruction_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._dr_method = _make_combobox(
            ["SGBM", "BM"],
            tooltip="Stereo matching algorithm",
        )
        self._dr_min_disparity = _make_int_spinbox(
            0, 128, 0, 1,
            tooltip="Minimum disparity value",
        )
        self._dr_num_disparities = _make_int_spinbox(
            16, 256, 128, 16,
            tooltip="Number of disparity levels (must be multiple of 16)",
        )
        self._dr_block_size = _make_int_spinbox(
            3, 21, 9, 2,
            tooltip="Block size for matching (odd number, 3-21)",
        )

        layout.addRow("Method:", self._dr_method)
        layout.addRow("Min disparity:", self._dr_min_disparity)
        layout.addRow("Num disparities:", self._dr_num_disparities)
        layout.addRow("Block size:", self._dr_block_size)
        return w

    # ------------------------------------------------------------------
    # Mesh Reconstruction
    # ------------------------------------------------------------------

    def _build_mesh_reconstruction_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._mr_method = _make_combobox(
            ["Ball Pivot", "Poisson"],
            tooltip="Surface reconstruction algorithm",
        )
        self._mr_ball_radius_factor = _make_spinbox(
            0.5, 3.0, 1.0, 0.1,
            tooltip=(
                "Ball radius factor relative to average point spacing. "
                "Auto-calculated from point cloud density."
            ),
        )
        self._mr_smoothing_iterations = _make_int_spinbox(
            0, 20, 3, 1,
            tooltip="Number of Laplacian smoothing iterations (0 = none)",
        )

        layout.addRow("Method:", self._mr_method)
        layout.addRow("Ball radius factor:", self._mr_ball_radius_factor)
        layout.addRow("Smoothing iterations:", self._mr_smoothing_iterations)
        return w

    # ------------------------------------------------------------------
    # B-REP Conversion
    # ------------------------------------------------------------------

    def _build_brep_conversion_tab(self) -> QtWidgets.QWidget:
        w = QtWidgets.QWidget()
        layout = QtWidgets.QFormLayout(w)
        layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._bc_min_region_area = _make_spinbox(
            0.1, 10.0, 1.0, 0.1,
            tooltip="Minimum region area as percentage of total surface",
        )
        self._bc_max_fit_error = _make_spinbox(
            0.001, 1.0, 0.01, 0.001,
            tooltip="Maximum allowed fit error (RMS) for surface fitting",
        )

        layout.addRow("Min region area (%):", self._bc_min_region_area)
        layout.addRow("Max fit error:", self._bc_max_fit_error)
        return w

    # ------------------------------------------------------------------
    # Defaults / Load / Save
    # ------------------------------------------------------------------

    def _load_defaults(self) -> None:
        """Set default values."""
        self.load_settings({})

    def get_settings(self) -> Dict[str, Any]:
        """Collect all current UI values into a flat settings dictionary."""
        return {
            "feature_extraction": {
                "method": self._fe_method.currentText(),
                "max_features": self._fe_max_features.value(),
            },
            "feature_matching": {
                "method": self._fm_method.currentText(),
                "ratio_threshold": self._fm_ratio_threshold.value(),
            },
            "dense_reconstruction": {
                "method": self._dr_method.currentText(),
                "min_disparity": self._dr_min_disparity.value(),
                "num_disparities": self._dr_num_disparities.value(),
                "block_size": self._dr_block_size.value(),
            },
            "mesh_reconstruction": {
                "method": self._mr_method.currentText(),
                "ball_radius_factor": self._mr_ball_radius_factor.value(),
                "smoothing_iterations": self._mr_smoothing_iterations.value(),
            },
            "brep_conversion": {
                "min_region_area": self._bc_min_region_area.value(),
                "max_fit_error": self._bc_max_fit_error.value(),
            },
        }

    def load_settings(self, settings: Dict[str, Any]) -> None:
        """Populate the UI from a settings dictionary.

        Parameters
        ----------
        settings:
            Nested dictionary matching the structure returned by
            ``get_settings()``.  Missing keys use built-in defaults.
        """
        fe = settings.get("feature_extraction", {})
        method_idx = self._fe_method.findText(fe.get("method", "SIFT"))
        if method_idx >= 0:
            self._fe_method.setCurrentIndex(method_idx)
        self._fe_max_features.setValue(fe.get("max_features", 10000))

        fm = settings.get("feature_matching", {})
        method_idx = self._fm_method.findText(fm.get("method", "FLANN"))
        if method_idx >= 0:
            self._fm_method.setCurrentIndex(method_idx)
        self._fm_ratio_threshold.setValue(fm.get("ratio_threshold", 0.75))

        dr = settings.get("dense_reconstruction", {})
        method_idx = self._dr_method.findText(dr.get("method", "SGBM"))
        if method_idx >= 0:
            self._dr_method.setCurrentIndex(method_idx)
        self._dr_min_disparity.setValue(dr.get("min_disparity", 0))
        self._dr_num_disparities.setValue(dr.get("num_disparities", 128))
        self._dr_block_size.setValue(dr.get("block_size", 9))

        mr = settings.get("mesh_reconstruction", {})
        method_idx = self._mr_method.findText(mr.get("method", "Ball Pivot"))
        if method_idx >= 0:
            self._mr_method.setCurrentIndex(method_idx)
        self._mr_ball_radius_factor.setValue(mr.get("ball_radius_factor", 1.0))
        self._mr_smoothing_iterations.setValue(mr.get("smoothing_iterations", 3))

        bc = settings.get("brep_conversion", {})
        self._bc_min_region_area.setValue(bc.get("min_region_area", 1.0))
        self._bc_max_fit_error.setValue(bc.get("max_fit_error", 0.01))
