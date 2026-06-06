# support_generator.py
#
# Support structure generator for FDM and resin printing.
# Generates pillar, tree, and grid supports with configurable parameters.
#
# Copyright (C) 2026 TungstenCAD Contributors
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation,
# Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA

"""Support structure generation for FDM and resin 3D printing."""

from __future__ import annotations

import math
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple

Vector3 = Tuple[float, float, float]


# ---------------------------------------------------------------------------
# Math helpers
# ---------------------------------------------------------------------------


def _v3_sub(a: Vector3, b: Vector3) -> Vector3:
    return (a[0] - b[0], a[1] - b[1], a[2] - b[2])


def _v3_add(a: Vector3, b: Vector3) -> Vector3:
    return (a[0] + b[0], a[1] + b[1], a[2] + b[2])


def _v3_scale(a: Vector3, s: float) -> Vector3:
    return (a[0] * s, a[1] * s, a[2] * s)


def _v3_dot(a: Vector3, b: Vector3) -> float:
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]


def _v3_cross(a: Vector3, b: Vector3) -> Vector3:
    return (
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0],
    )


def _v3_length(a: Vector3) -> float:
    return math.sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2])


def _v3_normalize(a: Vector3) -> Vector3:
    l = _v3_length(a)
    if l < 1e-12:
        return (0.0, 0.0, 1.0)
    return (a[0] / l, a[1] / l, a[2] / l)


def _v3_distance(a: Vector3, b: Vector3) -> float:
    dx, dy, dz = a[0] - b[0], a[1] - b[1], a[2] - b[2]
    return math.sqrt(dx * dx + dy * dy + dz * dz)


def _v3_lerp(a: Vector3, b: Vector3, t: float) -> Vector3:
    return (
        a[0] + (b[0] - a[0]) * t,
        a[1] + (b[1] - a[1]) * t,
        a[2] + (b[2] - a[2]) * t,
    )


def _triangle_normal(v0: Vector3, v1: Vector3, v2: Vector3) -> Vector3:
    e1 = _v3_sub(v1, v0)
    e2 = _v3_sub(v2, v0)
    n = _v3_cross(e1, e2)
    l = _v3_length(n)
    if l < 1e-12:
        return (0.0, 0.0, 1.0)
    return (n[0] / l, n[1] / l, n[2] / l)


def _z_axis() -> Vector3:
    return (0.0, 0.0, 1.0)


# ---------------------------------------------------------------------------
# Data classes
# ---------------------------------------------------------------------------


@dataclass
class OverhangRegion:
    """A single overhanging triangle face that may need support."""

    face_index: int
    centroid: Vector3
    normal: Vector3
    area: float
    z_height: float


@dataclass
class IslandData:
    """A disconnected region on a single layer that needs support."""

    layer_index: int
    z_height: float
    outline_points: List[Tuple[float, float]]
    area: float
    centroid: Tuple[float, float]


@dataclass
class SupportPoint:
    """A single support contact point for resin printing."""

    position: Vector3
    tip_radius: float = 0.3
    base_radius: float = 1.0
    height: float = 0.0
    angle: float = 0.0

    def area_2d(self) -> float:
        """Compute approximate 2D area influence of this support point."""
        return math.pi * self.base_radius * self.base_radius


@dataclass
class FDMSettings:
    """Configuration for FDM support generation."""

    support_angle: float = 45.0
    support_density: float = 0.15
    support_roof_layers: int = 3
    support_base_layers: int = 3
    support_z_distance: float = 0.2
    support_xy_distance: float = 0.8
    support_pattern: str = "grid"
    tree_support: bool = False
    support_bottom_radius: float = 2.0
    support_max_bridge_length: float = 10.0
    support_line_width: float = 0.4
    layer_height: float = 0.2


@dataclass
class ResinSettings:
    """Configuration for resin support generation."""

    support_point_density: float = 0.5
    support_point_radius: float = 0.3
    support_roof_thickness: float = 1.0
    support_base_thickness: float = 2.0
    support_angle: float = 45.0
    support_tip_diameter: float = 0.6
    support_base_diameter: float = 2.0
    support_bottom_length: float = 5.0
    support_z_distance: float = 0.1
    layer_height: float = 0.05


@dataclass
class FDMSupportResult:
    """Result container for FDM support generation."""

    support_vertices: List[float] = field(default_factory=list)
    support_triangles: List[int] = field(default_factory=list)
    support_settings: Optional[FDMSettings] = None
    estimated_material: float = 0.0
    pillar_positions: List[Tuple[float, float]] = field(default_factory=list)
    roof_regions: List[List[Tuple[float, float]]] = field(default_factory=list)


@dataclass
class ResinSupportResult:
    """Result container for resin support generation."""

    support_points: List[SupportPoint] = field(default_factory=list)
    support_vertices: List[float] = field(default_factory=list)
    support_triangles: List[int] = field(default_factory=list)
    pillar_data: List[Dict[str, float]] = field(default_factory=list)


# ---------------------------------------------------------------------------
# OverhangDetector
# ---------------------------------------------------------------------------


