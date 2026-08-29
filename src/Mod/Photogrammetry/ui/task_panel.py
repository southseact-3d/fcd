# SPDX-License-Identifier: LGPL-2.1-or-later
"""
Main task panel widget for the Photogrammetry workbench.

Provides the right-sidebar UI for configuring and controlling the
photogrammetry reconstruction pipeline inside Tungsten CAD.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

from typing import Any, Dict, List, Optional

from PySide6 import QtCore, QtGui, QtWidgets


# ---------------------------------------------------------------------------
# Constants
# ---------------------------------------------------------------------------

_PIPELINE_STAGES: List[str] = [
    "Import",
    "Feature Extraction",
    "Feature Matching",
    "Sparse Reconstruction (SfM)",
    "Dense Reconstruction",
    "Mesh Generation",
    "Texture Mapping",
]

_PENDING_STYLE = "color: #888; font-size: 11px;"
_RUNNING_STYLE = "color: #3498db; font-weight: bold; font-size: 11px;"
_DONE_STYLE = "color: #2ecc71; font-weight: bold; font-size: 11px;"
_ERROR_STYLE = "color: #e74c3c; font-weight: bold; font-size: 11px;"

_STATUS_ICONS: Dict[str, str] = {
    "pending": "○",
    "running": "◉",
    "done": "✓",
    "error": "✗",
}

_STATUS_STYLES: Dict[str, str] = {
    "pending": _PENDING_STYLE,
    "running": _RUNNING_STYLE,
    "done": _DONE_STYLE,
    "error": _ERROR_STYLE,
}


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


# ---------------------------------------------------------------------------
# Task panel
# ---------------------------------------------------------------------------

class PhotogrammetryTaskPanel(QtWidgets.QWidget):
    """Primary task panel widget displayed in the FreeCAD / Tungsten task area.

    Shows pipeline stage indicators, progress, control buttons, and results
    summary for the photogrammetry reconstruction workflow.
    """

    start_requested = QtCore.Signal()
    pause_requested = QtCore.Signal()
    cancel_requested = QtCore.Signal()
    brep_requested = QtCore.Signal()

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)
        self.setObjectName("PhotogrammetryTaskPanel")
        self.setMinimumWidth(350)

        self._stage_labels: List[QtWidgets.QLabel] = []
        self._stage_statuses: List[str] = ["pending"] * len(_PIPELINE_STAGES)

        self._build_ui()
        self._connect_signals()

    # ------------------------------------------------------------------
    # UI construction
    # ------------------------------------------------------------------

    def _build_ui(self) -> None:
        root = QtWidgets.QVBoxLayout(self)
        root.setContentsMargins(4, 4, 4, 4)
        root.setSpacing(4)

        header = _make_label("Photogrammetry Pipeline", bold=True)
        header.setStyleSheet("font-size: 13px;")
        header.setAlignment(QtCore.Qt.AlignCenter)
        root.addWidget(header)

        stages_group = QtWidgets.QGroupBox("Pipeline Stages")
        stages_layout = QtWidgets.QVBoxLayout()
        for idx, name in enumerate(_PIPELINE_STAGES):
            row = QtWidgets.QHBoxLayout()
            icon_lbl = QtWidgets.QLabel(_STATUS_ICONS["pending"])
            icon_lbl.setFixedWidth(20)
            icon_lbl.setAlignment(QtCore.Qt.AlignCenter)
            icon_lbl.setStyleSheet(_PENDING_STYLE)

            name_lbl = _make_label(f"{idx + 1}. {name}")
            name_lbl.setStyleSheet(_PENDING_STYLE)

            row.addWidget(icon_lbl)
            row.addWidget(name_lbl, 1)
            stages_layout.addLayout(row)
            self._stage_labels.append(icon_lbl)
        stages_group.setLayout(stages_layout)
        root.addWidget(stages_group)

        progress_group = QtWidgets.QGroupBox("Progress")
        progress_layout = QtWidgets.QVBoxLayout()

        self._progress_bar = QtWidgets.QProgressBar()
        self._progress_bar.setRange(0, 100)
        self._progress_bar.setValue(0)
        self._progress_bar.setTextVisible(True)
        self._progress_bar.setFormat("%p%")
        progress_layout.addWidget(self._progress_bar)

        self._status_label = _make_label("Ready")
        self._status_label.setStyleSheet("color: grey; font-size: 10px;")
        progress_layout.addWidget(self._status_label)

        progress_group.setLayout(progress_layout)
        root.addWidget(progress_group)

        controls_group = QtWidgets.QGroupBox("Controls")
        controls_layout = QtWidgets.QHBoxLayout()

        self._start_btn = _make_push_button("Start", tooltip="Begin the pipeline")
        self._start_btn.setStyleSheet(
            "QPushButton { font-weight: bold; background-color: #4a90d9; color: white; }"
            "QPushButton:hover { background-color: #5aa0e9; }"
        )

        self._pause_btn = _make_push_button(
            "Pause", tooltip="Pause the running pipeline", checkable=True
        )
        self._pause_btn.setEnabled(False)

        self._cancel_btn = _make_push_button("Cancel", tooltip="Cancel the pipeline")
        self._cancel_btn.setEnabled(False)

        self._brep_btn = _make_push_button(
            "Mesh to B-REP",
            tooltip="Convert generated mesh to B-REP solid",
        )
        self._brep_btn.setEnabled(False)

        controls_layout.addWidget(self._start_btn)
        controls_layout.addWidget(self._pause_btn)
        controls_layout.addWidget(self._cancel_btn)
        controls_layout.addWidget(self._brep_btn)
        controls_group.setLayout(controls_layout)
        root.addWidget(controls_group)

        results_group = QtWidgets.QGroupBox("Results Summary")
        results_layout = QtWidgets.QFormLayout()
        results_layout.setLabelAlignment(QtCore.Qt.AlignRight)

        self._points_label = _make_label("--")
        self._mesh_label = _make_label("--")
        self._time_label = _make_label("--")

        results_layout.addRow("Points:", self._points_label)
        results_layout.addRow("Mesh:", self._mesh_label)
        results_layout.addRow("Time:", self._time_label)

        results_group.setLayout(results_layout)
        root.addWidget(results_group)

        root.addStretch(1)

    def _connect_signals(self) -> None:
        self._start_btn.clicked.connect(self._on_start)
        self._pause_btn.clicked.connect(self._on_pause)
        self._cancel_btn.clicked.connect(self._on_cancel)
        self._brep_btn.clicked.connect(self._on_brep)

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------

    def _on_start(self) -> None:
        self._start_btn.setEnabled(False)
        self._pause_btn.setEnabled(True)
        self._pause_btn.setChecked(False)
        self._cancel_btn.setEnabled(True)
        self.start_requested.emit()

    def _on_pause(self) -> None:
        if self._pause_btn.isChecked():
            self._pause_btn.setText("Resume")
            self._status_label.setText("Paused")
        else:
            self._pause_btn.setText("Pause")
            self._status_label.setText("Running...")
        self.pause_requested.emit()

    def _on_cancel(self) -> None:
        self._start_btn.setEnabled(True)
        self._pause_btn.setEnabled(False)
        self._cancel_btn.setEnabled(False)
        self._pause_btn.setText("Pause")
        self._pause_btn.setChecked(False)
        self.cancel_requested.emit()

    def _on_brep(self) -> None:
        self.brep_requested.emit()

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    def set_stage(self, stage_index: int, status: str) -> None:
        """Update a stage indicator.

        Parameters
        ----------
        stage_index:
            Zero-based index of the pipeline stage.
        status:
            One of ``"pending"``, ``"running"``, ``"done"``, ``"error"``.
        """
        if stage_index < 0 or stage_index >= len(_PIPELINE_STAGES):
            return

        self._stage_statuses[stage_index] = status
        icon = _STATUS_ICONS.get(status, "○")
        style = _STATUS_STYLES.get(status, _PENDING_STYLE)

        self._stage_labels[stage_index].setText(icon)
        self._stage_labels[stage_index].setStyleSheet(style)

        # Also update the name label style (sibling in the same row)
        parent_layout = self._stage_labels[stage_index].parent().layout()
        if parent_layout is not None:
            name_lbl_item = parent_layout.itemAt(1)
            if name_lbl_item is not None and name_lbl_item.widget() is not None:
                name_lbl_item.widget().setStyleSheet(style)

    def set_progress(self, percent: float, message: str = "") -> None:
        """Update the progress bar and status message.

        Parameters
        ----------
        percent:
            Progress value between 0 and 100.
        message:
            Optional status text shown below the bar.
        """
        clamped = max(0.0, min(100.0, percent))
        self._progress_bar.setValue(int(clamped))
        if message:
            self._status_label.setText(message)

    def set_results(self, results: Dict[str, Any]) -> None:
        """Update the results summary labels.

        Parameters
        ----------
        results:
            Dictionary with optional keys ``"points"``, ``"vertices"``,
            ``"faces"``, ``"time"``.
        """
        points = results.get("points")
        if points is not None:
            self._points_label.setText(f"{int(points):,}")

        verts = results.get("vertices")
        faces = results.get("faces")
        if verts is not None or faces is not None:
            parts = []
            if verts is not None:
                parts.append(f"{int(verts):,} vertices")
            if faces is not None:
                parts.append(f"{int(faces):,} faces")
            self._mesh_label.setText(", ".join(parts))

        elapsed = results.get("time")
        if elapsed is not None:
            self._time_label.setText(self._format_time(float(elapsed)))

    def enable_brep_button(self, enabled: bool) -> None:
        """Enable or disable the Mesh-to-B-REP button."""
        self._brep_btn.setEnabled(enabled)

    def reset_stages(self) -> None:
        """Reset all stages to pending."""
        for idx in range(len(_PIPELINE_STAGES)):
            self.set_stage(idx, "pending")
        self._progress_bar.setValue(0)
        self._status_label.setText("Ready")
        self._points_label.setText("--")
        self._mesh_label.setText("--")
        self._time_label.setText("--")
        self._start_btn.setEnabled(True)
        self._pause_btn.setEnabled(False)
        self._pause_btn.setText("Pause")
        self._pause_btn.setChecked(False)
        self._cancel_btn.setEnabled(False)
        self._brep_btn.setEnabled(False)

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
