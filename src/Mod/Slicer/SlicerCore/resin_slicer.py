"""
Resin (MSLA/DLP) Slicer Engine for Tungsten CAD.

Provides layer-by-layer slicing for resin printers with binary mask generation,
anti-aliasing, island detection, hollowing analysis, and drain hole placement.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

import math
import struct
import zlib
from dataclasses import dataclass, field
from typing import List, Optional, Tuple

try:
    import numpy as np

    HAS_NUMPY = True
except ImportError:
    np = None  # type: ignore
    HAS_NUMPY = False


# ---------------------------------------------------------------------------
# Data classes
# ---------------------------------------------------------------------------


@dataclass
class ResinSettings:
    """All tuneable slicer settings."""

    layer_height: float = 0.05
    bottom_layer_height: float = 0.05
    bottom_layer_count: int = 5
    exposure_time: float = 2.5
    bottom_exposure_time: float = 30.0
    light_off_delay: float = 0.0
    lift_height: float = 5.0
    lift_speed: float = 1.0
    retract_speed: float = 1.0
    printer_resolution_x: int = 1440
    printer_resolution_y: int = 2560
    printer_width_mm: float = 68.04
    printer_height_mm: float = 120.96
    anti_aliasing: bool = True
    anti_aliasing_level: int = 4
    bottom_lift_height: float = 5.0
    bottom_retract_speed: float = 1.0

    @property
    def pixel_width_mm(self) -> float:
        return self.printer_width_mm / self.printer_resolution_x

    @property
    def pixel_height_mm(self) -> float:
        return self.printer_height_mm / self.printer_resolution_y

    @property
    def pixel_diagonal_mm(self) -> float:
        return math.sqrt(self.pixel_width_mm**2 + self.pixel_height_mm**2)


@dataclass
class ResinLayer:
    """One layer of the print."""

    z_height: float
    bitmap: List[List[int]]
    exposure_time: float
    light_off_delay: float
    lift_height: float
    lift_speed: float
    retract_speed: float
    thumbnail: Optional[List[List[int]]] = None
    is_bottom_layer: bool = False

    @property
    def pixel_count(self) -> int:
        count = 0
        for row in self.bitmap:
            for px in row:
                if px > 0:
                    count += 1
        return count


@dataclass
class IslandInfo:
    """Per-layer island data."""

    layer_index: int
    new_islands: int = 0
    completed_islands: int = 0
    island_polygons: List[List[Tuple[float, float]]] = field(default_factory=list)


@dataclass
class ResinSliceResult:
    """Full output of the slicing pipeline."""

    layers: List[ResinLayer] = field(default_factory=list)
    total_layers: int = 0
    total_height: float = 0.0
    island_data: List[IslandInfo] = field(default_factory=list)
    estimated_time: float = 0.0
    estimated_resin_volume: float = 0.0


@dataclass
class CavityInfo:
    """An enclosed void detected during hollowing analysis."""

    index: int
    centroid: Tuple[float, float, float]
    volume_mm3: float
    polygons: List[List[Tuple[float, float]]]
    drain_holes: List[Tuple[float, float, float]] = field(default_factory=list)


# ---------------------------------------------------------------------------
# Triangle / mesh helpers
# ---------------------------------------------------------------------------


def _edge_function(
    ax: float,
    ay: float,
    bx: float,
    by: float,
    cx: float,
    cy: float,
) -> float:
    return (bx - ax) * (cy - ay) - (by - ay) * (cx - ax)


def _intersect_edge(
    v1: Tuple[float, float, float],
    v2: Tuple[float, float, float],
    z: float,
) -> Tuple[float, float]:
    """Interpolate the edge (v1→v2) at the given z-plane. Returns (x, y)."""
    dz = v2[2] - v1[2]
    if abs(dz) < 1e-12:
        t = 0.0
    else:
        t = (z - v1[2]) / dz
    return (v1[0] + t * (v2[0] - v1[0]), v1[1] + t * (v2[1] - v1[1]))


def _triangle_cross_section(
    v1: Tuple[float, float, float],
    v2: Tuple[float, float, float],
    v3: Tuple[float, float, float],
    z: float,
) -> Optional[Tuple[Tuple[float, float], Tuple[float, float]]]:
    """Return the segment where a triangle intersects a z-plane, or None."""
    z_min = min(v1[2], v2[2], v3[2])
    z_max = max(v1[2], v2[2], v3[2])
    if z < z_min - 1e-12 or z > z_max + 1e-12:
        return None

    above = []
    below = []
    on_plane = []
    for v in (v1, v2, v3):
        if abs(v[2] - z) < 1e-12:
            on_plane.append(v)
        elif v[2] > z:
            above.append(v)
        else:
            below.append(v)

    if len(on_plane) == 2:
        return (on_plane[0][:2], on_plane[1][:2])

    if len(on_plane) == 1:
        if len(above) == 1 and len(below) == 1:
            return (on_plane[0][:2], _intersect_edge(above[0], below[0], z))
        elif len(above) == 2:
            return None
        elif len(below) == 2:
            return None

    if len(above) == 1 and len(below) == 1:
        p1 = _intersect_edge(above[0], below[0], z)
        other = above[0] if below[0] == v1 else v1
        if other[2] < z - 1e-12:
            p2 = _intersect_edge(below[0], other, z)
        else:
            p2 = _intersect_edge(above[0], other, z)
        return (p1, p2)

    if len(above) == 2 and len(below) == 1:
        p1 = _intersect_edge(above[0], below[0], z)
        p2 = _intersect_edge(above[1], below[0], z)
        return (p1, p2)

    if len(below) == 2 and len(above) == 1:
        p1 = _intersect_edge(below[0], above[0], z)
        p2 = _intersect_edge(below[1], above[0], z)
        return (p1, p2)

    return None


# ---------------------------------------------------------------------------
# Polygon utilities
# ---------------------------------------------------------------------------


def _polygon_area(poly: List[Tuple[float, float]]) -> float:
    """Signed area via shoelace formula."""
    n = len(poly)
    if n < 3:
        return 0.0
    area = 0.0
    for i in range(n):
        j = (i + 1) % n
        area += poly[i][0] * poly[j][1]
        area -= poly[j][0] * poly[i][1]
    return area * 0.5


def _point_in_polygon(px: float, py: float, poly: List[Tuple[float, float]]) -> bool:
    """Ray-casting point-in-polygon test."""
    n = len(poly)
    inside = False
    j = n - 1
    for i in range(n):
        xi, yi = poly[i]
        xj, yj = poly[j]
        if ((yi > py) != (yj > py)) and (
            px < (xj - xi) * (py - yi) / (yj - yi + 1e-30) + xi
        ):
            inside = not inside
        j = i
    return inside


def _segments_from_cross_sections(
    sections: List[Tuple[Tuple[float, float], Tuple[float, float]]],
) -> List[List[Tuple[float, float]]]:
    """Chain line segments into closed polygons (approximately)."""
    if not sections:
        return []

    eps = 1e-9
    used = [False] * len(sections)
    polys: List[List[Tuple[float, float]]] = []

    for start_idx in range(len(sections)):
        if used[start_idx]:
            continue
        used[start_idx] = True
        seg = sections[start_idx]
        poly: List[Tuple[float, float]] = [seg[0], seg[1]]

        changed = True
        while changed:
            changed = False
            for k in range(len(sections)):
                if used[k]:
                    continue
                s = sections[k]
                head = poly[0]
                tail = poly[-1]

                if abs(s[0][0] - tail[0]) < eps and abs(s[0][1] - tail[1]) < eps:
                    poly.append(s[1])
                    used[k] = True
                    changed = True
                elif abs(s[1][0] - tail[0]) < eps and abs(s[1][1] - tail[1]) < eps:
                    poly.append(s[0])
                    used[k] = True
                    changed = True
                elif abs(s[0][0] - head[0]) < eps and abs(s[0][1] - head[1]) < eps:
                    poly.insert(0, s[1])
                    used[k] = True
                    changed = True
                elif abs(s[1][0] - head[0]) < eps and abs(s[1][1] - head[1]) < eps:
                    poly.insert(0, s[0])
                    used[k] = True
                    changed = True

        if len(poly) >= 3:
            if (
                abs(poly[0][0] - poly[-1][0]) < eps
                and abs(poly[0][1] - poly[-1][1]) < eps
            ):
                poly.pop()
            if len(poly) >= 3:
                polys.append(poly)

    return polys


# ---------------------------------------------------------------------------
# Bitmap helpers (plain Python fallback)
# ---------------------------------------------------------------------------


def _create_bitmap(
    width: int,
    height: int,
    fill: int = 0,
) -> List[List[int]]:
    return [[fill] * width for _ in range(height)]


def _draw_polygon_to_bitmap(
    bitmap: List[List[int]],
    poly: List[Tuple[float, float]],
    resolution_x: int,
    resolution_y: int,
    pixel_w: float,
    pixel_h: float,
    value: int = 255,
) -> None:
    """Rasterise a polygon into the bitmap using scan-line fill."""
    if len(poly) < 3:
        return

    min_y = max(0, int(min(p[1] for p in poly) / pixel_h) - 1)
    max_y = min(resolution_y - 1, int(max(p[1] for p in poly) / pixel_h) + 1)

    for row in range(min_y, max_y + 1):
        y_scan = (row + 0.5) * pixel_h
        intersections: List[float] = []
        n = len(poly)
        j = n - 1
        for i in range(n):
            yi = poly[i][1]
            yj = poly[j][1]
            if (yi <= y_scan < yj) or (yj <= y_scan < yi):
                denom = yj - yi
                if abs(denom) < 1e-30:
                    continue
                x_intersect = poly[i][0] + (y_scan - yi) / denom * (
                    poly[j][0] - poly[i][0]
                )
                intersections.append(x_intersect)
            j = i

        intersections.sort()
        for k in range(0, len(intersections) - 1, 2):
            x_start = intersections[k]
            x_end = intersections[k + 1]
            col_start = max(0, int(x_start / pixel_w))
            col_end = min(resolution_x - 1, int(x_end / pixel_w))
            for col in range(col_start, col_end + 1):
                bitmap[row][col] = value


def _bitmap_from_polygons(
    polygons: List[List[Tuple[float, float]]],
    resolution_x: int,
    resolution_y: int,
    pixel_w: float,
    pixel_h: float,
) -> List[List[int]]:
    """Create a binary bitmap from a list of polygons."""
    bitmap = _create_bitmap(resolution_x, resolution_y, 0)
    for poly in polygons:
        if _polygon_area(poly) < 0:
            poly = list(reversed(poly))
        _draw_polygon_to_bitmap(
            bitmap,
            poly,
            resolution_x,
            resolution_y,
            pixel_w,
            pixel_h,
            255,
        )
    return bitmap


# ---------------------------------------------------------------------------
# Anti-aliasing via distance field
# ---------------------------------------------------------------------------


def _distance_field(bitmap: List[List[int]]) -> List[List[float]]:
    """Compute approximate distance field for a binary bitmap.
    Uses a simple two-pass chamfer distance (3-4 metric)."""
    h = len(bitmap)
    w = len(bitmap[0]) if h > 0 else 0
    INF = 1e18
    dist = [[INF] * w for _ in range(h)]

    for y in range(h):
        for x in range(w):
            if bitmap[y][x] > 127:
                dist[y][x] = 0.0

    for y in range(h):
        for x in range(w):
            d = dist[y][x]
            if d == 0.0:
                continue
            if y > 0:
                d = min(d, dist[y - 1][x] + 1.0)
            if x > 0:
                d = min(d, dist[y][x - 1] + 1.0)
            if y > 0 and x > 0:
                d = min(d, dist[y - 1][x - 1] + 1.414)
            if y > 0 and x < w - 1:
                d = min(d, dist[y - 1][x + 1] + 1.414)
            dist[y][x] = d

    for y in range(h - 1, -1, -1):
        for x in range(w - 1, -1, -1):
            d = dist[y][x]
            if d == 0.0:
                continue
            if y < h - 1:
                d = min(d, dist[y + 1][x] + 1.0)
            if x < w - 1:
                d = min(d, dist[y][x + 1] + 1.0)
            if y < h - 1 and x < w - 1:
                d = min(d, dist[y + 1][x + 1] + 1.414)
            if y < h - 1 and x > 0:
                d = min(d, dist[y + 1][x - 1] + 1.414)
            dist[y][x] = d

    return dist


def _apply_anti_aliasing(
    bitmap: List[List[int]],
    level: int,
    pixel_w: float,
    pixel_h: float,
) -> List[List[int]]:
    """Apply anti-aliasing by softening boundary pixels using the distance field."""
    if level <= 1:
        return bitmap

    h = len(bitmap)
    w = len(bitmap[0]) if h > 0 else 0
    dist = _distance_field(bitmap)

    blend_radius = level * 0.8
    scale = max(pixel_w, pixel_h)

    result = _create_bitmap(w, h, 0)
    for y in range(h):
        for x in range(w):
            d = dist[y][x] * scale
            if bitmap[y][x] > 127:
                if d < blend_radius:
                    alpha = max(0, min(1.0, d / blend_radius))
                    val = int(255 * (1.0 - alpha * 0.6))
                    result[y][x] = max(64, val)
                else:
                    result[y][x] = 255
            else:
                if d < blend_radius and d > 0:
                    alpha = max(0, min(1.0, d / blend_radius))
                    val = int(255 * alpha * 0.4)
                    result[y][x] = min(64, val)
                else:
                    result[y][x] = 0

    return result


# ---------------------------------------------------------------------------
# Island detection
# ---------------------------------------------------------------------------


def _find_connected_components(
    bitmap: List[List[int]],
    threshold: int = 128,
) -> List[List[Tuple[int, int]]]:
    """Flood-fill to find connected components. Returns list of pixel lists."""
    h = len(bitmap)
    w = len(bitmap[0]) if h > 0 else 0
    visited = [[False] * w for _ in range(h)]
    components: List[List[Tuple[int, int]]] = []

    for sy in range(h):
        for sx in range(w):
            if visited[sy][sx] or bitmap[sy][sx] < threshold:
                continue
            stack = [(sx, sy)]
            visited[sy][sx] = True
            pixels: List[Tuple[int, int]] = []
            while stack:
                cx, cy = stack.pop()
                pixels.append((cx, cy))
                for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    nx, ny = cx + dx, cy + dy
                    if 0 <= nx < w and 0 <= ny < h:
                        if not visited[ny][nx] and bitmap[ny][nx] >= threshold:
                            visited[ny][nx] = True
                            stack.append((nx, ny))
            components.append(pixels)

    return components


def _components_overlap(
    comp_a: List[Tuple[int, int]],
    comp_b: List[Tuple[int, int]],
) -> bool:
    """Check if two pixel sets overlap (by bounding-box proximity)."""
    if not comp_a or not comp_b:
        return False
    min_x_a = min(p[0] for p in comp_a)
    max_x_a = max(p[0] for p in comp_a)
    min_y_a = min(p[1] for p in comp_a)
    max_y_a = max(p[1] for p in comp_a)

    min_x_b = min(p[0] for p in comp_b)
    max_x_b = max(p[0] for p in comp_b)
    min_y_b = min(p[1] for p in comp_b)
    max_y_b = max(p[1] for p in comp_b)

    return not (
        max_x_a < min_x_b - 1
        or max_x_b < min_x_a - 1
        or max_y_a < min_y_b - 1
        or max_y_b < min_y_a - 1
    )


def _detect_islands(
    current_bitmap: List[List[int]],
    previous_bitmap: Optional[List[List[int]]],
    layer_index: int,
    pixel_w: float,
    pixel_h: float,
) -> IslandInfo:
    """Compare consecutive layers to detect new islands and completed regions."""
    info = IslandInfo(layer_index=layer_index)

    if previous_bitmap is None:
        comp_count = len(_find_connected_components(current_bitmap))
        info.new_islands = comp_count
        return info

    curr_comps = _find_connected_components(current_bitmap)
    prev_comps = _find_connected_components(previous_bitmap)

    for cc in curr_comps:
        supported = False
        for pc in prev_comps:
            if _components_overlap(cc, pc):
                supported = True
                break
        if not supported:
            info.new_islands += 1
            min_x = min(p[0] for p in cc) * pixel_w
            max_x = max(p[0] for p in cc) * pixel_w
            min_y = min(p[1] for p in cc) * pixel_h
            max_y = max(p[1] for p in cc) * pixel_h
            info.island_polygons.append(
                [
                    (min_x, min_y),
                    (max_x, min_y),
                    (max_x, max_y),
                    (min_x, max_y),
                ]
            )

    for pc in prev_comps:
        still_exists = False
        for cc in curr_comps:
            if _components_overlap(cc, pc):
                still_exists = True
                break
        if not still_exists:
            info.completed_islands += 1

    return info


# ---------------------------------------------------------------------------
# Hollowing detection
# ---------------------------------------------------------------------------


def _rasterize_layer_for_hollow(
    polygons: List[List[Tuple[float, float]]],
    res_x: int,
    res_y: int,
    pixel_w: float,
    pixel_h: float,
) -> List[List[int]]:
    """Return a bitmap where 255 = inside shell, 0 = outside."""
    return _bitmap_from_polygons(polygons, res_x, res_y, pixel_w, pixel_h)


def _flood_fill_from_border(
    bitmap: List[List[int]],
) -> List[List[bool]]:
    """Flood-fill from image border. Returns True for reachable (exterior) pixels."""
    h = len(bitmap)
    w = len(bitmap[0]) if h > 0 else 0
    reachable = [[False] * w for _ in range(h)]
    stack: List[Tuple[int, int]] = []

    for x in range(w):
        if bitmap[0][x] == 0:
            stack.append((x, 0))
            reachable[0][x] = True
        if bitmap[h - 1][x] == 0:
            stack.append((x, h - 1))
            reachable[h - 1][x] = True
    for y in range(h):
        if bitmap[y][0] == 0:
            stack.append((0, y))
            reachable[y][0] = True
        if bitmap[y][w - 1] == 0:
            stack.append((w - 1, y))
            reachable[y][w - 1] = True

    while stack:
        cx, cy = stack.pop()
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nx, ny = cx + dx, cy + dy
            if 0 <= nx < w and 0 <= ny < h:
                if not reachable[ny][nx] and bitmap[ny][nx] == 0:
                    reachable[ny][nx] = True
                    stack.append((nx, ny))

    return reachable


def _detect_cavities(
    cross_sections_per_layer: List[List[List[Tuple[float, float]]]],
    settings: ResinSettings,
) -> List[CavityInfo]:
    """Detect enclosed voids across all layers using flood-fill from outside."""
    res_x = settings.printer_resolution_x
    res_y = settings.printer_resolution_y
    pw = settings.pixel_width_mm
    ph = settings.pixel_height_mm

    layer_cavity_maps: List[List[List[int]]] = []
    for polys in cross_sections_per_layer:
        bm = _rasterize_layer_for_hollow(polys, res_x, res_y, pw, ph)
        reachable = _flood_fill_from_border(bm)
        cavity_bm = _create_bitmap(res_x, res_y, 0)
        for y in range(res_y):
            for x in range(res_x):
                if bm[y][x] > 0 and not reachable[y][x]:
                    cavity_bm[y][x] = 255
        layer_cavity_maps.append(cavity_bm)

    cavity_volumes: List[List[int]] = []
    cavity_idx_map: List[List[List[int]]] = []
    next_idx = 0
    assigned = [[-1] * res_x for _ in range(res_y)]
    for li, cav_bm in enumerate(layer_cavity_maps):
        idx_map = [[-1] * res_x for _ in range(res_y)]
        for y in range(res_y):
            for x in range(res_x):
                if cav_bm[y][x] > 0 and assigned[y][x] == -1:
                    stack = [(x, y)]
                    assigned[y][x] = next_idx
                    idx_map[y][x] = next_idx
                    pixels = [(x, y)]
                    while stack:
                        cx, cy = stack.pop()
                        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                            nx, ny = cx + dx, cy + dy
                            if 0 <= nx < res_x and 0 <= ny < len(cav_bm):
                                if cav_bm[ny][nx] > 0 and assigned[ny][nx] == -1:
                                    assigned[ny][nx] = next_idx
                                    idx_map[ny][nx] = next_idx
                                    stack.append((nx, ny))
                                    pixels.append((nx, ny))
                    cavity_volumes.append(len(pixels))
                    next_idx += 1
        cavity_idx_map.append(idx_map)

    cavities: List[CavityInfo] = []
    for ci in range(next_idx):
        total_px = cavity_volumes[ci]
        vol_mm3 = total_px * pw * ph * settings.layer_height
        if vol_mm3 < 1.0:
            continue

        sum_x = 0.0
        sum_y = 0.0
        sum_z = 0.0
        count = 0
        for li, idx_map in enumerate(cavity_idx_map):
            for y in range(res_y):
                for x in range(res_x):
                    if idx_map[y][x] == ci:
                        sum_x += (x + 0.5) * pw
                        sum_y += (y + 0.5) * ph
                        sum_z += li * settings.layer_height
                        count += 1
        centroid = (sum_x / max(count, 1), sum_y / max(count, 1), sum_z / max(count, 1))

        cavities.append(
            CavityInfo(
                index=ci,
                centroid=centroid,
                volume_mm3=vol_mm3,
                polygons=[],
            )
        )

    return cavities


# ---------------------------------------------------------------------------
# Drain hole placement
# ---------------------------------------------------------------------------


def _find_lowest_points(
    cavity_bm: List[List[int]],
    pixel_w: float,
    pixel_h: float,
    z_height: float,
    count: int = 2,
) -> List[Tuple[float, float, float]]:
    """Find the lowest Y points of a cavity bitmap for drain hole placement."""
    h = len(cavity_bm)
    w = len(cavity_bm[0]) if h > 0 else 0

    candidates: List[Tuple[float, float]] = []
    for y in range(h - 1, -1, -1):
        for x in range(w):
            if cavity_bm[y][x] > 0:
                candidates.append(((x + 0.5) * pixel_w, (y + 0.5) * pixel_h))
        if len(candidates) >= count * 3:
            break

    if len(candidates) < count:
        for y in range(h - 1, -1, -1):
            for x in range(w):
                if cavity_bm[y][x] > 0:
                    px = (x + 0.5) * pixel_w
                    py = (y + 0.5) * pixel_h
                    if not any(
                        abs(px - c[0]) < 1.0 and abs(py - c[1]) < 1.0
                        for c in candidates
                    ):
                        candidates.append((px, py))
                    if len(candidates) >= count:
                        break
            if len(candidates) >= count:
                break

    holes: List[Tuple[float, float, float]] = []
    for px, py in candidates[:count]:
        holes.append((px, py, z_height))
    return holes


def _suggest_drain_holes(
    cavities: List[CavityInfo],
    cross_sections_per_layer: List[List[List[Tuple[float, float]]]],
    settings: ResinSettings,
) -> List[CavityInfo]:
    """Suggest optimal drain hole positions for each cavity."""
    if not cavities:
        return cavities

    res_x = settings.printer_resolution_x
    res_y = settings.printer_resolution_y
    pw = settings.pixel_width_mm
    ph = settings.pixel_height_mm

    for cavity in cavities:
        z_range_min = float("inf")
        z_range_max = float("-inf")
        cavity_layer_bms: List[List[List[int]]] = []

        for li, polys in enumerate(cross_sections_per_layer):
            bm = _rasterize_layer_for_hollow(polys, res_x, res_y, pw, ph)
            reachable = _flood_fill_from_border(bm)
            cav_bm = _create_bitmap(res_x, res_y, 0)
            for y in range(res_y):
                for x in range(res_x):
                    if bm[y][x] > 0 and not reachable[y][x]:
                        cav_bm[y][x] = 255
            cavity_layer_bms.append(cav_bm)

            has_cavity = any(
                cav_bm[y][x] > 0 for y in range(res_y) for x in range(res_x)
            )
            if has_cavity:
                z = li * settings.layer_height
                z_range_min = min(z_range_min, z)
                z_range_max = max(z_range_max, z)

        if z_range_min > z_range_max:
            continue

        lowest_layer = cavity_layer_bms[-1] if cavity_layer_bms else None
        if lowest_layer is None:
            continue

        bottom_z = z_range_max
        holes = _find_lowest_points(lowest_layer, pw, ph, bottom_z, count=2)

        if len(holes) < 2:
            mid_layer_idx = len(cavity_layer_bms) // 2
            mid_bm = cavity_layer_bms[mid_layer_idx]
            mid_z = mid_layer_idx * settings.layer_height
            extra = _find_lowest_points(mid_bm, pw, ph, mid_z, count=2 - len(holes))
            holes.extend(extra)

        cavity.drain_holes = holes

    return cavities


# ---------------------------------------------------------------------------
# PNG thumbnail writer
# ---------------------------------------------------------------------------


def _encode_thumbnail_png(
    bitmap: List[List[int]],
    target_w: int = 120,
    target_h: int = 120,
) -> bytes:
    """Down-sample bitmap to target size and encode as grayscale PNG."""
    src_h = len(bitmap)
    src_w = len(bitmap[0]) if src_h > 0 else 0
    if src_w == 0 or src_h == 0:
        return b""

    rows: List[bytes] = []
    for ty in range(target_h):
        row = b"\x00"
        for tx in range(target_w):
            sx = int(tx * src_w / target_w)
            sy = int(ty * src_h / target_h)
            sx = min(sx, src_w - 1)
            sy = min(sy, src_h - 1)
            v = bitmap[sy][sx]
            row += struct.pack("B", v)
        rows.append(row)

    raw = b"".join(rows)

    def _chunk(ctype: bytes, data: bytes) -> bytes:
        c = ctype + data
        return (
            struct.pack(">I", len(data))
            + c
            + struct.pack(">I", zlib.crc32(c) & 0xFFFFFFFF)
        )

    ihdr = struct.pack(">IIBBBBB", target_w, target_h, 8, 0, 0, 0, 0)
    compressed = zlib.compress(raw, 9)

    return (
        b"\x89PNG\r\n\x1a\n"
        + _chunk(b"IHDR", ihdr)
        + _chunk(b"IDAT", compressed)
        + _chunk(b"IEND", b"")
    )


# ---------------------------------------------------------------------------
# Mesh slicing pipeline
# ---------------------------------------------------------------------------


def _slice_mesh_to_cross_sections(
    triangles: List[
        Tuple[
            Tuple[float, float, float],
            Tuple[float, float, float],
            Tuple[float, float, float],
        ]
    ],
    z_heights: List[float],
) -> List[List[List[Tuple[float, float]]]]:
    """Slice triangles at each z-height and return polygon cross-sections."""
    all_sections: List[List[List[Tuple[float, float]]]] = []

    for z in z_heights:
        sections: List[Tuple[Tuple[float, float], Tuple[float, float]]] = []
        for tri in triangles:
            result = _triangle_cross_section(tri[0], tri[1], tri[2], z)
            if result is not None:
                sections.append(result)
        polygons = _segments_from_cross_sections(sections)
        all_sections.append(polygons)

    return all_sections


def _compute_z_heights(
    settings: ResinSettings, mesh_z_min: float, mesh_z_max: float
) -> List[float]:
    """Generate the list of z-slice heights."""
    heights: List[float] = []

    bottom_z = mesh_z_min
    for i in range(settings.bottom_layer_count):
        z = bottom_z + (i + 0.5) * settings.bottom_layer_height
        if z < mesh_z_max:
            heights.append(z)

    z = bottom_z + settings.bottom_layer_count * settings.bottom_layer_height
    while z < mesh_z_max:
        heights.append(z)
        z += settings.layer_height

    return heights


def _estimate_print_time(
    layers: List[ResinLayer],
    settings: ResinSettings,
) -> float:
    """Rough estimate of total print time in seconds."""
    total = 0.0
    for i, layer in enumerate(layers):
        total += layer.exposure_time + layer.light_off_delay
        lift_h = layer.lift_height
        lift_s = layer.lift_speed
        retract_s = layer.retract_speed
        if lift_s > 0:
            total += lift_h / lift_s
        if retract_s > 0:
            total += lift_h / retract_s
    return total


def _estimate_resin_volume(
    layers: List[ResinLayer],
    settings: ResinSettings,
) -> float:
    """Estimate total resin volume in millilitres."""
    px_area = settings.pixel_width_mm * settings.pixel_height_mm
    total_px = 0
    for layer in layers:
        total_px += layer.pixel_count
    return total_px * px_area * settings.layer_height / 1000.0


# ---------------------------------------------------------------------------
# Main slicer entry point
# ---------------------------------------------------------------------------


class ResinSlicer:
    """MSLA/DLP resin slicer engine.

    Usage::

        slicer = ResinSlicer()
        result = slicer.slice(triangles, settings)
    """

    def slice(
        self,
        triangles: List[
            Tuple[
                Tuple[float, float, float],
                Tuple[float, float, float],
                Tuple[float, float, float],
            ]
        ],
        settings: Optional[ResinSettings] = None,
    ) -> ResinSliceResult:
        """Run the full resin slicing pipeline.

        Args:
            triangles: List of triangle tuples, each vertex is (x, y, z) in mm.
            settings: Printer and material settings.

        Returns:
            ResinSliceResult with all layers, island data, and estimates.
        """
        if settings is None:
            settings = ResinSettings()

        if not triangles:
            return ResinSliceResult()

        z_min = min(min(v[2] for v in tri) for tri in triangles)
        z_max = max(max(v[2] for v in tri) for tri in triangles)

        z_heights = _compute_z_heights(settings, z_min, z_max)
        if not z_heights:
            return ResinSliceResult()

        cross_sections = _slice_mesh_to_cross_sections(triangles, z_heights)

        res_x = settings.printer_resolution_x
        res_y = settings.printer_resolution_y
        pw = settings.pixel_width_mm
        ph = settings.pixel_height_mm

        layers: List[ResinLayer] = []
        island_data: List[IslandInfo] = []
        prev_bitmap: Optional[List[List[int]]] = None

        for i, z in enumerate(z_heights):
            polys = cross_sections[i] if i < len(cross_sections) else []

            bitmap = _bitmap_from_polygons(polys, res_x, res_y, pw, ph)

            is_bottom = i < settings.bottom_layer_count
            if is_bottom:
                exp_time = settings.bottom_exposure_time
                lift_h = settings.bottom_lift_height
                lift_s = settings.lift_speed
                retract_s = settings.bottom_retract_speed
            else:
                exp_time = settings.exposure_time
                lift_h = settings.lift_height
                lift_s = settings.lift_speed
                retract_s = settings.retract_speed

            if settings.anti_aliasing and settings.anti_aliasing_level > 1:
                bitmap = _apply_anti_aliasing(
                    bitmap, settings.anti_aliasing_level, pw, ph
                )

            thumb = _encode_thumbnail_png(bitmap, 120, 120)

            layer = ResinLayer(
                z_height=z,
                bitmap=bitmap,
                exposure_time=exp_time,
                light_off_delay=settings.light_off_delay,
                lift_height=lift_h,
                lift_speed=lift_s,
                retract_speed=retract_s,
                thumbnail=thumb,
                is_bottom_layer=is_bottom,
            )
            layers.append(layer)

            island = _detect_islands(bitmap, prev_bitmap, i, pw, ph)
            island_data.append(island)
            prev_bitmap = bitmap

        cavities = _detect_cavities(cross_sections, settings)
        cavities = _suggest_drain_holes(cavities, cross_sections, settings)

        total_time = _estimate_print_time(layers, settings)
        total_volume = _estimate_resin_volume(layers, settings)

        result = ResinSliceResult(
            layers=layers,
            total_layers=len(layers),
            total_height=z_max - z_min,
            island_data=island_data,
            estimated_time=total_time,
            estimated_resin_volume=total_volume,
        )

        return result


# ---------------------------------------------------------------------------
# Convenience: serialize / deserialize
# ---------------------------------------------------------------------------


def _serialize_result(result: ResinSliceResult) -> dict:
    """Convert result to a plain dict for JSON serialization."""
    return {
        "total_layers": result.total_layers,
        "total_height": result.total_height,
        "estimated_time": result.estimated_time,
        "estimated_resin_volume": result.estimated_resin_volume,
        "layers": [
            {
                "z_height": l.z_height,
                "exposure_time": l.exposure_time,
                "light_off_delay": l.light_off_delay,
                "lift_height": l.lift_height,
                "lift_speed": l.lift_speed,
                "retract_speed": l.retract_speed,
                "pixel_count": l.pixel_count,
                "is_bottom_layer": l.is_bottom_layer,
                "resolution": (len(l.bitmap[0]), len(l.bitmap)) if l.bitmap else (0, 0),
            }
            for l in result.layers
        ],
        "island_summary": [
            {
                "layer": i.layer_index,
                "new": i.new_islands,
                "completed": i.completed_islands,
            }
            for i in result.island_data
            if i.new_islands > 0 or i.completed_islands > 0
        ],
    }


if __name__ == "__main__":
    sample_triangles = [
        ((0, 0, 0), (10, 0, 0), (5, 10, 0)),
        ((0, 0, 0), (5, 10, 0), (0, 10, 5)),
        ((10, 0, 0), (5, 10, 0), (10, 10, 5)),
        ((0, 0, 0), (10, 0, 0), (5, 0, 10)),
    ]
    s = ResinSlicer()
    r = s.slice(sample_triangles)
    print(f"Sliced {r.total_layers} layers, height {r.total_height:.2f} mm")
    print(
        f"Estimated time: {r.estimated_time:.1f}s, volume: {r.estimated_resin_volume:.2f} ml"
    )
    island_layers = [i for i in r.island_data if i.new_islands > 0]
    print(f"Islands detected on {len(island_layers)} layers")