class OverhangDetector:
    """Shared overhang and island detection logic for FDM and resin."""

    def detect_overhangs(
        self,
        mesh_data,
        angle_threshold: float = 45.0,
    ) -> List[OverhangRegion]:
        """Find all faces whose normal angle from vertical Z exceeds the threshold.

        Args:
            mesh_data: A MeshData instance from mesh_io.py.
            angle_threshold: Maximum angle from vertical (Z-up) before a face
                is considered an overhang. Default 45 degrees.

        Returns:
            List of OverhangRegion for each overhanging face.
        """
        if not mesh_data.triangles or not mesh_data.vertices:
            return []

        threshold_cos = math.cos(math.radians(angle_threshold))
        regions: List[OverhangRegion] = []
        verts = mesh_data.vertices
        tris = mesh_data.triangles

        for i in range(0, len(tris), 3):
            i0, i1, i2 = tris[i], tris[i + 1], tris[i + 2]
            if (
                i0 * 3 + 2 >= len(verts)
                or i1 * 3 + 2 >= len(verts)
                or i2 * 3 + 2 >= len(verts)
            ):
                continue

            v0 = (verts[i0 * 3], verts[i0 * 3 + 1], verts[i0 * 3 + 2])
            v1 = (verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2])
            v2 = (verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2])

            normal = _triangle_normal(v0, v1, v2)
            nz = normal[2]

            if nz < -abs(threshold_cos):
                centroid = (
                    (v0[0] + v1[0] + v2[0]) / 3.0,
                    (v0[1] + v1[1] + v2[1]) / 3.0,
                    (v0[2] + v1[2] + v2[2]) / 3.0,
                )
                area = _v3_length(_v3_cross(_v3_sub(v1, v0), _v3_sub(v2, v0))) * 0.5
                face_idx = i // 3
                regions.append(
                    OverhangRegion(
                        face_index=face_idx,
                        centroid=centroid,
                        normal=normal,
                        area=area,
                        z_height=centroid[2],
                    )
                )

        return regions

    def detect_islands(
        self,
        cross_sections: List[List[Tuple[float, float]]],
        z_heights: List[float],
        layer_height: float,
    ) -> List[IslandData]:
        """Detect new regions per layer that appear as disconnected islands.

        For resin printing, each new island that has no support below it needs
        support points placed on it.

        Args:
            cross_sections: Per-layer list of polygon outlines (list of (x,y) loops).
            z_heights: Z height for each layer.
            layer_height: Height of each layer slice.

        Returns:
            List of IslandData for unsupported islands.
        """
        islands: List[IslandData] = []
        if not cross_sections:
            return islands

        prev_centroids: List[Tuple[float, float]] = []

        for layer_idx, polys in enumerate(cross_sections):
            z = z_heights[layer_idx] if layer_idx < len(z_heights) else 0.0
            current_centroids: List[Tuple[float, float]] = []

            for poly in polys:
                if len(poly) < 3:
                    continue

                cx = sum(p[0] for p in poly) / len(poly)
                cy = sum(p[1] for p in poly) / len(poly)
                area = (
                    abs(
                        sum(
                            poly[i][0] * poly[(i + 1) % len(poly)][1]
                            - poly[(i + 1) % len(poly)][0] * poly[i][1]
                            for i in range(len(poly))
                        )
                    )
                    * 0.5
                )
                current_centroids.append((cx, cy))

                supported = False
                for pcx, pcy in prev_centroids:
                    if math.sqrt((cx - pcx) ** 2 + (cy - pcy) ** 2) < 5.0:
                        supported = True
                        break

                if not supported and area > 0.1:
                    islands.append(
                        IslandData(
                            layer_index=layer_idx,
                            z_height=z,
                            outline_points=poly,
                            area=area,
                            centroid=(cx, cy),
                        )
                    )

            prev_centroids = current_centroids

        return islands

    def compute_gravity_map(
        self,
        mesh_data,
        resolution: float = 1.0,
    ) -> Dict[Tuple[int, int, int], bool]:
        """Voxel grid showing which voxels need support beneath the model.

        Casts rays downward from each voxel to check if there is model
        geometry above it that needs supporting.

        Args:
            mesh_data: A MeshData instance from mesh_io.py.
            resolution: Voxel edge length in mm.

        Returns:
            Dict mapping (ix, iy, iz) voxel coords to True if the voxel
            needs support (model geometry above, no model geometry below).
        """
        if not mesh_data.vertices or not mesh_data.triangles:
            return {}

        bbox_min = [float("inf")] * 3
        bbox_max = [float("-inf")] * 3
        verts = mesh_data.vertices
        for i in range(0, len(verts), 3):
            for j in range(3):
                if verts[i + j] < bbox_min[j]:
                    bbox_min[j] = verts[i + j]
                if verts[i + j] > bbox_max[j]:
                    bbox_max[j] = verts[i + j]

        nx = max(1, int(math.ceil((bbox_max[0] - bbox_min[0]) / resolution)))
        ny = max(1, int(math.ceil((bbox_max[1] - bbox_min[1]) / resolution)))
        nz = max(1, int(math.ceil((bbox_max[2] - bbox_min[2]) / resolution)))

        occupied: set = set()
        tris = mesh_data.triangles
        for i in range(0, len(tris), 3):
            i0, i1, i2 = tris[i], tris[i + 1], tris[i + 2]
            v0 = (verts[i0 * 3], verts[i0 * 3 + 1], verts[i0 * 3 + 2])
            v1 = (verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2])
            v2 = (verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2])
            tri_min = [min(v0[j], v1[j], v2[j]) for j in range(3)]
            tri_max = [max(v0[j], v1[j], v2[j]) for j in range(3)]

            ix_lo = max(0, int((tri_min[0] - bbox_min[0]) / resolution))
            ix_hi = min(nx - 1, int((tri_max[0] - bbox_min[0]) / resolution))
            iy_lo = max(0, int((tri_min[1] - bbox_min[1]) / resolution))
            iy_hi = min(ny - 1, int((tri_max[1] - bbox_min[1]) / resolution))
            iz_lo = max(0, int((tri_min[2] - bbox_min[2]) / resolution))
            iz_hi = min(nz - 1, int((tri_max[2] - bbox_min[2]) / resolution))

            for ix in range(ix_lo, ix_hi + 1):
                for iy in range(iy_lo, iy_hi + 1):
                    for iz in range(iz_lo, iz_hi + 1):
                        occupied.add((ix, iy, iz))

        gravity_map: Dict[Tuple[int, int, int], bool] = {}
        for ix in range(nx):
            for iy in range(ny):
                has_model_above = False
                for iz in range(nz - 1, -1, -1):
                    if (ix, iy, iz) in occupied:
                        has_model_above = True
                    elif has_model_above:
                        gravity_map[(ix, iy, iz)] = True

        return gravity_map


