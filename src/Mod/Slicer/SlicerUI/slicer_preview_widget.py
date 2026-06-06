#
# SPDX-License-Identifier: LGPL-2.1-or-later
# SlicerWorkbench - 2D Layer Preview Widget
#

from __future__ import annotations

import math
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Optional

from PySide6 import QtCore, QtGui, QtWidgets


class FeatureType(Enum):
    OuterWall = auto()
    InnerWall = auto()
    Infill = auto()
    Support = auto()
    Travel = auto()
    Skirt = auto()
    SupportInterface = auto()
    Unknown = auto()


class ColorMode(Enum):
    Feature = auto()
    Speed = auto()
    Filament = auto()
    Layer = auto()


FEAT_COLORS = {
    FeatureType.OuterWall: QtGui.QColor(220, 50, 50),
    FeatureType.InnerWall: QtGui.QColor(230, 150, 30),
    FeatureType.Infill: QtGui.QColor(50, 120, 220),
    FeatureType.Support: QtGui.QColor(40, 180, 60),
    FeatureType.Travel: QtGui.QColor(160, 160, 160),
    FeatureType.Skirt: QtGui.QColor(180, 120, 40),
    FeatureType.SupportInterface: QtGui.QColor(80, 200, 100),
    FeatureType.Unknown: QtGui.QColor(128, 128, 128),
}


@dataclass
class Segment2D:
    x0: float
    y0: float
    x1: float
    y1: float
    feature: FeatureType = FeatureType.Unknown
    speed: float = 0.0
    filament: float = 0.0

    @property
    def length(self) -> float:
        return math.hypot(self.x1 - self.x0, self.y1 - self.y0)


@dataclass
class LayerData:
    index: int
    z: float
    height: float = 0.2
    segments: list[Segment2D] = field(default_factory=list)

    @property
    def bounds(self) -> tuple[float, float, float, float]:
        if not self.segments:
            return (0.0, 0.0, 0.0, 0.0)
        xs: list[float] = []
        ys: list[float] = []
        for seg in self.segments:
            xs.extend([seg.x0, seg.x1])
            ys.extend([seg.y0, seg.y1])
        return (min(xs), min(ys), max(xs), max(ys))


