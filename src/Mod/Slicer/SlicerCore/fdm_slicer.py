"""
Custom FDM Slicer Engine - Generates valid G-code from triangular meshes.

Implements slicing, perimeter generation, infill patterns, support structures,
and G-code output compatible with Marlin/RepRap firmware.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

import math
import time
from dataclasses import dataclass, field
from enum import Enum, auto
from typing import List, Tuple, Optional, Dict, Any, Sequence

Vector2 = Tuple[float, float]
Vector3 = Tuple[float, float, float]
Segment2D = Tuple[Vector2, Vector2]


# ---------------------------------------------------------------------------
# Math helpers
# ---------------------------------------------------------------------------


def _cross2d(a: Vector2, b: Vector2) -> float:
    return a[0] * b[1] - a[1] * b[0]


def _dot2d(a: Vector2, b: Vector2) -> float:
    return a[0] * b[0] + a[1] * b[1]


def _sub2d(a: Vector2, b: Vector2) -> Vector2:
    return (a[0] - b[0], a[1] - b[1])


def _add2d(a: Vector2, b: Vector2) -> Vector2:
    return (a[0] + b[0], a[1] + b[1])


def _scale2d(a: Vector2, s: float) -> Vector2:
    return (a[0] * s, a[1] * s)


def _length2d(a: Vector2) -> float:
    return math.sqrt(a[0] * a[0] + a[1] * a[1])


def _normalize2d(a: Vector2) -> Vector2:
    l = _length2d(a)
    if l < 1e-12:
        return (0.0, 0.0)
    return (a[0] / l, a[1] / l)


def _perp2d(a: Vector2) -> Vector2:
    return (-a[1], a[0])


def _triangle_normal(v0: Vector3, v1: Vector3, v2: Vector3) -> Vector3:
    e1 = (v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2])
    e2 = (v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2])
    n = (
        e1[1] * e2[2] - e1[2] * e2[1],
        e1[2] * e2[0] - e1[0] * e2[2],
        e1[0] * e2[1] - e1[1] * e2[0],
    )
    l = math.sqrt(n[0] ** 2 + n[1] ** 2 + n[2] ** 2)
    if l < 1e-12:
        return (0.0, 0.0, 1.0)
    return (n[0] / l, n[1] / l, n[2] / l)


def _segment_circle_intersect(
    a: Vector2, b: Vector2, center: Vector2, radius: float
) -> bool:
    d = _sub2d(b, a)
    f = _sub2d(a, center)
    A = _dot2d(d, d)
    B = 2.0 * _dot2d(f, d)
    C = _dot2d(f, f) - radius * radius
    disc = B * B - 4.0 * A * C
    if disc < 0:
        return False
    disc = math.sqrt(disc)
    t1 = (-B - disc) / (2.0 * A)
    t2 = (-B + disc) / (2.0 * A)
    return (0.0 <= t1 <= 1.0) or (0.0 <= t2 <= 1.0)


# ---------------------------------------------------------------------------
# Polygon class (2D closed polyline)
# ---------------------------------------------------------------------------


class Polygon:
    __slots__ = ("points",)

    def __init__(self, points: Optional[List[Vector2]] = None):
        self.points: List[Vector2] = points if points is not None else []

    def area(self) -> float:
        n = len(self.points)
        if n < 3:
            return 0.0
        a = 0.0
        for i in range(n):
            j = (i + 1) % n
            a += self.points[i][0] * self.points[j][1]
            a -= self.points[j][0] * self.points[i][1]
        return a / 2.0

    def is_ccw(self) -> bool:
        return self.area() > 0

    def ensure_ccw(self) -> None:
        if not self.is_ccw():
            self.points.reverse()

    def centroid(self) -> Vector2:
        n = len(self.points)
        if n == 0:
            return (0.0, 0.0)
        cx = sum(p[0] for p in self.points) / n
        cy = sum(p[1] for p in self.points) / n
        return (cx, cy)

    def bbox(self) -> Tuple[float, float, float, float]:
        xs = [p[0] for p in self.points]
        ys = [p[1] for p in self.points]
        return (min(xs), min(ys), max(xs), max(ys))

    def contains_point(self, pt: Vector2) -> bool:
        inside = False
        n = len(self.points)
        j = n - 1
        for i in range(n):
            xi, yi = self.points[i]
            xj, yj = self.points[j]
            if ((yi > pt[1]) != (yj > pt[1])) and (
                pt[0] < (xj - xi) * (pt[1] - yi) / (yj - yi + 1e-30) + xi
            ):
                inside = not inside
            j = i
        return inside

    def copy(self) -> "Polygon":
        return Polygon(list(self.points))


# ---------------------------------------------------------------------------
# Simple polygon offset (inward / outward)
# ---------------------------------------------------------------------------


def _offset_polygon(poly: Polygon, distance: float) -> List[Polygon]:
    n = len(poly.points)
    if n < 3:
        return []

    result_pts: List[Vector2] = []
    for i in range(n):
        prev = poly.points[(i - 1) % n]
        curr = poly.points[i]
        nxt = poly.points[(i + 1) % n]

        e1 = _normalize2d(_sub2d(curr, prev))
        e2 = _normalize2d(_sub2d(nxt, curr))

        n1 = _perp2d(e1)
        n2 = _perp2d(e2)

        d1 = (n1[0] * distance, n1[1] * distance)
        d2 = (n2[0] * distance, n2[1] * distance)

        p1 = _add2d(curr, d1)
        p2 = _add2d(curr, d2)

        det = _cross2d(e1, e2)
        if abs(det) < 1e-10:
            result_pts.append(p1)
            continue

        dx = p2[0] - p1[0]
        dy = p2[1] - p1[1]
        t = (dx * e1[1] - dy * e1[0]) / det

        ix = p1[0] + e2[0] * t
        iy = p1[1] + e2[1] * t
        result_pts.append((ix, iy))

    offset = Polygon(result_pts)
    if offset.area() < 1e-6:
        return []

    if poly.is_ccw() and not offset.is_ccw():
        return []
    if not poly.is_ccw() and offset.is_ccw():
        return []

    return [offset]


def _offset_polygons(polys: List[Polygon], distance: float) -> List[Polygon]:
    result: List[Polygon] = []
    for p in polys:
        result.extend(_offset_polygon(p, distance))
    return result


# ---------------------------------------------------------------------------
# Line-polygon intersection for infill clipping
# ---------------------------------------------------------------------------


def _clip_line_to_polygon(
    line_a: Vector2, line_b: Vector2, poly: Polygon
) -> List[Segment2D]:
    intersections: List[Tuple[float, Vector2]] = []
    n = len(poly.points)
    d = _sub2d(line_b, line_a)
    dl = _length2d(d)
    if dl < 1e-12:
        return []

    for i in range(n):
        j = (i + 1) % n
        e_a = poly.points[i]
        e_b = poly.points[j]
        e = _sub2d(e_b, e_a)
        denom = d[0] * e[1] - d[1] * e[0]
        if abs(denom) < 1e-12:
            continue
        w = _sub2d(e_a, line_a)
        t = (w[0] * e[1] - w[1] * e[0]) / denom
        s = (w[0] * d[1] - w[1] * d[0]) / denom
        if 0.0 <= t <= 1.0 and 0.0 <= s <= 1.0:
            pt = _add2d(line_a, _scale2d(d, t))
            intersections.append((t, pt))

    intersections.sort(key=lambda x: x[0])
    segs: List[Segment2D] = []
    for k in range(0, len(intersections) - 1, 2):
        segs.append((intersections[k][1], intersections[k + 1][1]))
    return segs


def _point_in_polygons(pt: Vector2, polys: List[Polygon]) -> bool:
    for p in polys:
        if p.contains_point(pt):
            return True
    return False


# ---------------------------------------------------------------------------
# Infill patterns
# ---------------------------------------------------------------------------


class InfillPattern(Enum):
    GRID = auto()
    LINES = auto()
    GYROID = auto()
    HONEYCOMB = auto()
    CUBIC = auto()
    CONCENTRIC = auto()
    RECTILINEAR = auto()


class InfillGenerator:
    def __init__(self, settings: Dict[str, Any]):
        self.settings = settings

    def generate(
        self,
        boundary: List[Polygon],
        holes: List[Polygon],
        layer_index: int,
        layer_z: float,
        pattern: str,
        density: float,
    ) -> List[Segment2D]:
        if density <= 0:
            return []
        if not boundary:
            return []

        bbox_min_x = min(p[0] for poly in boundary for p in poly.points)
        bbox_min_y = min(p[1] for poly in boundary for p in poly.points)
        bbox_max_x = max(p[0] for poly in boundary for p in poly.points)
        bbox_max_y = max(p[1] for poly in boundary for p in poly.points)

        spacing = self._spacing_for_density(density)
        pat = pattern.lower()
        lines: List[Segment2D] = []

        if pat == "grid":
            lines.extend(
                self._grid_lines(
                    bbox_min_x, bbox_min_y, bbox_max_x, bbox_max_y, spacing, layer_index
                )
            )
        elif pat == "lines":
            lines.extend(
                self._lines_lines(
                    bbox_min_x, bbox_min_y, bbox_max_x, bbox_max_y, spacing, layer_index
                )
            )
        elif pat == "gyroid":
            lines.extend(
                self._gyroid_lines(
                    bbox_min_x, bbox_min_y, bbox_max_x, bbox_max_y, spacing, layer_z
                )
            )
        elif pat == "honeycomb":
            lines.extend(
                self._honeycomb_lines(
                    bbox_min_x, bbox_min_y, bbox_max_x, bbox_max_y, spacing, layer_z
                )
            )
        elif pat == "cubic":
            lines.extend(
                self._cubic_lines(
                    bbox_min_x, bbox_min_y, bbox_max_x, bbox_max_y, spacing, layer_index
                )
            )
        elif pat == "concentric":
            lines.extend(self._concentric_lines(boundary, spacing))
        else:
            lines.extend(
                self._lines_lines(
                    bbox_min_x, bbox_min_y, bbox_max_x, bbox_max_y, spacing, layer_index
                )
            )

        result: List[Segment2D] = []
        for seg in lines:
            clipped = _clip_line_to_polygon(seg[0], seg[1], boundary[0])
            for c in clipped:
                if not any(_point_in_polygons(pt, holes) for pt in (c[0], c[1])):
                    result.append(c)
        return result

    def _spacing_for_density(self, density: float) -> float:
        line_width = self.settings.get("wall_line_width", 0.4)
        return max(line_width, line_width / max(density, 0.01) * 2.0)

    def _grid_lines(
        self,
        min_x: float,
        min_y: float,
        max_x: float,
        max_y: float,
        spacing: float,
        layer_index: int,
    ) -> List[Segment2D]:
        lines: List[Segment2D] = []
        offset = spacing / 2.0
        x = min_x - spacing
        while x <= max_x + spacing:
            lines.append(((x, min_y - offset), (x, max_y + offset)))
            x += spacing
        y = min_y - spacing
        while y <= max_y + spacing:
            lines.append(((min_x - offset, y), (max_x + offset, y)))
            y += spacing
        return lines

    def _lines_lines(
        self,
        min_x: float,
        min_y: float,
        max_x: float,
        max_y: float,
        spacing: float,
        layer_index: int,
    ) -> List[Segment2D]:
        lines: List[Segment2D] = []
        diag = math.sqrt((max_x - min_x) ** 2 + (max_y - min_y) ** 2)
        cx = (min_x + max_x) / 2.0
        cy = (min_y + max_y) / 2.0
        angle = math.pi / 4.0 if layer_index % 2 == 0 else -math.pi / 4.0
        dx = math.cos(angle)
        dy = math.sin(angle)
        nx = -dy
        ny = dx
        t = -diag
        while t <= diag:
            px = cx + dx * t
            py = cy + dy * t
            a = (px - nx * diag, py - ny * diag)
            b = (px + nx * diag, py + ny * diag)
            lines.append((a, b))
            t += spacing
        return lines

    def _gyroid_lines(
        self,
        min_x: float,
        min_y: float,
        max_x: float,
        max_y: float,
        spacing: float,
        z: float,
    ) -> List[Segment2D]:
        lines: List[Segment2D] = []
        resolution = spacing * 0.5
        freq = 2.0 * math.pi / (spacing * 4.0)
        y = min_y
        while y <= max_y:
            pts: List[Vector2] = []
            x = min_x
            while x <= max_x:
                v = math.sin(freq * x) * math.cos(freq * y) + math.sin(
                    freq * y
                ) * math.cos(freq * z)
                if abs(v) < 0.15:
                    pts.append((x, y))
                x += resolution
            for i in range(len(pts) - 1):
                if _length2d(_sub2d(pts[i + 1], pts[i])) < spacing:
                    lines.append((pts[i], pts[i + 1]))
            y += spacing
        return lines

    def _honeycomb_lines(
        self,
        min_x: float,
        min_y: float,
        max_x: float,
        max_y: float,
        spacing: float,
        z: float,
    ) -> List[Segment2D]:
        lines: List[Segment2D] = []
        hex_r = spacing * 0.6
        row_h = hex_r * math.sqrt(3.0)
        row = 0
        y = min_y - row_h
        while y <= max_y + row_h:
            x_off = hex_r if row % 2 else 0.0
            x = min_x - hex_r + x_off
            while x <= max_x + hex_r:
                cx = x
                cy = y
                pts = []
                for k in range(6):
                    a = math.pi / 3.0 * k + math.pi / 6.0
                    pts.append((cx + hex_r * math.cos(a), cy + hex_r * math.sin(a)))
                for k in range(6):
                    lines.append((pts[k], pts[(k + 1) % 6]))
                x += hex_r * 3.0
            y += row_h
            row += 1
        return lines

    def _cubic_lines(
        self,
        min_x: float,
        min_y: float,
        max_x: float,
        max_y: float,
        spacing: float,
        layer_index: int,
    ) -> List[Segment2D]:
        lines: List[Segment2D] = []
        layer_offset = (layer_index % 3) * spacing / 3.0
        x = min_x - spacing + layer_offset
        while x <= max_x + spacing:
            lines.append(((x, min_y - spacing), (x, max_y + spacing)))
            x += spacing
        return lines

    def _concentric_lines(
        self, boundary: List[Polygon], spacing: float
    ) -> List[Segment2D]:
        lines: List[Segment2D] = []
        polys = list(boundary)
        for ring in range(1, 50):
            offset = _offset_polygons(polys, -spacing * ring)
            if not offset:
                break
            for p in offset:
                for i in range(len(p.points)):
                    j = (i + 1) % len(p.points)
                    lines.append((p.points[i], p.points[j]))
            polys = offset
        return lines


# ---------------------------------------------------------------------------
# Support generation
# ---------------------------------------------------------------------------


@dataclass
class SupportConfig:
    enabled: bool = False
    angle: float = 45.0
    pillar_size: float = 2.0
    pillar_spacing: float = 5.0
    interface_layers: int = 3
    interface_density: float = 0.8
    base_density: float = 0.15
    xy_distance: float = 0.8
    z_distance: float = 0.2
    tree_enabled: bool = False


@dataclass
class SupportPillar:
    x: float
    y: float
    bottom_z: float
    top_z: float


class SupportGenerator:
    def __init__(self, settings: Dict[str, Any]):
        self.config = SupportConfig(
            enabled=settings.get("support_enabled", False),
            angle=settings.get("support_angle", 45.0),
            tree_enabled=settings.get("support_tree", False),
        )

    def generate(
        self,
        vertices: List[Vector3],
        faces: List[Tuple[int, int, int]],
        layer_heights: List[float],
    ) -> Tuple[List[SupportPillar], List[List[Segment2D]]]:
        if not self.config.enabled:
            return [], []

        overhangs = self._detect_overhangs(vertices, faces)
        if not overhangs:
            return [], []

        pillars = self._create_pillars(overhangs, layer_heights)
        layers_support = self._generate_layer_support(pillars, layer_heights)
        return pillars, layers_support

    def _detect_overhangs(
        self, vertices: List[Vector3], faces: List[Tuple[int, int, int]]
    ) -> List[Tuple[Vector3, Vector3, float]]:
        threshold = math.cos(math.radians(90.0 - self.config.angle))
        overhangs: List[Tuple[Vector3, Vector3, float]] = []
        for i0, i1, i2 in faces:
            if i0 >= len(vertices) or i1 >= len(vertices) or i2 >= len(vertices):
                continue
            n = _triangle_normal(vertices[i0], vertices[i1], vertices[i2])
            nz = n[2]
            if nz < -abs(threshold):
                centroid_z = (vertices[i0][2] + vertices[i1][2] + vertices[i2][2]) / 3.0
                cx = (vertices[i0][0] + vertices[i1][0] + vertices[i2][0]) / 3.0
                cy = (vertices[i0][1] + vertices[i1][1] + vertices[i2][1]) / 3.0
                overhangs.append(((cx, cy, centroid_z), (n[0], n[1], n[2]), nz))
        return overhangs

    def _create_pillars(
        self,
        overhangs: List[Tuple[Vector3, Vector3, float]],
        layer_heights: List[float],
    ) -> List[SupportPillar]:
        pillars: List[SupportPillar] = []
        spacing = self.config.pillar_spacing
        for centroid, normal, _ in overhangs:
            cx, cy, cz = centroid
            gx = round(cx / spacing) * spacing
            gy = round(cy / spacing) * spacing
            bottom = 0.0
            top = cz
            pillars.append(SupportPillar(gx, gy, bottom, top))
        return pillars

    def _generate_layer_support(
        self,
        pillars: List[SupportPillar],
        layer_heights: List[float],
    ) -> List[List[Segment2D]]:
        result: List[List[Segment2D]] = [[] for _ in layer_heights]
        ps = self.config.pillar_size / 2.0
        for pillar in pillars:
            for idx, z in enumerate(layer_heights):
                if pillar.bottom_z <= z <= pillar.top_z:
                    x, y = pillar.x, pillar.y
                    rect = [
                        (x - ps, y - ps),
                        (x + ps, y - ps),
                        (x + ps, y + ps),
                        (x - ps, y + ps),
                    ]
                    for k in range(4):
                        result[idx].append((rect[k], rect[(k + 1) % 4]))
        return result


# ---------------------------------------------------------------------------
# Layer data
# ---------------------------------------------------------------------------


@dataclass
class Layer:
    index: int
    z: float
    thickness: float
    outer_perimeters: List[Polygon] = field(default_factory=list)
    inner_perimeters: List[Polygon] = field(default_factory=list)
    infill_segments: List[Segment2D] = field(default_factory=list)
    support_segments: List[Segment2D] = field(default_factory=list)
    skin_segments: List[Segment2D] = field(default_factory=list)


# ---------------------------------------------------------------------------
# Slice result
# ---------------------------------------------------------------------------


@dataclass
class SliceResult:
    layers: List[Layer] = field(default_factory=list)
    gcode: str = ""
    print_time_estimate: float = 0.0
    material_estimate: float = 0.0
    layer_count: int = 0
    total_height: float = 0.0


# ---------------------------------------------------------------------------
# G-code generator
# ---------------------------------------------------------------------------


class GcodeGenerator:
    def __init__(self, settings: Dict[str, Any]):
        self.settings = settings
        self.filament_diameter = settings.get("filament_diameter", 1.75)
        self.filament_area = math.pi * (self.filament_diameter / 2.0) ** 2
        self.flow_rate = settings.get("flow_rate", 1.0) / 100.0
        self.retraction_enabled = settings.get("retraction_enabled", True)
        self.retraction_distance = settings.get("retraction_distance", 5.0)
        self.retraction_speed = settings.get("retraction_speed", 45.0) * 60.0
        self.print_speed = settings.get("print_speed", 60.0) * 60.0
        self.travel_speed = settings.get("travel_speed", 120.0) * 60.0
        self.temperature = settings.get("temperature", 210)
        self.bed_temperature = settings.get("bed_temperature", 60)
        self.fan_speed = settings.get("fan_speed", 255)
        self.layer_height = settings.get("layer_height", 0.2)
        self.first_layer_height = settings.get("first_layer_height", 0.3)
        self.absolute_extrusion = True
        self.current_e = 0.0

    def generate(self, layers: List[Layer]) -> str:
        self.current_e = 0.0
        lines: List[str] = []
        self._add_start_gcode(lines)
        lines.append("; Layer count: {}".format(len(layers)))
        lines.append(
            "; Total height: {:.3f} mm".format(
                layers[-1].z + layers[-1].thickness if layers else 0.0
            )
        )
        lines.append("")

        for layer in layers:
            lines.extend(self._generate_layer(layer))
        self._add_end_gcode(lines)
        return "\n".join(lines)

    def _add_start_gcode(self, lines: List[str]) -> None:
        start = self.settings.get("start_gcode", "")
        if start:
            lines.append(start)
            return
        lines.append("; TungstenCad FDM Slicer - Start G-code")
        lines.append("G28 ; Home all axes")
        lines.append("M104 S{} ; Set nozzle temperature".format(self.temperature))
        lines.append("M140 S{} ; Set bed temperature".format(self.bed_temperature))
        lines.append("M190 S{} ; Wait for bed temperature".format(self.bed_temperature))
        lines.append("M109 S{} ; Wait for nozzle temperature".format(self.temperature))
        lines.append("G29 ; Auto bed leveling")
        lines.append("G92 E0 ; Reset extruder")
        lines.append("M82 ; Absolute extrusion mode")
        lines.append("G1 Z5.0 F3000 ; Move up 5mm")
        lines.append("; Prime line")
        lines.append("G1 X0.1 Y20.0 F5000 ; Move to start position")
        lines.append("G1 Z0.3 F3000 ; Lower to prime height")
        lines.append("G1 X0.1 Y150.0 E15.0 F1500 ; Prime line 1")
        lines.append("G1 X0.4 Y150.0 F5000 ; Shift to side")
        lines.append("G1 X0.4 Y20.0 E30.0 F1500 ; Prime line 2")
        lines.append("G92 E0 ; Reset extruder after prime")
        lines.append("")

    def _add_end_gcode(self, lines: List[str]) -> None:
        end = self.settings.get("end_gcode", "")
        if end:
            lines.append(end)
            return
        lines.append("; TungstenCad FDM Slicer - End G-code")
        lines.append("M104 S0 ; Turn off nozzle")
        lines.append("M140 S0 ; Turn off bed")
        lines.append("M107 ; Fan off")
        lines.append("G91 ; Relative positioning")
        lines.append("G1 Z10.0 F3000 ; Raise Z 10mm")
        lines.append("G90 ; Absolute positioning")
        lines.append("G28 X0 Y0 ; Home X and Y")
        lines.append("M84 ; Disable motors")

    def _generate_layer(self, layer: Layer) -> List[str]:
        lines: List[str] = []
        z = layer.z
        thickness = layer.thickness
        is_first = layer.index == 0

        lines.append("; ---- Layer {} ----".format(layer.index))
        lines.append("; Z = {:.3f} mm".format(z + thickness))
        lines.append("G1 Z{:.3f} F{}".format(z + thickness, int(self.travel_speed)))

        if is_first:
            lines.append("M106 S{}".format(int(self.fan_speed * 0.5)))
        else:
            lines.append("M106 S{}".format(self.fan_speed))

        lines.extend(self._generate_perimeters(layer, is_first))
        lines.extend(self._generate_infill(layer, is_first))
        lines.extend(self._generate_skin(layer, is_first))
        lines.extend(self._generate_support_moves(layer))

        return lines

    def _generate_perimeters(self, layer: Layer, is_first: bool) -> List[str]:
        lines: List[str] = []
        speed = self.print_speed if not is_first else self.print_speed * 0.5
        e_factor = self.flow_rate * (
            self.first_layer_height if is_first else layer.thickness
        )

        for poly in layer.outer_perimeters:
            if len(poly.points) < 2:
                continue
            pt = poly.points[0]
            lines.append(
                "G1 X{:.3f} Y{:.3f} F{}".format(pt[0], pt[1], int(self.travel_speed))
            )
            for pt in poly.points[1:]:
                dx = (
                    pt[0] - poly.points[poly.points.index(pt) - 1][0]
                    if poly.points.index(pt) > 0
                    else 0
                )
                dist = _length2d(
                    _sub2d(pt, poly.points[max(0, poly.points.index(pt) - 1)])
                )
                de = dist * e_factor * self.filament_area
                self.current_e += de
                lines.append(
                    "G1 X{:.3f} Y{:.3f} E{:.5f} F{}".format(
                        pt[0], pt[1], self.current_e, int(speed)
                    )
                )
            dist = _length2d(_sub2d(poly.points[0], poly.points[-1]))
            de = dist * e_factor * self.filament_area
            self.current_e += de
            lines.append(
                "G1 X{:.3f} Y{:.3f} E{:.5f} F{}".format(
                    poly.points[0][0], poly.points[0][1], self.current_e, int(speed)
                )
            )

        for poly in layer.inner_perimeters:
            if len(poly.points) < 2:
                continue
            pt = poly.points[0]
            lines.append(
                "G1 X{:.3f} Y{:.3f} F{}".format(pt[0], pt[1], int(self.travel_speed))
            )
            for i in range(1, len(poly.points)):
                dist = _length2d(_sub2d(poly.points[i], poly.points[i - 1]))
                de = dist * e_factor * self.filament_area
                self.current_e += de
                lines.append(
                    "G1 X{:.3f} Y{:.3f} E{:.5f} F{}".format(
                        poly.points[i][0], poly.points[i][1], self.current_e, int(speed)
                    )
                )
            dist = _length2d(_sub2d(poly.points[0], poly.points[-1]))
            de = dist * e_factor * self.filament_area
            self.current_e += de
            lines.append(
                "G1 X{:.3f} Y{:.3f} E{:.5f} F{}".format(
                    poly.points[0][0], poly.points[0][1], self.current_e, int(speed)
                )
            )

        return lines

    def _generate_infill(self, layer: Layer, is_first: bool) -> List[str]:
        lines: List[str] = []
        speed = self.print_speed if not is_first else self.print_speed * 0.5
        e_factor = self.flow_rate * (
            self.first_layer_height if is_first else layer.thickness
        )

        if not layer.infill_segments:
            return lines

        lines.append("; Infill")
        sorted_segs = self._sort_segments(layer.infill_segments)

        if self.retraction_enabled and sorted_segs:
            lines.append("G10 ; Retract")

        prev_end: Optional[Vector2] = None
        for seg in sorted_segs:
            if prev_end is not None:
                travel = _length2d(_sub2d(seg[0], prev_end))
                if travel > 0.1:
                    lines.append(
                        "G1 X{:.3f} Y{:.3f} F{}".format(
                            seg[0][0], seg[0][1], int(self.travel_speed)
                        )
                    )
            else:
                lines.append(
                    "G1 X{:.3f} Y{:.3f} F{}".format(
                        seg[0][0], seg[0][1], int(self.travel_speed)
                    )
                )

            if self.retraction_enabled and prev_end is not None:
                travel = _length2d(_sub2d(seg[0], prev_end))
                if travel > 0.1:
                    lines.append("G11 ; Unretract")

            dist = _length2d(_sub2d(seg[1], seg[0]))
            de = dist * e_factor * self.filament_area
            self.current_e += de
            lines.append(
                "G1 X{:.3f} Y{:.3f} E{:.5f} F{}".format(
                    seg[1][0], seg[1][1], self.current_e, int(speed)
                )
            )
            prev_end = seg[1]

        if self.retraction_enabled and sorted_segs:
            lines.append("G10 ; Retract")

        return lines

    def _generate_skin(self, layer: Layer, is_first: bool) -> List[str]:
        lines: List[str] = []
        speed = self.print_speed if not is_first else self.print_speed * 0.5
        e_factor = self.flow_rate * (
            self.first_layer_height if is_first else layer.thickness
        )

        if not layer.skin_segments:
            return lines

        lines.append("; Solid skin")
        for seg in layer.skin_segments:
            lines.append(
                "G1 X{:.3f} Y{:.3f} F{}".format(
                    seg[0][0], seg[0][1], int(self.travel_speed)
                )
            )
            dist = _length2d(_sub2d(seg[1], seg[0]))
            de = dist * e_factor * self.filament_area
            self.current_e += de
            lines.append(
                "G1 X{:.3f} Y{:.3f} E{:.5f} F{}".format(
                    seg[1][0], seg[1][1], self.current_e, int(speed)
                )
            )
        return lines

    def _generate_support_moves(self, layer: Layer) -> List[str]:
        lines: List[str] = []
        if not layer.support_segments:
            return lines
        e_factor = self.flow_rate * layer.thickness
        lines.append("; Support")
        for seg in layer.support_segments:
            lines.append(
                "G1 X{:.3f} Y{:.3f} F{}".format(
                    seg[0][0], seg[0][1], int(self.travel_speed)
                )
            )
            dist = _length2d(_sub2d(seg[1], seg[0]))
            de = dist * e_factor * self.filament_area * 0.5
            self.current_e += de
            lines.append(
                "G1 X{:.3f} Y{:.3f} E{:.5f} F{}".format(
                    seg[1][0], seg[1][1], self.current_e, int(self.print_speed * 0.7)
                )
            )
        return lines

    def _sort_segments(self, segments: List[Segment2D]) -> List[Segment2D]:
        if not segments:
            return []
        remaining = list(segments)
        sorted_segs: List[Segment2D] = [remaining.pop(0)]
        while remaining:
            end_pt = sorted_segs[-1][1]
            best_idx = 0
            best_dist = float("inf")
            best_flip = False
            for i, seg in enumerate(remaining):
                d1 = _length2d(_sub2d(seg[0], end_pt))
                d2 = _length2d(_sub2d(seg[1], end_pt))
                if d1 < best_dist:
                    best_dist = d1
                    best_idx = i
                    best_flip = False
                if d2 < best_dist:
                    best_dist = d2
                    best_idx = i
                    best_flip = True
            seg = remaining.pop(best_idx)
            if best_flip:
                sorted_segs.append((seg[1], seg[0]))
            else:
                sorted_segs.append(seg)
        return sorted_segs

    def estimate_material(self, layers: List[Layer]) -> float:
        total_e = 0.0
        for layer in layers:
            e_factor = self.flow_rate * layer.thickness
            for poly in layer.outer_perimeters + layer.inner_perimeters:
                for i in range(len(poly.points)):
                    j = (i + 1) % len(poly.points)
                    total_e += (
                        _length2d(_sub2d(poly.points[j], poly.points[i])) * e_factor
                    )
            for seg in layer.infill_segments + layer.skin_segments:
                total_e += _length2d(_sub2d(seg[1], seg[0])) * e_factor
        volume = total_e * self.filament_area
        density = 1.24
        return volume * density / 1000.0

    def estimate_time(self, layers: List[Layer]) -> float:
        total_time = 0.0
        for layer in layers:
            travel_time = 0.0
            print_time = 0.0
            for poly in layer.outer_perimeters + layer.inner_perimeters:
                for i in range(len(poly.points)):
                    j = (i + 1) % len(poly.points)
                    dist = _length2d(_sub2d(poly.points[j], poly.points[i]))
                    print_time += dist / (self.print_speed / 60.0)
            for seg in layer.infill_segments + layer.skin_segments:
                dist = _length2d(_sub2d(seg[1], seg[0]))
                print_time += dist / (self.print_speed / 60.0)
            total_time += travel_time + print_time
        start_time = 30.0
        end_time = 10.0
        return start_time + total_time + end_time


# ---------------------------------------------------------------------------
# Mesh data container
# ---------------------------------------------------------------------------


@dataclass
class MeshData:
    vertices: List[Vector3] = field(default_factory=list)
    faces: List[Tuple[int, int, int]] = field(default_factory=list)

    def bounds(self) -> Tuple[Vector3, Vector3]:
        if not self.vertices:
            return ((0, 0, 0), (0, 0, 0))
        xs = [v[0] for v in self.vertices]
        ys = [v[1] for v in self.vertices]
        zs = [v[2] for v in self.vertices]
        return ((min(xs), min(ys), min(zs)), (max(xs), max(ys), max(zs)))


# ---------------------------------------------------------------------------
# Main FDM Slicer
# ---------------------------------------------------------------------------


class FDMSlicer:
    def __init__(self, settings: Optional[Dict[str, Any]] = None):
        self.settings = settings or self._default_settings()
        self.infill_gen = InfillGenerator(self.settings)
        self.support_gen = SupportGenerator(self.settings)
        self.gcode_gen = GcodeGenerator(self.settings)

    @staticmethod
    def _default_settings() -> Dict[str, Any]:
        return {
            "layer_height": 0.2,
            "first_layer_height": 0.3,
            "nozzle_diameter": 0.4,
            "bed_size": (220.0, 220.0, 250.0),
            "print_speed": 60.0,
            "travel_speed": 120.0,
            "infill_percentage": 20.0,
            "infill_pattern": "grid",
            "wall_count": 3,
            "wall_line_width": 0.4,
            "top_layers": 4,
            "bottom_layers": 4,
            "support_enabled": False,
            "support_angle": 45.0,
            "temperature": 210,
            "bed_temperature": 60,
            "filament_diameter": 1.75,
            "retraction_enabled": True,
            "retraction_distance": 5.0,
            "retraction_speed": 45.0,
            "flow_rate": 100.0,
            "start_gcode": "",
            "end_gcode": "",
        }

    def slice(
        self, mesh_data: MeshData, settings: Optional[Dict[str, Any]] = None
    ) -> SliceResult:
        if settings:
            self.settings.update(settings)
            self.infill_gen = InfillGenerator(self.settings)
            self.support_gen = SupportGenerator(self.settings)
            self.gcode_gen = GcodeGenerator(self.settings)

        result = SliceResult()
        if not mesh_data.vertices or not mesh_data.faces:
            result.gcode = "; Empty mesh - no geometry to slice\n"
            return result

        t0 = time.time()

        bounds = mesh_data.bounds()
        min_z = bounds[0][2]
        max_z = bounds[1][2]

        first_h = self.settings.get("first_layer_height", 0.3)
        layer_h = self.settings.get("layer_height", 0.2)
        wall_count = self.settings.get("wall_count", 3)
        wall_width = self.settings.get("wall_line_width", 0.4)
        top_layers = self.settings.get("top_layers", 4)
        bottom_layers = self.settings.get("bottom_layers", 4)
        infill_pct = self.settings.get("infill_percentage", 20.0)
        infill_pattern = self.settings.get("infill_pattern", "grid")

        layer_heights: List[float] = []
        z = min_z
        first = True
        while z < max_z:
            layer_heights.append(z)
            h = first_h if first else layer_h
            z += h
            first = False

        result.total_height = max_z - min_z
        result.layer_count = len(layer_heights)

        layers: List[Layer] = []

        for li, z in enumerate(layer_heights):
            h = first_h if li == 0 else layer_h
            layer = Layer(index=li, z=z, thickness=h)
            polys = self._slice_layer(mesh_data, z, h)
            if not polys:
                layers.append(layer)
                continue

            for p in polys:
                p.ensure_ccw()

            layer.outer_perimeters = polys
            inner_polys = list(polys)
            for w in range(1, wall_count):
                inner_polys = _offset_polygons(inner_polys, -wall_width)
                if not inner_polys:
                    break
            layer.inner_perimeters = inner_polys

            inner_boundary = (
                layer.inner_perimeters
                if layer.inner_perimeters
                else layer.outer_perimeters
            )
            if not inner_boundary:
                layers.append(layer)
                continue

            if li < bottom_layers or li >= len(layer_heights) - top_layers:
                skin_density = 1.0
                layer.skin_segments = self.infill_gen.generate(
                    inner_boundary, [], li, z + h, "lines", skin_density
                )
            else:
                layer.infill_segments = self.infill_gen.generate(
                    inner_boundary, [], li, z + h, infill_pattern, infill_pct / 100.0
                )

            layers.append(layer)

        if self.settings.get("support_enabled", False):
            pillars, support_per_layer = self.support_gen.generate(
                mesh_data.vertices, mesh_data.faces, layer_heights
            )
            for i, segs in enumerate(support_per_layer):
                if i < len(layers):
                    layers[i].support_segments = segs

        result.layers = layers
        result.gcode = self.gcode_gen.generate(layers)
        result.material_estimate = self.gcode_gen.estimate_material(layers)
        result.print_time_estimate = self.gcode_gen.estimate_time(layers)

        elapsed = time.time() - t0
        result.gcode = "; Sliced in {:.2f} seconds\n".format(elapsed) + result.gcode

        return result

    def _slice_layer(self, mesh: MeshData, z: float, thickness: float) -> List[Polygon]:
        segments: List[Segment2D] = []
        z_max = z + thickness

        for i0, i1, i2 in mesh.faces:
            if (
                i0 >= len(mesh.vertices)
                or i1 >= len(mesh.vertices)
                or i2 >= len(mesh.vertices)
            ):
                continue
            v0 = mesh.vertices[i0]
            v1 = mesh.vertices[i1]
            v2 = mesh.vertices[i2]

            zs = sorted([v0[2], v1[2], v2[2]])
            if zs[2] < z or zs[0] > z_max:
                continue

            segs = self._intersect_triangle(v0, v1, v2, z, z_max)
            segments.extend(segs)

        return self._connect_segments(segments)

    def _intersect_triangle(
        self, v0: Vector3, v1: Vector3, v2: Vector3, z_lo: float, z_hi: float
    ) -> List[Segment2D]:
        segments: List[Segment2D] = []
        plane_z = z_lo

        edges = [(v0, v1), (v1, v2), (v2, v0)]
        pts: List[Vector2] = []
        for a, b in edges:
            if (a[2] - plane_z) * (b[2] - plane_z) < 0:
                t = (plane_z - a[2]) / (b[2] - a[2] + 1e-30)
                ix = a[0] + t * (b[0] - a[0])
                iy = a[1] + t * (b[1] - a[1])
                pts.append((ix, iy))
            elif abs(a[2] - plane_z) < 1e-6:
                pts.append((a[0], a[1]))

        if len(pts) >= 2:
            segments.append((pts[0], pts[1]))
        elif len(pts) == 3:
            segments.append((pts[0], pts[1]))
            segments.append((pts[1], pts[2]))

        return segments

    def _connect_segments(self, segments: List[Segment2D]) -> List[Polygon]:
        if not segments:
            return []

        eps = 0.001
        lines: List[List[Vector2]] = [[s[0], s[1]] for s in segments]
        changed = True
        while changed:
            changed = False
            for i in range(len(lines)):
                if not lines[i]:
                    continue
                for j in range(i + 1, len(lines)):
                    if not lines[j]:
                        continue
                    hi = lines[i][-1]
                    hj_start = lines[j][0]
                    if (
                        abs(hi[0] - hj_start[0]) < eps
                        and abs(hi[1] - hj_start[1]) < eps
                    ):
                        lines[i].extend(lines[j][1:])
                        lines[j] = []
                        changed = True
                        break
                    hi_start = lines[i][0]
                    hj = lines[j][-1]
                    if (
                        abs(hi_start[0] - hj[0]) < eps
                        and abs(hi_start[1] - hj[1]) < eps
                    ):
                        lines[i] = lines[j][:-1] + lines[i]
                        lines[j] = []
                        changed = True
                        break
                    hj_end = lines[j][-1]
                    if abs(hi[0] - hj_end[0]) < eps and abs(hi[1] - hj_end[1]) < eps:
                        lines[i].extend(reversed(lines[j][:-1]))
                        lines[j] = []
                        changed = True
                        break
                    hi_end = lines[i][0]
                    hj_start2 = lines[j][0]
                    if (
                        abs(hi_end[0] - hj_start2[0]) < eps
                        and abs(hi_end[1] - hj_start2[1]) < eps
                    ):
                        lines[i] = list(reversed(lines[i]))
                        lines[i].extend(lines[j][1:])
                        lines[j] = []
                        changed = True
                        break

        polygons: List[Polygon] = []
        for line in lines:
            if len(line) < 3:
                continue
            first = line[0]
            last = line[-1]
            dist = _length2d(_sub2d(first, last))
            if dist < eps:
                poly = Polygon(line[:-1])
                if len(poly.points) >= 3 and abs(poly.area()) > 0.01:
                    polygons.append(poly)
            else:
                if len(line) >= 3:
                    poly = Polygon(line)
                    if abs(poly.area()) > 0.01:
                        polygons.append(poly)

        return polygons


# ---------------------------------------------------------------------------
# Standalone SupportGenerator public API (class alias)
# ---------------------------------------------------------------------------


@dataclass
class SupportResult:
    pillars: List[SupportPillar] = field(default_factory=list)
    layer_support: List[List[Segment2D]] = field(default_factory=list)
    raft_layers: List[List[Segment2D]] = field(default_factory=list)


class StandaloneSupportGenerator:
    def __init__(self, settings: Optional[Dict[str, Any]] = None):
        self.settings = settings or {}
        self.gen = SupportGenerator(self.settings)

    def generate_support(
        self,
        model_mesh: MeshData,
        settings: Optional[Dict[str, Any]] = None,
    ) -> SupportResult:
        if settings:
            self.settings.update(settings)
            self.gen = SupportGenerator(self.settings)

        layer_h = self.settings.get("layer_height", 0.2)
        first_h = self.settings.get("first_layer_height", 0.3)
        bounds = model_mesh.bounds()
        min_z = bounds[0][2]
        max_z = bounds[1][2]

        heights: List[float] = []
        z = min_z
        first = True
        while z < max_z:
            heights.append(z)
            h = first_h if first else layer_h
            z += h
            first = False

        pillars, layer_support = self.gen.generate(
            model_mesh.vertices, model_mesh.faces, heights
        )
        result = SupportResult(pillars=pillars, layer_support=layer_support)

        if self.settings.get("raft_enabled", False):
            result.raft_layers = self._generate_raft(bounds, layer_h)

        return result

    def _generate_raft(
        self, bounds: Tuple[Vector3, Vector3], layer_height: float
    ) -> List[List[Segment2D]]:
        min_xy = (bounds[0][0] - 5.0, bounds[0][1] - 5.0)
        max_xy = (bounds[1][0] + 5.0, bounds[1][1] + 5.0)
        raft_layers: List[List[Segment2D]] = []
        raft_thickness = 0.3
        raft_count = 3
        for rl in range(raft_count):
            z = rl * raft_thickness
            segs: List[Segment2D] = []
            spacing = 2.0
            x = min_xy[0]
            while x <= max_xy[0]:
                segs.append(((x, min_xy[1]), (x, max_xy[1])))
                x += spacing
            y = min_xy[1]
            while y <= max_xy[1]:
                segs.append(((min_xy[0], y), (max_xy[0], y)))
                y += spacing
            raft_layers.append(segs)
        return raft_layers