# ---------------------------------------------------------------------------
# FDM SupportGenerator
# ---------------------------------------------------------------------------


class FDMGenerator:
    """Generates FDM support structures (pillar, tree, roof, base)."""

    def __init__(self, settings: Optional[FDMSettings] = None):
        self.settings = settings or FDMSettings()
        self.detector = OverhangDetector()

    def generate(
        self, mesh_data, settings: Optional[FDMSettings] = None
    ) -> FDMSupportResult:
        """Generate complete FDM support structures.

        Args:
            mesh_data: A MeshData instance from mesh_io.py.
            settings: Override settings. Uses defaults if None.

        Returns:
            FDMSupportResult with support geometry and metadata.
        """
        if settings is not None:
            self.settings = settings

        result = FDMSupportResult(support_settings=self.settings)
        if not mesh_data.vertices or not mesh_data.triangles:
            return result

        overhangs = self.detect_overhangs(mesh_data, self.settings.support_angle)
        if not overhangs:
            return result

        if self.settings.tree_support:
            verts, tris = self.generate_tree_support(
                overhangs, mesh_data, self.settings
            )
        else:
            verts, tris = self.generate_pillar_support(
                overhangs, mesh_data, self.settings
            )

        roof_verts, roof_tris = self.generate_support_roof(overhangs, self.settings)
        verts.extend(roof_verts)
        tris.extend(t + len(verts) // 3 - len(roof_verts) // 3 for t in roof_tris)

        base_verts, base_tris = self.generate_support_base(overhangs, self.settings)
        verts.extend(base_verts)
        tris.extend(t + len(verts) // 3 - len(base_verts) // 3 for t in base_tris)

        result.support_vertices = verts
        result.support_triangles = tris
        result.estimated_material = self._estimate_material(verts, tris)
        result.pillar_positions = [(o.centroid[0], o.centroid[1]) for o in overhangs]
        return result

    def detect_overhangs(
        self, mesh_data, angle_threshold: float = 45.0
    ) -> List[OverhangRegion]:
        """Find all faces with normal angle exceeding the threshold from vertical."""
        return self.detector.detect_overhangs(mesh_data, angle_threshold)

    def generate_pillar_support(
        self,
        overhang_regions: List[OverhangRegion],
        mesh_data,
        settings: FDMSettings,
    ) -> Tuple[List[float], List[int]]:
        """Generate simple pillar/line support from each overhang down to the build plate.

        Creates vertical rectangular pillars beneath each overhang centroid,
        extending from the overhang height down to Z=0.

        Args:
            overhang_regions: Detected overhanging faces.
            mesh_data: Original model mesh for bounds and collision.
            settings: FDM support settings.

        Returns:
            Tuple of (vertices, triangles) as flat lists.
        """
        verts: List[float] = []
        tris: List[int] = []

        spacing = max(settings.support_line_width * 2.0, 2.0)
        half = settings.support_bottom_radius * 0.5
        z_gap = settings.support_z_distance

        pillar_map: Dict[Tuple[float, float], float] = {}
        for region in overhang_regions:
            gx = round(region.centroid[0] / spacing) * spacing
            gy = round(region.centroid[1] / spacing) * spacing
            key = (gx, gy)
            top_z = region.z_height - z_gap
            if key not in pillar_map or top_z > pillar_map[key]:
                pillar_map[key] = top_z

        for (px, py), top_z in pillar_map.items():
            if top_z <= 0.0:
                continue
            base_z = 0.0
            v0 = (px - half, py - half, base_z)
            v1 = (px + half, py - half, base_z)
            v2 = (px + half, py + half, base_z)
            v3 = (px - half, py + half, base_z)
            v4 = (px - half, py - half, top_z)
            v5 = (px + half, py - half, top_z)
            v6 = (px + half, py + half, top_z)
            v7 = (px - half, py + half, top_z)

            base_idx = len(verts) // 3
            for v in (v0, v1, v2, v3, v4, v5, v6, v7):
                verts.extend([v[0], v[1], v[2]])

            faces = [
                (0, 1, 2),
                (0, 2, 3),
                (4, 6, 5),
                (4, 7, 6),
                (0, 4, 5),
                (0, 5, 1),
                (2, 6, 7),
                (2, 7, 3),
                (0, 3, 7),
                (0, 7, 4),
                (1, 5, 6),
                (1, 6, 2),
            ]
            for a, b, c in faces:
                tris.extend([base_idx + a, base_idx + b, base_idx + c])

        return verts, tris

    def generate_tree_support(
        self,
        overhang_regions: List[OverhangRegion],
        mesh_data,
        settings: FDMSettings,
    ) -> Tuple[List[float], List[int]]:
        """Generate organic tree-like support with branching toward the build plate.

        Each tree trunk starts at an overhang centroid and grows downward,
        angling toward the nearest mesh surface or the build plate. Branches
        split when overhang regions are close enough to share a trunk.

        Args:
            overhang_regions: Detected overhanging faces.
            mesh_data: Original model mesh.
            settings: FDM support settings.

        Returns:
            Tuple of (vertices, triangles) as flat lists.
        """
        verts: List[float] = []
        tris: List[int] = []

        if not overhang_regions:
            return verts, tris

        z_gap = settings.support_z_distance
        half = settings.support_line_width * 0.5
        branch_radius = settings.support_bottom_radius * 0.3
        tip_radius = settings.support_line_width * 0.6

        clusters = self._cluster_overhangs(overhang_regions, 8.0)

        for cluster in clusters:
            trunk_pts = self._compute_tree_trunk(cluster, mesh_data, settings)
            trunk_pts[0] = (
                trunk_pts[0][0],
                trunk_pts[0][1],
                max(0.0, trunk_pts[0][2] - z_gap),
            )
            trunk_pts[-1] = (
                trunk_pts[-1][0],
                trunk_pts[-1][1],
                0.0,
            )

            for i in range(len(trunk_pts) - 1):
                p0 = trunk_pts[i]
                p1 = trunk_pts[i + 1]
                t_frac = i / max(1, len(trunk_pts) - 2)
                r0 = tip_radius + (branch_radius - tip_radius) * t_frac
                r1 = tip_radius + (branch_radius - tip_radius) * min(
                    1.0, t_frac + 1.0 / max(1, len(trunk_pts) - 1)
                )

                seg_verts, seg_tris = self._cylinder_segment(p0, p1, r0, r1, 6)
                idx_offset = len(verts) // 3
                verts.extend(seg_verts)
                tris.extend(t + idx_offset for t in seg_tris)

        return verts, tris

    def generate_support_roof(
        self,
        support_regions: List[OverhangRegion],
        settings: FDMSettings,
    ) -> Tuple[List[float], List[int]]:
        """Generate denser interface layer between support and model.

        The roof is a thin solid layer printed at higher density to provide
        a clean interface with the model surface.

        Args:
            support_regions: Regions needing roof support.
            settings: FDM support settings.

        Returns:
            Tuple of (vertices, triangles) as flat lists.
        """
        verts: List[float] = []
        tris: List[int] = []
        roof_height = settings.support_roof_layers * settings.layer_height
        half_width = settings.support_line_width * 1.5

        for region in support_regions:
            cx, cy, cz = region.centroid
            base_z = max(0.0, cz - settings.support_z_distance - roof_height)
            top_z = max(0.0, cz - settings.support_z_distance)
            if top_z <= base_z:
                continue

            ex = math.sqrt(region.area) * 0.5
            ey = ex

            v_positions = [
                (cx - ex, cy - ey, base_z),
                (cx + ex, cy - ey, base_z),
                (cx + ex, cy + ey, base_z),
                (cx - ex, cy + ey, base_z),
                (cx - ex, cy - ey, top_z),
                (cx + ex, cy - ey, top_z),
                (cx + ex, cy + ey, top_z),
                (cx - ex, cy + ey, top_z),
            ]

            base_idx = len(verts) // 3
            for v in v_positions:
                verts.extend([v[0], v[1], v[2]])

            roof_faces = [
                (0, 1, 2),
                (0, 2, 3),
                (4, 6, 5),
                (4, 7, 6),
                (0, 4, 5),
                (0, 5, 1),
                (2, 6, 7),
                (2, 7, 3),
            ]
            for a, b, c in roof_faces:
                tris.extend([base_idx + a, base_idx + b, base_idx + c])

        return verts, tris

    def generate_support_base(
        self,
        support_regions: List[OverhangRegion],
        settings: FDMSettings,
    ) -> Tuple[List[float], List[int]]:
        """Generate wider base pads for support pillar stability.

        Creates flared base pads at the build plate to prevent support
        pillars from tipping during printing.

        Args:
            support_regions: Regions needing support.
            settings: FDM support settings.

        Returns:
            Tuple of (vertices, triangles) as flat lists.
        """
        verts: List[float] = []
        tris: List[int] = []
        base_height = settings.support_base_layers * settings.layer_height
        base_radius = settings.support_bottom_radius

        if base_height <= 0.0 or base_radius <= 0.0:
            return verts, tris

        seen_positions: set = set()
        spacing = max(settings.support_line_width * 2.0, 2.0)

        for region in support_regions:
            gx = round(region.centroid[0] / spacing) * spacing
            gy = round(region.centroid[1] / spacing) * spacing
            key = (round(gx, 2), round(gy, 2))
            if key in seen_positions:
                continue
            seen_positions.add(key)

            segments = 8
            for s in range(segments):
                a0 = 2.0 * math.pi * s / segments
                a1 = 2.0 * math.pi * (s + 1) / segments

                inner_r = settings.support_line_width
                outer_r = base_radius

                ix0 = gx + inner_r * math.cos(a0)
                iy0 = gy + inner_r * math.sin(a0)
                ix1 = gx + inner_r * math.cos(a1)
                iy1 = gy + inner_r * math.sin(a1)

                ox0 = gx + outer_r * math.cos(a0)
                oy0 = gy + outer_r * math.sin(a0)
                ox1 = gx + outer_r * math.cos(a1)
                oy1 = gy + outer_r * math.sin(a1)

                base_idx = len(verts) // 3
                verts.extend(
                    [
                        ix0,
                        iy0,
                        0.0,
                        ix1,
                        iy1,
                        0.0,
                        ox1,
                        oy1,
                        0.0,
                        ox0,
                        oy0,
                        0.0,
                        ix0,
                        iy0,
                        base_height,
                        ix1,
                        iy1,
                        base_height,
                    ]
                )
                tris.extend(
                    [
                        base_idx,
                        base_idx + 1,
                        base_idx + 2,
                        base_idx,
                        base_idx + 2,
                        base_idx + 3,
                        base_idx + 4,
                        base_idx + 6,
                        base_idx + 5,
                        base_idx + 4,
                        base_idx + 7,
                        base_idx + 6,
                        base_idx,
                        base_idx + 4,
                        base_idx + 5,
                        base_idx,
                        base_idx + 5,
                        base_idx + 1,
                        base_idx + 2,
                        base_idx + 6,
                        base_idx + 7,
                        base_idx + 2,
                        base_idx + 7,
                        base_idx + 3,
                    ]
                )

        return verts, tris

    # -- Internal helpers ----------------------------------------------------

    def _cluster_overhangs(
        self, regions: List[OverhangRegion], radius: float
    ) -> List[List[OverhangRegion]]:
        """Group overhang regions that are close enough to share a tree trunk."""
        clusters: List[List[OverhangRegion]] = []
        used = [False] * len(regions)

        for i, r in enumerate(regions):
            if used[i]:
                continue
            cluster = [r]
            used[i] = True
            for j in range(i + 1, len(regions)):
                if used[j]:
                    continue
                if _v3_distance(r.centroid, regions[j].centroid) < radius:
                    cluster.append(regions[j])
                    used[j] = True
            clusters.append(cluster)

        return clusters

    def _compute_tree_trunk(
        self,
        cluster: List[OverhangRegion],
        mesh_data,
        settings: FDMSettings,
    ) -> List[Vector3]:
        """Compute the trunk path for a tree support from cluster centroid to bed."""
        cx = sum(r.centroid[0] for r in cluster) / len(cluster)
        cy = sum(r.centroid[1] for r in cluster) / len(cluster)
        top_z = max(r.z_height for r in cluster)

        steps = max(3, int(top_z / 5.0))
        points: List[Vector3] = []
        x, y = cx, cy

        for step in range(steps + 1):
            t = step / max(1, steps)
            z = top_z * (1.0 - t)
            angle_offset = math.sin(t * math.pi * 2.0) * 1.5
            points.append((x + angle_offset, y + angle_offset * 0.7, z))

        return points

    def _cylinder_segment(
        self,
        p0: Vector3,
        p1: Vector3,
        r0: float,
        r1: float,
        sides: int = 6,
    ) -> Tuple[List[float], List[int]]:
        """Create a tapered cylinder segment between two points."""
        verts: List[float] = []
        tris: List[int] = []

        direction = _v3_sub(p1, p0)
        length = _v3_length(direction)
        if length < 1e-6:
            return verts, tris

        if abs(r0) < 1e-6:
            r0 = 0.01
        if abs(r1) < 1e-6:
            r1 = 0.01

        z_axis = _v3_normalize(direction)
        if abs(z_axis[2]) < 0.999:
            ref = (0.0, 0.0, 1.0)
        else:
            ref = (1.0, 0.0, 0.0)
        x_axis = _v3_normalize(_v3_cross(ref, z_axis))
        y_axis = _v3_cross(z_axis, x_axis)

        for ring_idx, (center, radius) in enumerate([(p0, r0), (p1, r1)]):
            for i in range(sides):
                angle = 2.0 * math.pi * i / sides
                dx = math.cos(angle)
                dy = math.sin(angle)
                px = center[0] + x_axis[0] * dx * radius + y_axis[0] * dy * radius
                py = center[1] + x_axis[1] * dx * radius + y_axis[1] * dy * radius
                pz = center[2] + x_axis[2] * dx * radius + y_axis[2] * dy * radius
                verts.extend([px, py, pz])

        for i in range(sides):
            n = (i + 1) % sides
            b = len(verts) // 3 - sides * 2
            tris.extend([b + i, b + n, b + sides + i])
            tris.extend([b + n, b + sides + n, b + sides + i])

        cap_center_0 = len(verts) // 3
        verts.extend([p0[0], p0[1], p0[2]])
        for i in range(sides):
            n = (i + 1) % sides
            tris.extend([cap_center_0, n, i])

        cap_center_1 = len(verts) // 3
        verts.extend([p1[0], p1[1], p1[2]])
        s = sides
        for i in range(sides):
            n = (i + 1) % sides
            tris.extend([cap_center_1, s + i, s + n])

        return verts, tris

    def _estimate_material(self, verts: List[float], tris: List[int]) -> float:
        """Estimate support material volume in cubic millimeters."""
        total = 0.0
        for i in range(0, len(tris), 3):
            i0, i1, i2 = tris[i], tris[i + 1], tris[i + 2]
            v0 = (verts[i0 * 3], verts[i0 * 3 + 1], verts[i0 * 3 + 2])
            v1 = (verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2])
            v2 = (verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2])
            cross = _v3_cross(_v3_sub(v1, v0), _v3_sub(v2, v0))
            total += abs(cross[2]) * 0.5
        return total * 0.001


