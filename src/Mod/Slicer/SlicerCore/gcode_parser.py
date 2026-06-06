"""
G-code parser for preview, analysis, and visualization.

Parses G-code into structured data for layer analysis, toolpath rendering,
and print statistics estimation.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

import re
import math
from dataclasses import dataclass, field
from enum import Enum, auto
from pathlib import Path
from typing import Optional


class FeatureType(Enum):
    PERIMETER = auto()
    INNER_PERIMETER = auto()
    OUTER_PERIMETER = auto()
    INFILL = auto()
    SOLID_INFILL = auto()
    TOP_SOLID = auto()
    BOTTOM_SOLID = auto()
    SUPPORT = auto()
    SUPPORT_INTERFACE = auto()
    BRIDGE = auto()
    OVERHANG = auto()
    SKIRT = auto()
    SKIN = auto()
    TRAVEL = auto()
    WIPE = auto()
    UNKNOWN = auto()


class ColorMapMode(Enum):
    SPEED = auto()
    FEATURE = auto()
    LAYER = auto()
    FILAMENT = auto()


FEEDRATE_COLOR_MIN = 1000.0
FEEDRATE_COLOR_MAX = 12000.0

FEATURE_COLORS: dict[FeatureType, tuple[int, int, int]] = {
    FeatureType.PERIMETER: (220, 50, 50),
    FeatureType.INNER_PERIMETER: (220, 50, 50),
    FeatureType.OUTER_PERIMETER: (200, 30, 30),
    FeatureType.INFILL: (50, 100, 220),
    FeatureType.SOLID_INFILL: (40, 80, 200),
    FeatureType.TOP_SOLID: (30, 60, 180),
    FeatureType.BOTTOM_SOLID: (60, 90, 190),
    FeatureType.SUPPORT: (50, 180, 50),
    FeatureType.SUPPORT_INTERFACE: (40, 160, 40),
    FeatureType.BRIDGE: (180, 120, 50),
    FeatureType.OVERHANG: (160, 100, 40),
    FeatureType.SKIRT: (150, 150, 50),
    FeatureType.SKIN: (180, 50, 180),
    FeatureType.TRAVEL: (180, 180, 180),
    FeatureType.WIPE: (120, 120, 120),
    FeatureType.UNKNOWN: (100, 100, 100),
}

FIRST_LAYER_COLOR = (220, 200, 30)


@dataclass
class Move:
    command: str = ""
    x: Optional[float] = None
    y: Optional[float] = None
    z: Optional[float] = None
    e: Optional[float] = None
    f: Optional[int] = None
    is_extrusion: bool = False
    is_travel: bool = False
    is_retract: bool = False
    is_layer_change: bool = False
    layer_index: int = 0
    comment: Optional[str] = None
    feature_type: FeatureType = FeatureType.UNKNOWN
    position: tuple[Optional[float], Optional[float], Optional[float]] = (
        None,
        None,
        None,
    )
    distance: float = 0.0
    time: float = 0.0


@dataclass
class Layer:
    index: int = 0
    z: float = 0.0
    start_move_index: int = 0
    end_move_index: int = 0
    move_count: int = 0
    extrusion_distance: float = 0.0
    travel_distance: float = 0.0
    bounding_box: tuple[float, float, float, float, float, float] = (
        float("inf"),
        float("inf"),
        float("inf"),
        -float("inf"),
        -float("inf"),
        -float("inf"),
    )


@dataclass
class ToolpathLayer:
    vertices: list[tuple[float, float, float]] = field(default_factory=list)
    colors: list[tuple[int, int, int]] = field(default_factory=list)
    segment_types: list[FeatureType] = field(default_factory=list)


@dataclass
class FeatureLayer:
    layer_index: int = 0
    features: dict[FeatureType, list[tuple[int, int]]] = field(default_factory=dict)


@dataclass
class GcodeData:
    moves: list[Move] = field(default_factory=list)
    layers: list[Layer] = field(default_factory=list)
    print_time: float = 0.0
    filament_used: float = 0.0
    bounding_box: tuple[float, float, float, float, float, float] = (
        float("inf"),
        float("inf"),
        float("inf"),
        -float("inf"),
        -float("inf"),
        -float("inf"),
    )
    layer_count: int = 0
    total_extrusion: float = 0.0
    metadata: dict[str, str] = field(default_factory=dict)


# ---------------------------------------------------------------------------
# Comment / metadata patterns
# ---------------------------------------------------------------------------

_LAYER_COMMENT_RE = re.compile(r";\s*LAYER:\s*(\d+)")
_TYPE_COMMENT_RE = re.compile(r";\s*TYPE:\s*(.+)", re.IGNORECASE)
_TIME_COMMENT_RE = re.compile(r";\s*TIME:\s*(\d+)")
_FILAMENT_COMMENT_RE = re.compile(r";\s*Filament used:\s*([\d.]+)")
_META_KV_RE = re.compile(r";\s*(\w[\w\s]*?):\s*(.+)")


def _parse_comment_feature_type(raw: str) -> FeatureType:
    lower = raw.strip().lower()
    mapping = {
        "perimeter": FeatureType.PERIMETER,
        "inner perimeter": FeatureType.INNER_PERIMETER,
        "outer perimeter": FeatureType.OUTER_PERIMETER,
        "external perimeter": FeatureType.OUTER_PERIMETER,
        "infill": FeatureType.INFILL,
        "solid infill": FeatureType.SOLID_INFILL,
        "top solid infill": FeatureType.TOP_SOLID,
        "bottom solid infill": FeatureType.BOTTOM_SOLID,
        "top solid": FeatureType.TOP_SOLID,
        "bottom solid": FeatureType.BOTTOM_SOLID,
        "support": FeatureType.SUPPORT,
        "support material": FeatureType.SUPPORT,
        "support material interface": FeatureType.SUPPORT_INTERFACE,
        "support interface": FeatureType.SUPPORT_INTERFACE,
        "bridge": FeatureType.BRIDGE,
        "bridge infill": FeatureType.BRIDGE,
        "overhang": FeatureType.OVERHANG,
        "skirt": FeatureType.SKIRT,
        "skirt/brim": FeatureType.SKIRT,
        "skin": FeatureType.SKIN,
        "travel": FeatureType.TRAVEL,
        "move": FeatureType.TRAVEL,
        "wipe": FeatureType.WIPE,
    }
    return mapping.get(lower, FeatureType.UNKNOWN)


def _color_lerp(
    c1: tuple[int, int, int], c2: tuple[int, int, int], t: float
) -> tuple[int, int, int]:
    t = max(0.0, min(1.0, t))
    return (
        int(c1[0] + (c2[0] - c1[0]) * t),
        int(c1[1] + (c2[1] - c1[1]) * t),
        int(c1[2] + (c2[2] - c1[2]) * t),
    )


def _speed_color(feedrate: Optional[int]) -> tuple[int, int, int]:
    if feedrate is None:
        return (100, 100, 100)
    t = (feedrate - FEEDRATE_COLOR_MIN) / (FEEDRATE_COLOR_MAX - FEEDRATE_COLOR_MIN)
    return _color_lerp((0, 0, 220), (220, 30, 0), t)


def _layer_height_color(layer_z: float, max_z: float) -> tuple[int, int, int]:
    if max_z <= 0:
        return (100, 100, 220)
    t = layer_z / max_z
    return _color_lerp((50, 50, 220), (220, 50, 50), t)


def _filament_color(extrusion: float, max_ext: float) -> tuple[int, int, int]:
    if max_ext <= 0:
        return (100, 220, 100)
    t = extrusion / max_ext
    return _color_lerp((50, 180, 50), (220, 50, 50), t)


# ---------------------------------------------------------------------------
# Core parser
# ---------------------------------------------------------------------------


class GcodeParser:
    def parse(self, gcode_text: str) -> GcodeData:
        moves: list[Move] = []
        layers: list[Layer] = []
        metadata: dict[str, str] = {}
        current_layer_index = -1
        current_z: float = 0.0
        last_x: float = 0.0
        last_y: float = 0.0
        last_z: float = 0.0
        last_f: Optional[int] = None
        current_feature = FeatureType.UNKNOWN
        bbox = [float("inf")] * 3 + [-float("inf")] * 3
        total_e = 0.0

        for raw_line in gcode_text.splitlines():
            line = raw_line.split(";")[0].strip()
            comment_part = (
                raw_line.split(";", 1)[1].strip() if ";" in raw_line else None
            )

            if comment_part:
                self._process_comment(comment_part, metadata)

            if not line:
                if comment_part:
                    lm = _LAYER_COMMENT_RE.match(";" + comment_part)
                    if lm:
                        new_idx = int(lm.group(1))
                        if layers:
                            layers[-1].end_move_index = len(moves) - 1
                            layers[-1].move_count = (
                                layers[-1].end_move_index - layers[-1].start_move_index
                            )
                        current_layer_index = new_idx
                        layers.append(
                            Layer(
                                index=new_idx,
                                z=current_z,
                                start_move_index=len(moves),
                            )
                        )
                    tm = _TYPE_COMMENT_RE.match(";" + comment_part)
                    if tm:
                        current_feature = _parse_comment_feature_type(tm.group(1))
                continue

            move = self._parse_move_line(
                line,
                comment_part,
                last_x,
                last_y,
                last_z,
                last_f,
                current_layer_index,
                current_feature,
            )

            if move.command.startswith("G"):
                if move.f is not None:
                    last_f = move.f
                else:
                    move.f = last_f

                new_x = move.x if move.x is not None else last_x
                new_y = move.y if move.y is not None else last_y
                new_z = move.z if move.z is not None else last_z

                if move.z is not None and move.z != last_z:
                    move.is_layer_change = True
                    current_z = move.z

                dx = new_x - last_x
                dy = new_y - last_y
                dz = new_z - last_z
                move.distance = math.sqrt(dx * dx + dy * dy + dz * dz)

                move.e = move.e if move.e is not None else None
                if move.e is not None:
                    move.is_extrusion = move.e > 0
                    move.is_retract = move.e < 0
                    move.is_travel = False
                    total_e += move.e
                else:
                    move.is_travel = move.distance > 0.001
                    move.is_extrusion = False

                move.position = (new_x, new_y, new_z)
                last_x = new_x
                last_y = new_y
                last_z = new_z

                if move.f is not None and move.distance > 0:
                    feedrate_mm_s = move.f / 60.0
                    move.time = (
                        move.distance / feedrate_mm_s if feedrate_mm_s > 0 else 0.0
                    )

                self._update_bbox(bbox, new_x, new_y, new_z)

            moves.append(move)

        if layers:
            layers[-1].end_move_index = len(moves) - 1
            layers[-1].move_count = (
                layers[-1].end_move_index - layers[-1].start_move_index
            )

        self._compute_layer_stats(moves, layers)

        print_time = sum(m.time for m in moves if m.time > 0)

        data = GcodeData(
            moves=moves,
            layers=layers,
            print_time=print_time,
            filament_used=total_e,
            bounding_box=tuple(bbox),
            layer_count=len(layers),
            total_extrusion=total_e,
            metadata=metadata,
        )
        return data

    def parse_file(self, filepath: str | Path) -> GcodeData:
        path = Path(filepath)
        text = path.read_text(encoding="utf-8", errors="replace")
        return self.parse(text)

    def _process_comment(self, comment: str, metadata: dict[str, str]) -> None:
        tm = _TIME_COMMENT_RE.match(";" + comment)
        if tm:
            metadata.setdefault("print_time", tm.group(1))
        fm = _FILAMENT_COMMENT_RE.match(";" + comment)
        if fm:
            metadata.setdefault("filament_used", fm.group(1))
        kv = _META_KV_RE.match(";" + comment)
        if kv:
            key = kv.group(1).strip()
            value = kv.group(2).strip()
            metadata.setdefault(key, value)

    def _parse_move_line(
        self,
        line: str,
        comment: Optional[str],
        last_x: float,
        last_y: float,
        last_z: float,
        last_f: Optional[int],
        layer_index: int,
        feature_type: FeatureType,
    ) -> Move:
        tokens = line.split()
        if not tokens:
            return Move(
                comment=comment, layer_index=layer_index, feature_type=feature_type
            )

        command = tokens[0].upper()
        move = Move(
            command=command,
            comment=comment,
            layer_index=layer_index,
            feature_type=feature_type,
        )

        for token in tokens[1:]:
            if len(token) < 2:
                continue
            axis = token[0].upper()
            try:
                val = float(token[1:])
            except ValueError:
                continue
            if axis == "X":
                move.x = val
            elif axis == "Y":
                move.y = val
            elif axis == "Z":
                move.z = val
            elif axis == "E":
                move.e = val
            elif axis == "F":
                move.f = int(val)

        return move

    def _update_bbox(self, bbox: list[float], x: float, y: float, z: float) -> None:
        if x < bbox[0]:
            bbox[0] = x
        if y < bbox[1]:
            bbox[1] = y
        if z < bbox[2]:
            bbox[2] = z
        if x > bbox[3]:
            bbox[3] = x
        if y > bbox[4]:
            bbox[4] = y
        if z > bbox[5]:
            bbox[5] = z

    def _compute_layer_stats(self, moves: list[Move], layers: list[Layer]) -> None:
        for layer in layers:
            ext_dist = 0.0
            trav_dist = 0.0
            for i in range(
                layer.start_move_index, min(layer.end_move_index + 1, len(moves))
            ):
                m = moves[i]
                if m.is_extrusion:
                    ext_dist += m.distance
                elif m.is_travel:
                    trav_dist += m.distance
                if m.x is not None:
                    if m.x < layer.bounding_box[0]:
                        layer.bounding_box = (
                            m.x,
                            layer.bounding_box[1],
                            layer.bounding_box[2],
                            layer.bounding_box[3],
                            layer.bounding_box[4],
                            layer.bounding_box[5],
                        )
                    if m.x > layer.bounding_box[3]:
                        layer.bounding_box = (
                            layer.bounding_box[0],
                            layer.bounding_box[1],
                            layer.bounding_box[2],
                            m.x,
                            layer.bounding_box[4],
                            layer.bounding_box[5],
                        )
                if m.y is not None:
                    if m.y < layer.bounding_box[1]:
                        layer.bounding_box = (
                            layer.bounding_box[0],
                            m.y,
                            layer.bounding_box[2],
                            layer.bounding_box[3],
                            layer.bounding_box[4],
                            layer.bounding_box[5],
                        )
                    if m.y > layer.bounding_box[4]:
                        layer.bounding_box = (
                            layer.bounding_box[0],
                            layer.bounding_box[1],
                            layer.bounding_box[2],
                            layer.bounding_box[3],
                            m.y,
                            layer.bounding_box[5],
                        )
            layer.extrusion_distance = ext_dist
            layer.travel_distance = trav_dist


# ---------------------------------------------------------------------------
# Analyzer
# ---------------------------------------------------------------------------


class GcodeAnalyzer:
    def analyze_layer_times(self, gcode_data: GcodeData) -> list[float]:
        times: list[float] = []
        for layer in gcode_data.layers:
            t = 0.0
            for i in range(
                layer.start_move_index,
                min(layer.end_move_index + 1, len(gcode_data.moves)),
            ):
                t += gcode_data.moves[i].time
            times.append(t)
        return times

    def analyze_feature_types(self, gcode_data: GcodeData) -> list[FeatureLayer]:
        result: list[FeatureLayer] = []
        for layer in gcode_data.layers:
            fl = FeatureLayer(layer_index=layer.index)
            for i in range(
                layer.start_move_index,
                min(layer.end_move_index + 1, len(gcode_data.moves)),
            ):
                m = gcode_data.moves[i]
                ft = m.feature_type
                if ft not in fl.features:
                    fl.features[ft] = []
                fl.features[ft].append((i, i))
            result.append(fl)
        return result

    def analyze_material_usage(self, gcode_data: GcodeData) -> dict[str, float]:
        usage: dict[str, float] = {}
        for m in gcode_data.moves:
            if m.is_extrusion and m.e is not None and m.e > 0:
                key = m.feature_type.name
                usage[key] = usage.get(key, 0.0) + m.e
        return usage

    def estimate_print_time(self, gcode_data: GcodeData) -> float:
        if gcode_data.print_time > 0:
            return gcode_data.print_time
        total = 0.0
        for m in gcode_data.moves:
            total += m.time
        return total

    def get_layer_boundaries(
        self, gcode_data: GcodeData
    ) -> list[tuple[float, float, float, float, float, float]]:
        return [layer.bounding_box for layer in gcode_data.layers]

    def get_toolpath_data(
        self,
        gcode_data: GcodeData,
        color_mode: ColorMapMode = ColorMapMode.FEATURE,
    ) -> list[ToolpathLayer]:
        result: list[ToolpathLayer] = []
        max_z = (
            gcode_data.bounding_box[5]
            if gcode_data.bounding_box[5] > gcode_data.bounding_box[2]
            else 1.0
        )
        max_ext = max(
            (m.e for m in gcode_data.moves if m.e is not None and m.e > 0), default=1.0
        )
        if max_ext <= 0:
            max_ext = 1.0

        for layer in gcode_data.layers:
            tl = ToolpathLayer()
            prev_pos: Optional[tuple[float, float, float]] = None

            for i in range(
                layer.start_move_index,
                min(layer.end_move_index + 1, len(gcode_data.moves)),
            ):
                m = gcode_data.moves[i]
                pos = m.position
                if pos[0] is None or pos[1] is None or pos[2] is None:
                    continue
                p = (pos[0], pos[1], pos[2])

                if prev_pos is not None and (prev_pos != p):
                    tl.vertices.append(prev_pos)
                    tl.vertices.append(p)
                    color = self._compute_color(
                        m, color_mode, max_z, max_ext, layer.index
                    )
                    tl.colors.append(color)
                    tl.colors.append(color)
                    tl.segment_types.append(m.feature_type)

                prev_pos = p

            result.append(tl)
        return result

    def _compute_color(
        self,
        move: Move,
        mode: ColorMapMode,
        max_z: float,
        max_ext: float,
        layer_index: int,
    ) -> tuple[int, int, int]:
        if mode == ColorMapMode.SPEED:
            return _speed_color(move.f)
        elif mode == ColorMapMode.FEATURE:
            base = FEATURE_COLORS.get(move.feature_type, (100, 100, 100))
            if layer_index == 0:
                return FIRST_LAYER_COLOR
            return base
        elif mode == ColorMapMode.LAYER:
            z = move.position[2] if move.position[2] is not None else 0.0
            return _layer_height_color(z, max_z)
        elif mode == ColorMapMode.FILAMENT:
            if move.e is not None and move.e > 0:
                return _filament_color(move.e, max_ext)
            return (180, 180, 180)
        return (100, 100, 100)


# ---------------------------------------------------------------------------
# Color map helper
# ---------------------------------------------------------------------------


class GcodeColorMap:
    SPEED = ColorMapMode.SPEED
    FEATURE = ColorMapMode.FEATURE
    LAYER = ColorMapMode.LAYER
    FILAMENT = ColorMapMode.FILAMENT

    @staticmethod
    def get_color(mode: ColorMapMode, move: Move, **kwargs) -> tuple[int, int, int]:
        max_z = kwargs.get("max_z", 1.0)
        max_ext = kwargs.get("max_ext", 1.0)
        layer_index = kwargs.get("layer_index", 0)

        if mode == ColorMapMode.SPEED:
            return _speed_color(move.f)
        elif mode == ColorMapMode.FEATURE:
            base = FEATURE_COLORS.get(move.feature_type, (100, 100, 100))
            if layer_index == 0:
                return FIRST_LAYER_COLOR
            return base
        elif mode == ColorMapMode.LAYER:
            z = move.position[2] if move.position[2] is not None else 0.0
            return _layer_height_color(z, max_z)
        elif mode == ColorMapMode.FILAMENT:
            if move.e is not None and move.e > 0:
                return _filament_color(move.e, max_ext)
            return (180, 180, 180)
        return (100, 100, 100)

    @staticmethod
    def feature_color(feature: FeatureType) -> tuple[int, int, int]:
        return FEATURE_COLORS.get(feature, (100, 100, 100))


# ---------------------------------------------------------------------------
# Metadata extractor
# ---------------------------------------------------------------------------


class MetadataExtractor:
    FLAVOR_KEYS = ["FLAVOR", "firmware", "generated by"]
    TIME_KEYS = ["TIME", "PRINT_TIME", "print time"]
    FILAMENT_KEYS = ["Filament used", "FILAMENT_USED", "filament"]
    LAYER_KEYS = ["LAYER"]
    TYPE_KEYS = ["TYPE"]
    BBOX_KEYS = ["MINX", "MINY", "MINZ", "MAXX", "MAXY", "MAXZ"]

    def __init__(self, gcode_text: str) -> None:
        self.raw_text = gcode_text
        self.metadata: dict[str, str] = {}
        self._extract_all()

    def _extract_all(self) -> None:
        for raw_line in self.raw_text.splitlines():
            if not raw_line.startswith(";"):
                continue
            comment = raw_line[1:].strip()

            lm = _LAYER_COMMENT_RE.match(";" + comment)
            if lm:
                self.metadata.setdefault("layer_changes", "")
                existing = self.metadata.get("layer_changes", "")
                self.metadata["layer_changes"] = (existing + "," + lm.group(1)).lstrip(
                    ","
                )
                continue

            tm = _TYPE_COMMENT_RE.match(";" + comment)
            if tm:
                self.metadata.setdefault("feature_types", "")
                existing = self.metadata.get("feature_types", "")
                val = tm.group(1).strip()
                if val not in existing:
                    self.metadata["feature_types"] = (existing + "," + val).lstrip(",")
                continue

            kv = _META_KV_RE.match(";" + comment)
            if kv:
                key = kv.group(1).strip()
                value = kv.group(2).strip()
                self.metadata.setdefault(key, value)

    def get_flavor(self) -> Optional[str]:
        for key in self.FLAVOR_KEYS:
            if key in self.metadata:
                return self.metadata[key]
        return None

    def get_print_time(self) -> Optional[int]:
        for key in self.TIME_KEYS:
            if key in self.metadata:
                try:
                    return int(self.metadata[key])
                except ValueError:
                    pass
        return None

    def get_filament_used(self) -> Optional[float]:
        for key in self.FILAMENT_KEYS:
            if key in self.metadata:
                try:
                    return float(self.metadata[key].replace("mm", "").strip())
                except ValueError:
                    pass
        return None

    def get_layer_changes(self) -> list[int]:
        raw = self.metadata.get("layer_changes", "")
        if not raw:
            return []
        return [int(x) for x in raw.split(",") if x.strip()]

    def get_feature_types(self) -> list[str]:
        raw = self.metadata.get("feature_types", "")
        if not raw:
            return []
        return [x.strip() for x in raw.split(",") if x.strip()]

    def get_bounding_box(
        self,
    ) -> Optional[tuple[float, float, float, float, float, float]]:
        vals: list[Optional[float]] = []
        for key in self.BBOX_KEYS:
            raw = self.metadata.get(key)
            if raw is None:
                return None
            try:
                vals.append(float(raw))
            except ValueError:
                return None
        return (vals[0], vals[1], vals[2], vals[3], vals[4], vals[5])

    def get_all(self) -> dict[str, str]:
        return dict(self.metadata)
