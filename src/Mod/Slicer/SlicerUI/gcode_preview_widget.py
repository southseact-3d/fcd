#
# SPDX-License-Identifier: LGPL-2.1-or-later
# SlicerWorkbench - G-code Preview Widget
#

from __future__ import annotations

import math
import re
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import Optional

from PySide6 import QtCore, QtGui, QtWidgets


class GcodeColorMode(Enum):
    Feature = auto()
    Speed = auto()
    Layer = auto()


class GcodeFeature(Enum):
    Perimeter = auto()
    InnerPerimeter = auto()
    Infill = auto()
    Support = auto()
    SupportInterface = auto()
    Travel = auto()
    Retraction = auto()
    Skirt = auto()
    Tower = auto()
    Unknown = auto()


GCODFEAT_COLORS = {
    GcodeFeature.Perimeter: QtGui.QColor(220, 50, 50),
    GcodeFeature.InnerPerimeter: QtGui.QColor(230, 150, 30),
    GcodeFeature.Infill: QtGui.QColor(50, 120, 220),
    GcodeFeature.Support: QtGui.QColor(40, 180, 60),
    GcodeFeature.SupportInterface: QtGui.QColor(80, 200, 100),
    GcodeFeature.Travel: QtGui.QColor(160, 160, 160),
    GcodeFeature.Retraction: QtGui.QColor(100, 100, 100),
    GcodeFeature.Skirt: QtGui.QColor(180, 120, 40),
    GcodeFeature.Tower: QtGui.QColor(180, 80, 180),
    GcodeFeature.Unknown: QtGui.QColor(128, 128, 128),
}

FEAT_LINE_WIDTHS = {
    GcodeFeature.Perimeter: 2.5,
    GcodeFeature.InnerPerimeter: 2.0,
    GcodeFeature.Infill: 1.2,
    GcodeFeature.Support: 1.4,
    GcodeFeature.SupportInterface: 1.4,
    GcodeFeature.Travel: 0.8,
    GcodeFeature.Retraction: 0.5,
    GcodeFeature.Skirt: 1.5,
    GcodeFeature.Tower: 1.6,
    GcodeFeature.Unknown: 1.0,
}

_COMMENT_FEATURE_MAP: dict[str, GcodeFeature] = {
    "outer perimeter": GcodeFeature.Perimeter,
    "external perimeter": GcodeFeature.Perimeter,
    "perimeter": GcodeFeature.Perimeter,
    "inner perimeter": GcodeFeature.InnerPerimeter,
    "wall-outer": GcodeFeature.Perimeter,
    "wall-inner": GcodeFeature.InnerPerimeter,
    "wall": GcodeFeature.InnerPerimeter,
    "infill": GcodeFeature.Infill,
    "solid infill": GcodeFeature.Infill,
    "internal infill": GcodeFeature.Infill,
    "top solid infill": GcodeFeature.Infill,
    "bottom solid infill": GcodeFeature.Infill,
    "fill": GcodeFeature.Infill,
    "support material": GcodeFeature.Support,
    "support": GcodeFeature.Support,
    "support-m": GcodeFeature.Support,
    "support material interface": GcodeFeature.SupportInterface,
    "support interface": GcodeFeature.SupportInterface,
    "support-m-interface": GcodeFeature.SupportInterface,
    "skirt": GcodeFeature.Skirt,
    "skirt-brim": GcodeFeature.Skirt,
    "brim": GcodeFeature.Skirt,
    "prime tower": GcodeFeature.Tower,
    "wipe tower": GcodeFeature.Tower,
}


_LAYER_Z_RE = re.compile(r";\s*LAYER:(\d+)", re.IGNORECASE)
_SPEED_RE = re.compile(r"G[01]\s+.*F([\d.]+)", re.IGNORECASE)
_X_RE = re.compile(r"X([\d.e+-]+)", re.IGNORECASE)
_Y_RE = re.compile(r"Y([\d.e+-]+)", re.IGNORECASE)
_Z_RE = re.compile(r"Z([\d.e+-]+)", re.IGNORECASE)
_E_RE = re.compile(r"E([-]?[\d.e+-]+)", re.IGNORECASE)