# ---------------------------------------------------------------------------
# Resin SupportGenerator
# ---------------------------------------------------------------------------


class ResinGenerator:
    """Generates resin printer support structures with tip/base geometry."""

    def __init__(self, settings: Optional[ResinSettings] = None):
        self.settings = settings or ResinSettings()
        self.detector = OverhangDetector()

    def generate(
        self, mesh_data, settings: Optional[ResinSettings] = None
    ) -> ResinSupportResult:
        """Generate complete resin support structures.

        Args:
            mesh_data: A MeshData instance from mesh_io.py.
            settings: Override settings. Uses defaults if None.

        Returns:
            ResinSupportResult with support geometry and contact points.
        """
        if settings is not None:
            self.settings = settings

        result = ResinSupportResult()
        if not mesh_data.vertices or not mesh_data.triangles:
            return result

        points = self.place_support_points(mesh_data, self.settings)
        result.support_points = points

        all_verts: List[float] = []
        all_tris: List[int] = []
        pillar_data: List[Dict[str, float]] = []

        for pt in points:
            tip_verts, tip_tris = self.generate_tip(pt, self.settings)
            base_verts, base_tris = self.generate_base(pt, self.settings)

            tip_offset = len(all_verts) // 3
            all_verts.extend(tip_verts)
            all_tris.extend(t + tip_offset for t in tip_tris)

            base_offset = len(all_verts) // 3
            all_verts.extend(base_verts)
            all_tris.extend(t + base_offset for t in base_tris)

            pillar_data.append(
                {
                    "x": pt.position[0],
                    "y": pt.position[1],
                    "z": pt.position[2],
                    "height": pt.height,
                    "tip_radius": pt.tip_radius,
                    "base_radius": pt.base_radius,
                }
            )

        result.support_vertices = all_verts
        result.support_triangles = all_tris
        result.pillar_data = pillar_data
        return result

    def generate_pillar_support(
        self, mesh_data, settings: Optional[ResinSettings] = None
    ) -> Tuple[List[float], List[int]]:
        """Generate pillar supports with tip and base for each contact point.

        Args:
            mesh_data: Model mesh for geometry queries.
            settings: Resin support settings.

        Returns:
            Tuple of (vertices, triangles) as flat lists.
        """
        if settings is not None:
            self.settings = settings

        points = self.place_support_points(mesh_data, self.settings)
        verts: List[float] = []
        tris: List[int] = []

        for pt in points:
            tv, tt = self.generate_tip(pt, self.settings)
            bv, bt = self.generate_base(pt, self.settings)

            to = len(verts) // 3
            verts.extend(tv)
            tris.extend(t + to for t in tt)

            bo = len(verts) // 3
            verts.extend(bv)
            tris.extend(t + bo for t in bt)

        return verts, tris

    def generate_grid_support(
        self, mesh_data, settings: Optional[ResinSettings] = None
    ) -> Tuple[List[float], List[int]]:
        """Generate cross-hatch grid support connecting multiple pillars.

        Creates a lattice of thin walls that interconnect pillar supports
        for increased rigidity on larger models.

        Args:
            mesh_data: Model mesh for bounds.
            settings: Resin support settings.

        Returns:
            Tuple of (vertices, triangles) as flat lists.
        """
        if settings is not None:
            self.settings = settings

        pillar_verts, pillar_tris = self.generate_pillar_support(
            mesh_data, self.settings
        )
        verts = list(pillar_verts)
        tris = list(pillar_tris)

        points = self.place_support_points(mesh_data, self.settings)
        if len(points) < 2:
            return verts, tris

        grid_spacing = 5.0
        wall_half = 0.15

        positions = [(pt.position[0], pt.position[1], pt.position[2]) for pt in points]
        x_coords = sorted(
            set(round(p[0] / grid_spacing) * grid_spacing for p in positions)
        )
        y_coords = sorted(
            set(round(p[1] / grid_spacing) * grid_spacing for p in positions)
        )

        grid_height = self.settings.support_bottom_length

        for gx in x_coords:
            v, t = self._grid_wall(
                (gx, y_coords[0], 0.0),
                (gx, y_coords[-1], 0.0),
                wall_half,
                grid_height,
            )
            offset = len(verts) // 3
            verts.extend(v)
            tris.extend(ti + offset for ti in t)

        for gy in y_coords:
            v, t = self._grid_wall(
                (x_coords[0], gy, 0.0),
                (x_coords[-1], gy, 0.0),
                wall_half,
                grid_height,
            )
            offset = len(verts) // 3
            verts.extend(v)
            tris.extend(ti + offset for ti in t)

        return verts, tris

    def place_support_points(
        self, mesh_data, settings: Optional[ResinSettings] = None
    ) -> List[SupportPoint]:
        """Distribute support points on overhang surfaces.

        Uses the overhang detector to find faces exceeding the angle threshold,
        then places contact points at face centroids with spacing controlled
        by support_point_density.

        Args:
            mesh_data: Model mesh to analyze.
            settings: Resin support settings.

        Returns:
            List of SupportPoint positions with tip/base radii.
        """
        if settings is not None:
            self.settings = settings

        overhangs = self.detector.detect_overhangs(
            mesh_data, self.settings.support_angle
        )
        if not overhangs:
            return []

        min_spacing = max(1.0, 5.0 / max(self.settings.support_point_density, 0.01))

        sorted_regions = sorted(overhangs, key=lambda r: r.area, reverse=True)
        placed: List[SupportPoint] = []
        placed_positions: List[Vector3] = []

        for region in sorted_regions:
            pos = region.centroid
            too_close = False
            for pp in placed_positions:
                if _v3_distance(pos, pp) < min_spacing:
                    too_close = True
                    break
            if too_close:
                continue

            height = pos[2]
            tip_r = self.settings.support_tip_diameter * 0.5
            base_r = self.settings.support_base_diameter * 0.5
            angle = math.degrees(math.acos(min(1.0, max(-1.0, -region.normal[2]))))

            placed.append(
                SupportPoint(
                    position=pos,
                    tip_radius=tip_r,
                    base_radius=base_r,
                    height=height,
                    angle=angle,
                )
            )
            placed_positions.append(pos)

        return placed

    def generate_tip(
        self, point: SupportPoint, settings: Optional[ResinSettings] = None
    ) -> Tuple[List[float], List[int]]:
        """Generate tapered tip geometry for a support point.

        Creates a cone frustum that narrows from the support shaft diameter
        down to the contact point diameter.

        Args:
            point: Support point with position and dimensions.
            settings: Resin support settings.

        Returns:
            Tuple of (vertices, triangles) as flat lists.
        """
        if settings is not None:
            self.settings = settings

        verts: List[float] = []
        tris: List[int] = []
        sides = 8
        tip_length = 0.5

        cx, cy, cz = point.position
        bottom_z = cz
        top_z = cz + tip_length

        for i in range(sides):
            a0 = 2.0 * math.pi * i / sides
            a1 = 2.0 * math.pi * (i + 1) / sides
            br = point.base_radius
            tr = point.tip_radius

            verts.extend(
                [
                    cx + br * math.cos(a0),
                    cy + br * math.sin(a0),
                    bottom_z,
                    cx + br * math.cos(a1),
                    cy + br * math.sin(a1),
                    bottom_z,
                    cx + tr * math.cos(a1),
                    cy + tr * math.sin(a1),
                    top_z,
                    cx + tr * math.cos(a0),
                    cy + tr * math.sin(a0),
                    top_z,
                ]
            )
            b = len(verts) // 3 - 4
            tris.extend([b, b + 1, b + 2, b, b + 2, b + 3])

        cap = len(verts) // 3
        verts.extend([cx, cy, top_z])
        for i in range(sides):
            n = (i + 1) % sides
            tris.extend([cap, n, i])

        return verts, tris

    def generate_base(
        self, point: SupportPoint, settings: Optional[ResinSettings] = None
    ) -> Tuple[List[float], List[int]]:
        """Generate cylindrical or conical base for a support point.

        Creates the lower portion of the support pillar that connects
        to the build plate.

        Args:
            point: Support point with position and dimensions.
            settings: Resin support settings.

        Returns:
            Tuple of (vertices, triangles) as flat lists.
        """
        if settings is not None:
            self.settings = settings

        verts: List[float] = []
        tris: List[int] = []
        sides = 8
        base_height = max(0.1, point.height - 0.5)

        cx, cy, _ = point.position
        bottom_z = 0.0
        top_z = base_height

        br = point.base_radius * 1.3
        tr = point.base_radius

        for i in range(sides):
            a0 = 2.0 * math.pi * i / sides
            a1 = 2.0 * math.pi * (i + 1) / sides

            verts.extend(
                [
                    cx + br * math.cos(a0),
                    cy + br * math.sin(a0),
                    bottom_z,
                    cx + br * math.cos(a1),
                    cy + br * math.sin(a1),
                    bottom_z,
                    cx + tr * math.cos(a1),
                    cy + tr * math.sin(a1),
                    top_z,
                    cx + tr * math.cos(a0),
                    cy + tr * math.sin(a0),
                    top_z,
                ]
            )
            b = len(verts) // 3 - 4
            tris.extend([b, b + 1, b + 2, b, b + 2, b + 3])

        cap_bottom = len(verts) // 3
        verts.extend([cx, cy, bottom_z])
        for i in range(sides):
            n = (i + 1) % sides
            tris.extend([cap_bottom, n, i])

        return verts, tris

    def _grid_wall(
        self,
        start: Vector3,
        end: Vector3,
        half_width: float,
        height: float,
    ) -> Tuple[List[float], List[int]]:
        """Create a thin wall segment for grid support."""
        verts: List[float] = []
        tris: List[int] = []

        dx = end[0] - start[0]
        dy = end[1] - start[1]
        length = math.sqrt(dx * dx + dy * dy)
        if length < 1e-6:
            return verts, tris

        nx = -dy / length
        ny = dx / length

        corners = [
            (start[0] + nx * half_width, start[1] + ny * half_width, 0.0),
            (start[0] - nx * half_width, start[1] - ny * half_width, 0.0),
            (end[0] - nx * half_width, end[1] - ny * half_width, 0.0),
            (end[0] + nx * half_width, end[1] + ny * half_width, 0.0),
        ]

        top_corners = [(c[0], c[1], height) for c in corners]

        base_idx = len(verts) // 3
        for v in corners:
            verts.extend([v[0], v[1], v[2]])
        for v in top_corners:
            verts.extend([v[0], v[1], v[2]])

        tris.extend(
            [
                base_idx,
                base_idx + 1,
                base_idx + 2,
                base_idx,
                base_idx + 2,
                base_idx + 3,
                base_idx + 4,
                base_idx + 6,
                base_idx + 5,
                base_idx + 4,
                base_idx + 7,
                base_idx + 6,
                base_idx,
                base_idx + 4,
                base_idx + 5,
                base_idx,
                base_idx + 5,
                base_idx + 1,
                base_idx + 2,
                base_idx + 6,
                base_idx + 7,
                base_idx + 2,
                base_idx + 7,
                base_idx + 3,
            ]
        )

        return verts, tris


