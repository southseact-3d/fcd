# SPDX-License-Identifier: LGPL-2.1-or-later

"""Manual support painting widget for interactive support placement."""

from __future__ import annotations

from PySide6 import QtCore, QtGui, QtWidgets

try:
    import FreeCADGui

    _HAS_FREECAD = True
except ImportError:
    _HAS_FREECAD = False


# ---------------------------------------------------------------------------
# Support point data container
# ---------------------------------------------------------------------------


class _SupportPoint:
    """Represents a single support point on the model surface."""

    __slots__ = ("x", "y", "z", "radius", "is_placed")

    def __init__(self, x: float, y: float, z: float, radius: float = 0.5) -> None:
        self.x = x
        self.y = y
        self.z = z
        self.radius = radius
        self.is_placed = True


# ---------------------------------------------------------------------------
# Widget
# ---------------------------------------------------------------------------


class SupportPaintWidget(QtWidgets.QWidget):
    """Widget for manually painting support points.

    Signals
    -------
    support_points_changed(list)
        Emitted whenever the list of support points changes.  Payload is a
        list of ``_SupportPoint`` instances.
    clear_all_requested()
        Emitted when the user clicks *Clear All Supports*.
    """

    support_points_changed = QtCore.Signal(list)
    clear_all_requested = QtCore.Signal()

    # Tool modes
    TOOL_ADD = 0
    TOOL_REMOVE = 1
    TOOL_AUTOFILL = 2

    def __init__(self, parent: QtWidgets.QWidget | None = None) -> None:
        super().__init__(parent)

        self._points: list[_SupportPoint] = []
        self._undo_stack: list[list[_SupportPoint]] = []
        self._active_tool: int = self.TOOL_ADD
        self._brush_size: float = 1.0
        self._point_density: float = 0.5

        self._ui_setup()

    # ------------------------------------------------------------------
    # Public API
    # ------------------------------------------------------------------

    @property
    def points(self) -> list[_SupportPoint]:
        return list(self._points)

    def add_support_point(self, x: float, y: float, z: float) -> None:
        """Add a support point at the given world coordinate."""
        self._push_undo()
        pt = _SupportPoint(x, y, z, self._brush_size)
        self._points.append(pt)
        self._update_status()
        self.support_points_changed.emit(list(self._points))

    def remove_nearest(
        self, x: float, y: float, z: float, threshold: float = 2.0
    ) -> bool:
        """Remove the closest support point within *threshold* units.

        Returns *True* if a point was removed.
        """
        if not self._points:
            return False

        best_idx = -1
        best_dist = float("inf")
        for i, pt in enumerate(self._points):
            d = ((pt.x - x) ** 2 + (pt.y - y) ** 2 + (pt.z - z) ** 2) ** 0.5
            if d < best_dist:
                best_dist = d
                best_idx = i

        if best_idx >= 0 and best_dist <= threshold:
            self._push_undo()
            self._points.pop(best_idx)
            self._update_status()
            self.support_points_changed.emit(list(self._points))
            return True
        return False

    def clear_all(self) -> None:
        """Remove every support point."""
        if not self._points:
            return
        self._push_undo()
        self._points.clear()
        self._update_status()
        self.support_points_changed.emit(list(self._points))
        self.clear_all_requested.emit()

    def undo(self) -> None:
        """Revert the last action."""
        if not self._undo_stack:
            return
        self._points = self._undo_stack.pop()
        self._update_status()
        self.support_points_changed.emit(list(self._points))

    # ------------------------------------------------------------------
    # UI construction
    # ------------------------------------------------------------------

    def _ui_setup(self) -> None:
        root = QtWidgets.QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)

        # --- Tool selector -----------------------------------------------
        tool_group = QtWidgets.QGroupBox("Support Tool")
        tool_lay = QtWidgets.QVBoxLayout(tool_group)

        self._radio_add = QtWidgets.QRadioButton("Add Support Point")
        self._radio_add.setToolTip("Click on model surface to place a support point")
        self._radio_add.setChecked(True)
        self._radio_add.toggled.connect(self._on_tool_changed)
        tool_lay.addWidget(self._radio_add)

        self._radio_remove = QtWidgets.QRadioButton("Remove Support Point")
        self._radio_remove.setToolTip("Click near a support point to remove it")
        self._radio_remove.toggled.connect(self._on_tool_changed)
        tool_lay.addWidget(self._radio_remove)

        self._radio_autofill = QtWidgets.QRadioButton("Auto-fill Islands")
        self._radio_autofill.setToolTip(
            "Automatically place supports on overhang islands"
        )
        self._radio_autofill.toggled.connect(self._on_tool_changed)
        tool_lay.addWidget(self._radio_autofill)

        root.addWidget(tool_group)

        # --- Brush settings ----------------------------------------------
        brush_group = QtWidgets.QGroupBox("Brush Settings")
        brush_lay = QtWidgets.QFormLayout(brush_group)

        self._slider_brush = QtWidgets.QSlider(QtCore.Qt.Orientation.Horizontal)
        self._slider_brush.setRange(10, 500)
        self._slider_brush.setValue(100)
        self._slider_brush.setTickPosition(QtWidgets.QSlider.TickPosition.TicksBelow)
        self._slider_brush.setTickInterval(50)
        self._slider_brush.setToolTip("Support point radius / brush size")
        self._slider_brush.valueChanged.connect(self._on_brush_changed)
        brush_lay.addRow("Brush Size:", self._slider_brush)

        self._lbl_brush_val = QtWidgets.QLabel("1.00 mm")
        self._lbl_brush_val.setAlignment(QtCore.Qt.AlignmentFlag.AlignRight)
        brush_lay.addRow("", self._lbl_brush_val)

        self._slider_density = QtWidgets.QSlider(QtCore.Qt.Orientation.Horizontal)
        self._slider_density.setRange(10, 200)
        self._slider_density.setValue(50)
        self._slider_density.setTickPosition(QtWidgets.QSlider.TickPosition.TicksBelow)
        self._slider_density.setTickInterval(25)
        self._slider_density.setToolTip(
            "Point density for auto-fill (higher = more supports)"
        )
        self._slider_density.valueChanged.connect(self._on_density_changed)
        brush_lay.addRow("Auto-fill Density:", self._slider_density)

        self._lbl_density_val = QtWidgets.QLabel("0.50")
        self._lbl_density_val.setAlignment(QtCore.Qt.AlignmentFlag.AlignRight)
        brush_lay.addRow("", self._lbl_density_val)

        root.addWidget(brush_group)

        # --- Action buttons ----------------------------------------------
        actions_group = QtWidgets.QGroupBox("Actions")
        actions_lay = QtWidgets.QHBoxLayout(actions_group)

        self._btn_undo = QtWidgets.QPushButton("Undo")
        self._btn_undo.setToolTip("Revert the last support action")
        self._btn_undo.setIcon(
            self.style().standardIcon(QtWidgets.QStyle.StandardPixmap.SP_ArrowBack)
        )
        self._btn_undo.clicked.connect(self.undo)
        actions_lay.addWidget(self._btn_undo)

        self._btn_clear = QtWidgets.QPushButton("Clear All")
        self._btn_clear.setToolTip("Remove all support points")
        self._btn_clear.setIcon(
            self.style().standardIcon(
                QtWidgets.QStyle.StandardPixmap.SP_DialogDiscardButton
            )
        )
        self._btn_clear.clicked.connect(self.clear_all)
        actions_lay.addWidget(self._btn_clear)

        root.addWidget(actions_group)

        # --- Status label -----------------------------------------------
        self._lbl_status = QtWidgets.QLabel("0 supports placed, 0 removed")
        self._lbl_status.setAlignment(QtCore.Qt.AlignmentFlag.AlignCenter)
        self._lbl_status.setStyleSheet(
            "padding: 4px; background: palette(base); border: 1px solid palette(mid);"
        )
        root.addWidget(self._lbl_status)

        root.addStretch(1)

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------

    def _on_tool_changed(self) -> None:
        if self._radio_add.isChecked():
            self._active_tool = self.TOOL_ADD
        elif self._radio_remove.isChecked():
            self._active_tool = self.TOOL_REMOVE
        elif self._radio_autofill.isChecked():
            self._active_tool = self.TOOL_AUTOFILL

    def _on_brush_changed(self, value: int) -> None:
        self._brush_size = value / 100.0
        self._lbl_brush_val.setText(f"{self._brush_size:.2f} mm")

    def _on_density_changed(self, value: int) -> None:
        self._point_density = value / 100.0
        self._lbl_density_val.setText(f"{self._point_density:.2f}")

    # ------------------------------------------------------------------
    # Undo
    # ------------------------------------------------------------------

    def _push_undo(self) -> None:
        self._undo_stack.append(list(self._points))
        if len(self._undo_stack) > 50:
            self._undo_stack.pop(0)

    # ------------------------------------------------------------------
    # Status
    # ------------------------------------------------------------------

    def _update_status(self) -> None:
        placed = len(self._points)
        removed = max(0, len(self._undo_stack) - (1 if self._undo_stack else 0))
        self._lbl_status.setText(f"{placed} supports placed")