@dataclass
class GcodeSegment:
    x0: float = 0.0
    y0: float = 0.0
    z0: float = 0.0
    x1: float = 0.0
    y1: float = 0.0
    z1: float = 0.0
    e: float = 0.0
    speed: float = 0.0
    feature: GcodeFeature = GcodeFeature.Unknown
    layer_index: int = 0
    is_retraction: bool = False

    @property
    def length_2d(self) -> float:
        return math.hypot(self.x1 - self.x0, self.y1 - self.y0)

    @property
    def is_travel(self) -> bool:
        return self.feature == GcodeFeature.Travel or (
            self.e <= 0 and not self.is_retraction
        )


@dataclass
class GcodeLayerInfo:
    index: int
    z: float
    segments: list[GcodeSegment] = field(default_factory=list)
    move_count: int = 0
    total_time: float = 0.0
    total_filament: float = 0.0
    total_distance: float = 0.0


@dataclass
class GcodeParseResult:
    layers: list[GcodeLayerInfo] = field(default_factory=list)
    min_x: float = 0.0
    min_y: float = 0.0
    max_x: float = 300.0
    max_y: float = 300.0
    speed_min: float = 0.0
    speed_max: float = 300.0


class GcodeParser:
    """Parses G-code text into structured layers and segments."""

    def parse(self, gcode_text: str) -> GcodeParseResult:
        result = GcodeParseResult()
        lines = gcode_text.splitlines()

        current_layer = GcodeLayerInfo(index=0, z=0.0)
        result.layers.append(current_layer)

        cur_x = 0.0
        cur_y = 0.0
        cur_z = 0.0
        cur_e = 0.0
        cur_speed = 0.0
        cur_feature = GcodeFeature.Unknown
        speeds: list[float] = []
        xs: list[float] = []
        ys: list[float] = []

        for line in lines:
            stripped = line.strip()

            if stripped.startswith(";"):
                lower = stripped.lower()
                if "layer" in lower:
                    m = _LAYER_Z_RE.search(stripped)
                    if m:
                        idx = int(m.group(1))
                        current_layer = GcodeLayerInfo(index=idx, z=cur_z)
                        result.layers.append(current_layer)
                        continue
                for pattern, feat in _COMMENT_FEATURE_MAP.items():
                    if pattern in lower:
                        cur_feature = feat
                        break
                continue

            if not stripped:
                continue

            parts = stripped.split(";")[0].strip()
            if not parts:
                continue

            if "M82" in parts or "M83" in parts:
                continue

            sm = _SPEED_RE.search(parts)
            if sm:
                cur_speed = float(sm.group(1)) / 60.0
                if cur_speed > 0:
                    speeds.append(cur_speed)

            if not parts.upper().startswith("G1") and not parts.upper().startswith("G0"):
                continue

            nx, ny, nz, ne = None, None, None, None
            xm = _X_RE.search(parts)
            ym = _Y_RE.search(parts)
            zm = _Z_RE.search(parts)
            em = _E_RE.search(parts)

            if xm:
                nx = float(xm.group(1))
            if ym:
                ny = float(ym.group(1))
            if zm:
                nz = float(zm.group(1))
            if em:
                ne = float(em.group(1))

            is_retract = ne is not None and ne < 0
            is_travel = (ne is None or ne <= 0) and not is_retract

            if is_retract:
                cur_feature = GcodeFeature.Retraction
            elif is_travel and cur_feature not in (
                GcodeFeature.Support,
                GcodeFeature.SupportInterface,
            ):
                cur_feature = GcodeFeature.Travel

            seg = GcodeSegment(
                x0=cur_x,
                y0=cur_y,
                z0=cur_z,
                x1=nx if nx is not None else cur_x,
                y1=ny if ny is not None else cur_y,
                z1=nz if nz is not None else cur_z,
                e=ne if ne is not None else 0.0,
                speed=cur_speed,
                feature=cur_feature,
                layer_index=current_layer.index,
                is_retraction=is_retract,
            )

            if nz is not None and nz != cur_z:
                current_layer.z = nz
                seg.z1 = nz

            current_layer.segments.append(seg)
            current_layer.move_count += 1

            if seg.e > 0:
                current_layer.total_filament += seg.e
            dist = seg.length_2d
            current_layer.total_distance += dist

            if cur_speed > 0:
                current_layer.total_time += dist / cur_speed

            if nx is not None:
                cur_x = nx
            if ny is not None:
                cur_y = ny
            if nz is not None:
                cur_z = nz
            if ne is not None:
                cur_e += ne

            xs.append(cur_x)
            ys.append(cur_y)

        if xs:
            result.min_x = min(xs)
            result.max_x = max(xs)
        if ys:
            result.min_y = min(ys)
            result.max_y = max(ys)
        if speeds:
            result.speed_min = min(speeds)
            result.speed_max = max(speeds)

        result.layers = [lyr for lyr in result.layers if lyr.segments]
        for i, lyr in enumerate(result.layers):
            lyr.index = i

        return result