# ---------------------------------------------------------------------------
# ManualSupportEditor
# ---------------------------------------------------------------------------


class ManualSupportEditor:
    """Interactive support editing for adding, removing, and optimizing supports."""

    def __init__(self):
        self.support_points: List[SupportPoint] = []
        self.detector = OverhangDetector()

    def add_support_point(
        self,
        position: Vector3,
        mesh_data,
        tip_radius: float = 0.3,
        base_radius: float = 1.0,
    ) -> SupportPoint:
        """Add a manual support point at the given position.

        Args:
            position: (x, y, z) coordinate for the support contact point.
            mesh_data: Model mesh for height reference.
            tip_radius: Radius of the support tip at the contact point.
            base_radius: Radius of the support base at the build plate.

        Returns:
            The newly created SupportPoint.
        """
        height = max(0.0, position[2])
        point = SupportPoint(
            position=position,
            tip_radius=tip_radius,
            base_radius=base_radius,
            height=height,
            angle=0.0,
        )
        self.support_points.append(point)
        return point

    def remove_support_point(self, position: Vector3, radius: float = 2.0) -> int:
        """Remove support points within the given radius of position.

        Args:
            position: Center point for removal search.
            radius: Maximum distance from position to remove points.

        Returns:
            Number of points removed.
        """
        before = len(self.support_points)
        self.support_points = [
            pt
            for pt in self.support_points
            if _v3_distance(pt.position, position) > radius
        ]
        return before - len(self.support_points)

    def auto_fill_islands(
        self,
        mesh_data,
        settings: Optional[ResinSettings] = None,
    ) -> List[SupportPoint]:
        """Auto-place supports on detected islands that lack support.

        Uses the OverhangDetector to find overhanging regions, then places
        support points at uncovered locations.

        Args:
            mesh_data: Model mesh to analyze.
            settings: Resin support settings for point density and sizing.

        Returns:
            List of newly added SupportPoint instances.
        """
        if settings is None:
            settings = ResinSettings()

        detector = OverhangDetector()
        overhangs = detector.detect_overhangs(mesh_data, settings.support_angle)
        if not overhangs:
            return []

        gen = ResinGenerator(settings)
        new_points = gen.place_support_points(mesh_data, settings)

        added: List[SupportPoint] = []
        min_spacing = max(1.0, 5.0 / max(settings.support_point_density, 0.01))

        for pt in new_points:
            too_close = False
            for existing in self.support_points:
                if _v3_distance(pt.position, existing.position) < min_spacing:
                    too_close = True
                    break
            if not too_close:
                self.support_points.append(pt)
                added.append(pt)

        return added

    def optimize_supports(
        self,
        support_data: ResinSupportResult,
        settings: Optional[ResinSettings] = None,
    ) -> ResinSupportResult:
        """Reduce support material while maintaining stability.

        Removes redundant supports that are too close together, and
        prunes supports on upward-facing surfaces that do not need them.

        Args:
            support_data: Current support result to optimize.
            settings: Resin support settings.

        Returns:
            Optimized ResinSupportResult.
        """
        if settings is None:
            settings = ResinSettings()

        points = list(support_data.support_points)
        if not points:
            return support_data

        min_spacing = max(2.0, 8.0 / max(settings.support_point_density, 0.01))

        sorted_pts = sorted(points, key=lambda p: p.angle, reverse=True)
        kept: List[SupportPoint] = []

        for pt in sorted_pts:
            dominated = False
            for existing in kept:
                if _v3_distance(pt.position, existing.position) < min_spacing:
                    if pt.area_2d() <= existing.area_2d():
                        dominated = True
                        break
                    else:
                        kept = [k for k in kept if k is not existing]
                        break
            if not dominated:
                kept.append(pt)

        self.support_points = kept
        return self._rebuild_result(kept, settings)

    def _rebuild_result(
        self,
        points: List[SupportPoint],
        settings: ResinSettings,
    ) -> ResinSupportResult:
        """Rebuild a ResinSupportResult from a list of support points."""
        result = ResinSupportResult(support_points=points)
        all_verts: List[float] = []
        all_tris: List[int] = []
        pillar_data: List[Dict[str, float]] = []

        gen = ResinGenerator(settings)
        for pt in points:
            tv, tt = gen.generate_tip(pt, settings)
            bv, bt = gen.generate_base(pt, settings)

            to = len(all_verts) // 3
            all_verts.extend(tv)
            all_tris.extend(t + to for t in tt)

            bo = len(all_verts) // 3
            all_verts.extend(bv)
            all_tris.extend(t + bo for t in bt)

            pillar_data.append(
                {
                    "x": pt.position[0],
                    "y": pt.position[1],
                    "z": pt.position[2],
                    "height": pt.height,
                    "tip_radius": pt.tip_radius,
                    "base_radius": pt.base_radius,
                }
            )

        result.support_vertices = all_verts
        result.support_triangles = all_tris
        result.pillar_data = pillar_data
        return result