class SlicerPreviewWidget(QtWidgets.QWidget):
    """2D layer preview rendering sliced cross-sections.

    Signals:
        layer_changed(int) - emitted when user scrubs to a different layer
    """

    layer_changed = QtCore.Signal(int)

    _GRID_SIZE = 220.0
    _GRID_STEP = 10.0

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)

        self._layers: list[LayerData] = []
        self._current_layer: int = 0
        self._color_mode: ColorMode = ColorMode.Feature
        self._highlight_polygon: Optional[list[tuple[float, float]]] = None

        self._zoom: float = 1.0
        self._pan_x: float = 0.0
        self._pan_y: float = 0.0
        self._panning: bool = False
        self._last_mouse: QtCore.QPoint = QtCore.QPoint()

        self._speed_min: float = 0.0
        self._speed_max: float = 300.0

        self.setMinimumSize(320, 320)
        self.setMouseTracking(True)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)

        self._slider = QtWidgets.QSlider(QtCore.Qt.Vertical, self)
        self._slider.setRange(0, 0)
        self._slider.setTickPosition(QtWidgets.QSlider.Right)
        self._slider.setTickInterval(1)
        self._slider.valueChanged.connect(self._on_slider_changed)
        self._slider.setFixedWidth(36)

        self._info_label = QtWidgets.QLabel(self)
        self._info_label.setStyleSheet(
            "QLabel{background:rgba(0,0,0,160);color:#fff;"
            "padding:4px 8px;border-radius:4px;font:monospace 11px;}"
        )
        self._info_label.setAlignment(QtCore.Qt.AlignBottom | QtCore.Qt.AlignLeft)
        self._info_label.hide()

        self._rebuild_layout()

    def _rebuild_layout(self) -> None:
        lay = QtWidgets.QHBoxLayout(self)
        lay.setContentsMargins(0, 0, 0, 0)
        lay.addWidget(self._slider, 0, QtCore.Qt.AlignRight)
        self.setLayout(lay)

    # -- Public API -----------------------------------------------------------

    def set_layer_data(self, layers: list[LayerData]) -> None:
        self._layers = layers
        self._slider.blockSignals(True)
        self._slider.setRange(0, max(len(layers) - 1, 0))
        self._slider.setValue(min(self._current_layer, len(layers) - 1))
        self._slider.blockSignals(False)
        self._current_layer = self._slider.value()
        self._update_speed_range()
        self.update()

    def set_current_layer(self, index: int) -> None:
        index = max(0, min(index, len(self._layers) - 1))
        self._current_layer = index
        self._slider.blockSignals(True)
        self._slider.setValue(index)
        self._slider.blockSignals(False)
        self.update()

    def set_color_mode(self, mode: ColorMode) -> None:
        self._color_mode = mode
        self.update()

    def highlight_region(self, polygon: list[tuple[float, float]]) -> None:
        self._highlight_polygon = polygon if polygon else None
        self.update()

    def clear_highlight(self) -> None:
        self._highlight_polygon = None
        self.update()

    def current_layer_data(self) -> Optional[LayerData]:
        if 0 <= self._current_layer < len(self._layers):
            return self._layers[self._current_layer]
        return None

    @QtCore.Slot(int)
    def _on_slider_changed(self, value: int) -> None:
        self._current_layer = value
        self.update()
        self.layer_changed.emit(value)

    # -- Helpers --------------------------------------------------------------

    def _update_speed_range(self) -> None:
        speeds: list[float] = []
        for layer in self._layers:
            for seg in layer.segments:
                if seg.speed > 0:
                    speeds.append(seg.speed)
        if speeds:
            self._speed_min = min(speeds)
            self._speed_max = max(speeds)
        else:
            self._speed_min = 0.0
            self._speed_max = 300.0

    def _color_for_segment(self, seg: Segment2D, layer_index: int) -> QtGui.QColor:
        if self._color_mode == ColorMode.Feature:
            return FEAT_COLORS.get(seg.feature, FEAT_COLORS[FeatureType.Unknown])

        if self._color_mode == ColorMode.Speed:
            t = 0.0
            if self._speed_max > self._speed_min:
                t = (seg.speed - self._speed_min) / (self._speed_max - self._speed_min)
            t = max(0.0, min(1.0, t))
            return QtGui.QColor.fromHsvF(0.66 * (1.0 - t), 0.85, 0.95)

        if self._color_mode == ColorMode.Filament:
            if seg.filament <= 0:
                return QtGui.QColor(128, 128, 128)
            t = min(seg.filament / 5.0, 1.0)
            return QtGui.QColor.fromHsvF(0.3 * (1.0 - t), 0.7, 0.9)

        if self._color_mode == ColorMode.Layer:
            total = max(len(self._layers) - 1, 1)
            t = layer_index / total
            return QtGui.QColor.fromHsvF(t * 0.85, 0.75, 0.95)

        return QtGui.QColor(200, 200, 200)

    def _line_width(self, feat: FeatureType) -> float:
        if feat == FeatureType.OuterWall:
            return 2.5
        if feat == FeatureType.InnerWall:
            return 2.0
        if feat in (FeatureType.Support, FeatureType.SupportInterface):
            return 1.4
        if feat == FeatureType.Travel:
            return 0.8
        return 1.2

    def _world_to_screen(self, wx: float, wy: float) -> tuple[int, int]:
        half = self._GRID_SIZE / 2.0
        cx = self.width() / 2.0 + self._pan_x
        cy = self.height() / 2.0 + self._pan_y
        sx = cx + ((wx - half) * self._zoom)
        sy = cy + ((wy - half) * self._zoom)
        return int(sx), int(sy)

    def _screen_to_world(self, sx: int, sy: int) -> tuple[float, float]:
        half = self._GRID_SIZE / 2.0
        cx = self.width() / 2.0 + self._pan_x
        cy = self.height() / 2.0 + self._pan_y
        wx = (sx - cx) / self._zoom + half
        wy = (sy - cy) / self._zoom + half
        return wx, wy

    def _update_info_label(self, layer: Optional[LayerData]) -> None:
        if layer is None:
            self._info_label.hide()
            return
        n_seg = len(layer.segments)
        total_len = sum(s.length for s in layer.segments)
        feat_counts: dict[FeatureType, int] = {}
        for s in layer.segments:
            feat_counts[s.feature] = feat_counts.get(s.feature, 0) + 1
        feat_str = ", ".join(
            f"{f.name}:{c}"
            for f, c in sorted(feat_counts.items(), key=lambda x: x[0].value)
        )
        text = (
            f"Layer {layer.index}  |  Z={layer.z:.2f} mm  |  "
            f"{n_seg} segs  |  {total_len:.1f} mm"
        )
        if feat_str:
            text += f"\n{feat_str}"
        self._info_label.setText(text)
        self._info_label.adjustSize()
        self._info_label.show()

    # -- Events ---------------------------------------------------------------

    def resizeEvent(self, event: QtGui.QResizeEvent) -> None:
        super().resizeEvent(event)
        self._info_label.setGeometry(8, self.height() - 60, 400, 56)

    def paintEvent(self, event: QtGui.QPaintEvent) -> None:
        painter = QtGui.QPainter(self)
        painter.setRenderHint(QtGui.QPainter.Antialiasing, True)
        painter.fillRect(self.rect(), QtGui.QColor(24, 24, 30))

        self._draw_grid(painter)
        self._draw_layer(painter)
        self._draw_highlight(painter)

        layer = self.current_layer_data()
        self._update_info_label(layer)
        painter.end()

    def wheelEvent(self, event: QtGui.QWheelEvent) -> None:
        delta = event.angleDelta().y()
        factor = 1.15 if delta > 0 else 1.0 / 1.15
        self._zoom = max(0.2, min(self._zoom * factor, 40.0))
        self.update()

    def mousePressEvent(self, event: QtGui.QMouseEvent) -> None:
        if event.button() == QtCore.Qt.MiddleButton:
            self._panning = True
            if hasattr(event, "position"):
                self._last_mouse = event.position().toPoint()
            else:
                self._last_mouse = event.pos()
            event.accept()
        else:
            super().mousePressEvent(event)

    def mouseMoveEvent(self, event: QtGui.QMouseEvent) -> None:
        if self._panning:
            if hasattr(event, "position"):
                pos = event.position().toPoint()
            else:
                pos = event.pos()
            dx = pos.x() - self._last_mouse.x()
            dy = pos.y() - self._last_mouse.y()
            self._pan_x += dx
            self._pan_y += dy
            self._last_mouse = pos
            self.update()
            event.accept()
        else:
            super().mouseMoveEvent(event)

    def mouseReleaseEvent(self, event: QtGui.QMouseEvent) -> None:
        if event.button() == QtCore.Qt.MiddleButton:
            self._panning = False
            event.accept()
        else:
            super().mouseReleaseEvent(event)

    def keyPressEvent(self, event: QtGui.QKeyEvent) -> None:
        key = event.key()
        if key in (QtCore.Qt.Key_Plus, QtCore.Qt.Key_Equal):
            self._zoom = min(self._zoom * 1.2, 40.0)
            self.update()
        elif key == QtCore.Qt.Key_Minus:
            self._zoom = max(self._zoom / 1.2, 0.2)
            self.update()
        elif key == QtCore.Qt.Key_Home:
            self._pan_x = 0.0
            self._pan_y = 0.0
            self._zoom = 1.0
            self.update()
        else:
            super().keyPressEvent(event)

    # -- Drawing --------------------------------------------------------------

    def _draw_grid(self, painter: QtGui.QPainter) -> None:
        grid = self._GRID_SIZE
        step = self._GRID_STEP

        pen = QtGui.QPen(QtGui.QColor(60, 60, 70))
        pen.setWidthF(0.5)
        painter.setPen(pen)

        i = 0.0
        while i <= grid:
            x0, y0 = self._world_to_screen(i, 0)
            x1, y1 = self._world_to_screen(i, grid)
            painter.drawLine(x0, y0, x1, y1)
            x0, y0 = self._world_to_screen(0, i)
            x1, y1 = self._world_to_screen(grid, i)
            painter.drawLine(x0, y0, x1, y1)
            i += step

        pen2 = QtGui.QPen(QtGui.QColor(100, 100, 120))
        pen2.setWidthF(1.5)
        painter.setPen(pen2)
        tl = self._world_to_screen(0, 0)
        br = self._world_to_screen(grid, grid)
        painter.drawRect(tl[0], tl[1], br[0] - tl[0], br[1] - tl[1])

    def _draw_layer(self, painter: QtGui.QPainter) -> None:
        layer = self.current_layer_data()
        if layer is None:
            return

        travels: list[Segment2D] = []
        feature_segs: list[Segment2D] = []
        for seg in layer.segments:
            if seg.feature == FeatureType.Travel:
                travels.append(seg)
            else:
                feature_segs.append(seg)

        if travels:
            pen = QtGui.QPen(QtGui.QColor(160, 160, 160))
            pen.setWidthF(0.8)
            pen.setStyle(QtCore.Qt.DashLine)
            pen.setDashPattern([4, 4])
            painter.setPen(pen)
            painter.setBrush(QtCore.Qt.NoBrush)
            for seg in travels:
                x0, y0 = self._world_to_screen(seg.x0, seg.y0)
                x1, y1 = self._world_to_screen(seg.x1, seg.y1)
                painter.drawLine(x0, y0, x1, y1)

        for seg in feature_segs:
            color = self._color_for_segment(seg, layer.index)
            pen = QtGui.QPen(color)
            pen.setWidthF(self._line_width(seg.feature))
            pen.setCapStyle(QtCore.Qt.RoundCap)
            painter.setPen(pen)
            x0, y0 = self._world_to_screen(seg.x0, seg.y0)
            x1, y1 = self._world_to_screen(seg.x1, seg.y1)
            painter.drawLine(x0, y0, x1, y1)

    def _draw_highlight(self, painter: QtGui.QPainter) -> None:
        if not self._highlight_polygon or len(self._highlight_polygon) < 3:
            return

        points = []
        for wx, wy in self._highlight_polygon:
            sx, sy = self._world_to_screen(wx, wy)
            points.append(QtCore.QPoint(sx, sy))

        pen = QtGui.QPen(QtGui.QColor(255, 255, 0))
        pen.setWidthF(2.0)
        painter.setPen(pen)

        fill = QtGui.QColor(255, 255, 0, 40)
        painter.setBrush(fill)
        painter.drawPolygon(QtGui.QPolygon(points))