class GcodePreviewWidget(QtWidgets.QWidget):
    """2D G-code toolpath preview with layer navigation and animation.

    Signals:
        layer_changed(int) - emitted when the displayed layer changes
    """

    layer_changed = QtCore.Signal(int)

    def __init__(self, parent: Optional[QtWidgets.QWidget] = None) -> None:
        super().__init__(parent)

        self._parse_result: Optional[GcodeParseResult] = None
        self._current_layer: int = 0
        self._color_mode: GcodeColorMode = GcodeColorMode.Feature

        self._zoom: float = 1.0
        self._pan_x: float = 0.0
        self._pan_y: float = 0.0
        self._panning: bool = False
        self._last_mouse: QtCore.QPoint = QtCore.QPoint()

        self._animating: bool = False
        self._anim_timer = QtCore.QTimer(self)
        self._anim_timer.setInterval(80)
        self._anim_timer.timeout.connect(self._on_anim_tick)
        self._anim_start: int = 0
        self._anim_end: int = 0
        self._anim_step: int = 1
        self._anim_current: int = 0

        self.setMinimumSize(320, 320)
        self.setMouseTracking(True)
        self.setFocusPolicy(QtCore.Qt.StrongFocus)

        self._build_ui()

    def _build_ui(self) -> None:
        self._slider = QtWidgets.QSlider(QtCore.Qt.Vertical, self)
        self._slider.setRange(0, 0)
        self._slider.setTickPosition(QtWidgets.QSlider.Right)
        self._slider.setTickInterval(1)
        self._slider.valueChanged.connect(self._on_slider_changed)
        self._slider.setFixedWidth(36)

        self._play_btn = QtWidgets.QPushButton("\u25b6", self)
        self._play_btn.setFixedSize(30, 30)
        self._play_btn.setToolTip("Animate layer playback")
        self._play_btn.clicked.connect(self._toggle_animation)
        self._play_btn.setStyleSheet(
            "QPushButton{background:#333;color:#ccc;border:1px solid #555;"
            "border-radius:4px;font:14px;}"
            "QPushButton:hover{background:#444;}"
        )

        self._info_label = QtWidgets.QLabel(self)
        self._info_label.setStyleSheet(
            "QLabel{background:rgba(0,0,0,160);color:#fff;"
            "padding:4px 8px;border-radius:4px;font:monospace 11px;}"
        )
        self._info_label.setAlignment(QtCore.Qt.AlignBottom | QtCore.Qt.AlignLeft)
        self._info_label.hide()

        right_col = QtWidgets.QVBoxLayout()
        right_col.addWidget(self._play_btn, 0, QtCore.Qt.AlignHCenter)
        right_col.addWidget(self._slider, 1)
        right_col.addStretch()

        root = QtWidgets.QHBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        root.addStretch(1)
        root.addLayout(right_col)
        self.setLayout(root)

    # -- Public API -----------------------------------------------------------

    def set_gcode(self, gcode_text: str) -> None:
        parser = GcodeParser()
        self._parse_result = parser.parse(gcode_text)
        self._current_layer = 0
        total = max(len(self._parse_result.layers) - 1, 0)
        self._slider.blockSignals(True)
        self._slider.setRange(0, total)
        self._slider.setValue(0)
        self._slider.blockSignals(False)
        self._stop_animation()
        self.update()

    def set_layer(self, index: int) -> None:
        if self._parse_result is None:
            return
        index = max(0, min(index, len(self._parse_result.layers) - 1))
        self._current_layer = index
        self._slider.blockSignals(True)
        self._slider.setValue(index)
        self._slider.blockSignals(False)
        self.update()
        self.layer_changed.emit(index)

    def get_layer_info(self, index: int) -> Optional[dict]:
        if self._parse_result is None:
            return None
        if index < 0 or index >= len(self._parse_result.layers):
            return None
        lyr = self._parse_result.layers[index]
        info = {
            "index": lyr.index,
            "z": lyr.z,
            "moves": lyr.move_count,
            "time_seconds": lyr.total_time,
            "filament_mm": lyr.total_filament,
            "distance_mm": lyr.total_distance,
        }
        return info

    def animate_layer(self, start: int, end: int) -> None:
        if self._parse_result is None:
            return
        total = len(self._parse_result.layers) - 1
        self._anim_start = max(0, min(start, total))
        self._anim_end = max(0, min(end, total))
        if self._anim_start <= self._anim_end:
            self._anim_step = 1
        else:
            self._anim_step = -1
        self._anim_current = self._anim_start
        self._animating = True
        self._play_btn.setText("\u23f9")
        self.set_layer(self._anim_current)
        self._anim_timer.start()

    def set_color_mode(self, mode: GcodeColorMode) -> None:
        self._color_mode = mode
        self.update()

    def stop_animation(self) -> None:
        self._stop_animation()

    # -- Slots ----------------------------------------------------------------

    @QtCore.Slot(int)
    def _on_slider_changed(self, value: int) -> None:
        self._current_layer = value
        self.update()
        self.layer_changed.emit(value)

    @QtCore.Slot()
    def _on_anim_tick(self) -> None:
        self._anim_current += self._anim_step
        if self._anim_step > 0 and self._anim_current > self._anim_end:
            self._stop_animation()
            return
        if self._anim_step < 0 and self._anim_current < self._anim_end:
            self._stop_animation()
            return
        self.set_layer(self._anim_current)

    @QtCore.Slot()
    def _toggle_animation(self) -> None:
        if self._animating:
            self._stop_animation()
        else:
            n = len(self._parse_result.layers) if self._parse_result else 0
            if n > 1:
                self.animate_layer(0, n - 1)

    def _stop_animation(self) -> None:
        self._animating = False
        self._anim_timer.stop()
        self._play_btn.setText("\u25b6")

    # -- Color helpers --------------------------------------------------------

    def _color_for_segment(self, seg: GcodeSegment) -> QtGui.QColor:
        pr = self._parse_result

        if self._color_mode == GcodeColorMode.Feature:
            return GCODFEAT_COLORS.get(
                seg.feature, GCODFEAT_COLORS[GcodeFeature.Unknown]
            )

        if self._color_mode == GcodeColorMode.Speed:
            if pr is None or pr.speed_max <= pr.speed_min:
                return QtGui.QColor(128, 128, 255)
            t = (seg.speed - pr.speed_min) / (pr.speed_max - pr.speed_min)
            t = max(0.0, min(1.0, t))
            return QtGui.QColor.fromHsvF(0.66 * (1.0 - t), 0.85, 0.95)

        if self._color_mode == GcodeColorMode.Layer:
            if pr is None or len(pr.layers) <= 1:
                return QtGui.QColor(128, 200, 255)
            t = seg.layer_index / max(len(pr.layers) - 1, 1)
            return QtGui.QColor.fromHsvF(t * 0.85, 0.75, 0.95)

        return QtGui.QColor(200, 200, 200)

    def _line_width(self, feature: GcodeFeature) -> float:
        return FEAT_LINE_WIDTHS.get(feature, 1.0)

    # -- Coordinate transform -------------------------------------------------

    def _bounds(self) -> tuple[float, float, float, float]:
        if self._parse_result is None:
            return (0.0, 0.0, 300.0, 300.0)
        pad = 20.0
        return (
            self._parse_result.min_x - pad,
            self._parse_result.min_y - pad,
            self._parse_result.max_x + pad,
            self._parse_result.max_y + pad,
        )

    def _world_to_screen(self, wx: float, wy: float) -> tuple[int, int]:
        bx0, by0, bx1, by1 = self._bounds()
        bw = bx1 - bx0
        bh = by1 - by0
        if bw <= 0 or bh <= 0:
            return (self.width() // 2, self.height() // 2)
        scale_x = (self.width() - 50) / bw
        scale_y = (self.height() - 50) / bh
        s = min(scale_x, scale_y) * self._zoom
        cx = self.width() / 2.0 + self._pan_x
        cy = self.height() / 2.0 + self._pan_y
        mid_wx = (bx0 + bx1) / 2.0
        mid_wy = (by0 + by1) / 2.0
        sx = cx + (wx - mid_wx) * s
        sy = cy + (wy - mid_wy) * s
        return int(sx), int(sy)

    # -- Info panel ------------------------------------------------------------

    def _update_info_label(self) -> None:
        info = self.get_layer_info(self._current_layer)
        if info is None:
            self._info_label.hide()
            return
        minutes = info["time_seconds"] / 60.0
        layer_text = "Layer %d  |  Z=%.2f mm" % (info["index"], info["z"])
        stats_text = "Moves: %d  |  Time: %.1f min  |  Filament: %.1f mm" % (
            info["moves"],
            minutes,
            info["filament_mm"],
        )
        self._info_label.setText(layer_text + "\n" + stats_text)
        self._info_label.adjustSize()
        self._info_label.show()

    # -- Events ---------------------------------------------------------------

    def resizeEvent(self, event: QtGui.QResizeEvent) -> None:
        super().resizeEvent(event)
        self._info_label.setGeometry(8, self.height() - 60, 450, 56)

    def paintEvent(self, event: QtGui.QPaintEvent) -> None:
        painter = QtGui.QPainter(self)
        painter.setRenderHint(QtGui.QPainter.Antialiasing, True)
        painter.fillRect(self.rect(), QtGui.QColor(24, 24, 30))

        self._draw_bed(painter)
        self._draw_layer(painter)
        self._update_info_label()
        painter.end()

    def wheelEvent(self, event: QtGui.QWheelEvent) -> None:
        delta = event.angleDelta().y()
        factor = 1.15 if delta > 0 else 1.0 / 1.15
        self._zoom = max(0.2, min(self._zoom * factor, 50.0))
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
            self._zoom = min(self._zoom * 1.2, 50.0)
            self.update()
        elif key == QtCore.Qt.Key_Minus:
            self._zoom = max(self._zoom / 1.2, 0.2)
            self.update()
        elif key == QtCore.Qt.Key_Home:
            self._pan_x = 0.0
            self._pan_y = 0.0
            self._zoom = 1.0
            self.update()
        elif key == QtCore.Qt.Key_Space:
            self._toggle_animation()
        else:
            super().keyPressEvent(event)

    # -- Drawing --------------------------------------------------------------

    def _draw_bed(self, painter: QtGui.QPainter) -> None:
        bx0, by0, bx1, by1 = self._bounds()
        tl = self._world_to_screen(bx0, by0)
        br = self._world_to_screen(bx1, by1)

        pen = QtGui.QPen(QtGui.QColor(50, 50, 60))
        pen.setWidthF(0.5)
        painter.setPen(pen)
        step = 10.0
        x = bx0
        while x <= bx1:
            p0 = self._world_to_screen(x, by0)
            p1 = self._world_to_screen(x, by1)
            painter.drawLine(p0[0], p0[1], p1[0], p1[1])
            x += step
        y = by0
        while y <= by1:
            p0 = self._world_to_screen(bx0, y)
            p1 = self._world_to_screen(bx1, y)
            painter.drawLine(p0[0], p0[1], p1[0], p1[1])
            y += step

        pen2 = QtGui.QPen(QtGui.QColor(80, 80, 100))
        pen2.setWidthF(1.5)
        painter.setPen(pen2)
        painter.setBrush(QtCore.Qt.NoBrush)
        painter.drawRect(tl[0], tl[1], br[0] - tl[0], br[1] - tl[1])

    def _draw_layer(self, painter: QtGui.QPainter) -> None:
        if self._parse_result is None:
            return
        if self._current_layer < 0 or self._current_layer >= len(
            self._parse_result.layers
        ):
            return

        layer = self._parse_result.layers[self._current_layer]

        travels: list[GcodeSegment] = []
        feature_segs: list[GcodeSegment] = []
        for seg in layer.segments:
            if seg.is_travel:
                travels.append(seg)
            else:
                feature_segs.append(seg)

        if travels:
            pen = QtGui.QPen(QtGui.QColor(140, 140, 140))
            pen.setWidthF(0.8)
            pen.setStyle(QtCore.Qt.DashLine)
            pen.setDashPattern([3, 3])
            painter.setPen(pen)
            for seg in travels:
                p0 = self._world_to_screen(seg.x0, seg.y0)
                p1 = self._world_to_screen(seg.x1, seg.y1)
                painter.drawLine(p0[0], p0[1], p1[0], p1[1])

        for seg in feature_segs:
            color = self._color_for_segment(seg)
            pen = QtGui.QPen(color)
            pen.setWidthF(self._line_width(seg.feature))
            pen.setCapStyle(QtCore.Qt.RoundCap)
            painter.setPen(pen)
            p0 = self._world_to_screen(seg.x0, seg.y0)
            p1 = self._world_to_screen(seg.x1, seg.y1)
            painter.drawLine(p0[0], p0[1], p1[0], p1[1])
