"""
Mesh hollowing and cavity detection for resin 3D printing.

Provides tools to hollow solid meshes, detect internal cavities that trap resin,
and generate drain holes for proper resin drainage during printing.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

import math
from collections import deque
from dataclasses import dataclass, field
from enum import Enum
from typing import Dict, List, Optional, Set, Tuple

from SlicerCore.mesh_io import MeshData


class HollowMethod(str, Enum):
    OFFSET = "offset"
    VOXEL = "voxel"


class InfillPattern(str, Enum):
    GRID = "grid"
    GYROID = "gyroid"
    HONEYCOMB = "honeycomb"


@dataclass
class Vec3:
    x: float
    y: float
    z: float

    def __add__(self, other: Vec3) -> Vec3:
        return Vec3(self.x + other.x, self.y + other.y, self.z + other.z)

    def __sub__(self, other: Vec3) -> Vec3:
        return Vec3(self.x - other.x, self.y - other.y, self.z - other.z)

    def __mul__(self, scalar: float) -> Vec3:
        return Vec3(self.x * scalar, self.y * scalar, self.z * scalar)

    def __rmul__(self, scalar: float) -> Vec3:
        return self.__mul__(scalar)

    def dot(self, other: Vec3) -> float:
        return self.x * other.x + self.y * other.y + self.z * other.z

    def cross(self, other: Vec3) -> Vec3:
        return Vec3(
            self.y * other.z - self.z * other.y,
            self.z * other.x - self.x * other.z,
            self.x * other.y - self.y * other.x,
        )

    def length(self) -> float:
        return math.sqrt(self.x * self.x + self.y * self.y + self.z * self.z)

    def normalized(self) -> Vec3:
        ln = self.length()
        if ln < 1e-12:
            return Vec3(0.0, 0.0, 0.0)
        return Vec3(self.x / ln, self.y / ln, self.z / ln)

    def to_tuple(self) -> Tuple[float, float, float]:
        return (self.x, self.y, self.z)


@dataclass
class AABB:
    min_point: Vec3
    max_point: Vec3

    def size(self) -> Vec3:
        return self.max_point - self.min_point

    def center(self) -> Vec3:
        s = self.size()
        return Vec3(
            self.min_point.x + s.x * 0.5,
            self.min_point.y + s.y * 0.5,
            self.min_point.z + s.z * 0.5,
        )

    def contains_point(self, p: Vec3) -> bool:
        return (
            self.min_point.x <= p.x <= self.max_point.x
            and self.min_point.y <= p.y <= self.max_point.y
            and self.min_point.z <= p.z <= self.max_point.z
        )


@dataclass
class Cavity:
    center_point: Vec3
    volume: float
    bounding_box: AABB
    lowest_point: Vec3
    connected_regions: int
    voxel_indices: List[Tuple[int, int, int]] = field(default_factory=list)


@dataclass
class DrainHole:
    position: Vec3
    direction: Vec3
    radius: float = 2.0


@dataclass
class HollowResult:
    original_mesh: MeshData
    hollowed_mesh: MeshData
    cavities: List[Cavity]
    drain_holes: List[DrainHole]
    volume_saved: float
    estimated_resin_saved: float


class VoxelGrid:
    """3D voxel grid used for interior/exterior classification and cavity detection."""

    def __init__(self, mesh_data: MeshData, resolution: float) -> None:
        self.resolution = resolution
        self.bounds = self._compute_bounds(mesh_data)
        size = self.bounds.size()
        self.grid_x = max(1, int(math.ceil(size.x / resolution)))
        self.grid_y = max(1, int(math.ceil(size.y / resolution)))
        self.grid_z = max(1, int(math.ceil(size.z / resolution)))
        total = self.grid_x * self.grid_y * self.grid_z
        self.voxels: bytearray = bytearray(total)
        self.surface_mask: bytearray = bytearray(total)
        self.exterior_mask: bytearray = bytearray(total)

    def _compute_bounds(self, mesh: MeshData) -> AABB:
        if not mesh.vertices:
            return AABB(Vec3(0, 0, 0), Vec3(1, 1, 1))
        xs = [v[0] for v in mesh.vertices]
        ys = [v[1] for v in mesh.vertices]
        zs = [v[2] for v in mesh.vertices]
        pad = 0.01
        return AABB(
            min_point=Vec3(min(xs) - pad, min(ys) - pad, min(zs) - pad),
            max_point=Vec3(max(xs) + pad, max(ys) + pad, max(zs) + pad),
        )

    def _xyz_to_index(self, ix: int, iy: int, iz: int) -> int:
        return iz * self.grid_y * self.grid_x + iy * self.grid_x + ix

    def _index_to_xyz(self, idx: int) -> Tuple[int, int, int]:
        iz = idx // (self.grid_y * self.grid_x)
        rem = idx % (self.grid_y * self.grid_x)
        iy = rem // self.grid_x
        ix = rem % self.grid_x
        return ix, iy, iz

    def _set_voxel(self, buf: bytearray, ix: int, iy: int, iz: int, val: int) -> None:
        if 0 <= ix < self.grid_x and 0 <= iy < self.grid_y and 0 <= iz < self.grid_z:
            buf[self._xyz_to_index(ix, iy, iz)] = val

    def _get_voxel(self, buf: bytearray, ix: int, iy: int, iz: int) -> int:
        if 0 <= ix < self.grid_x and 0 <= iy < self.grid_y and 0 <= iz < self.grid_z:
            return buf[self._xyz_to_index(ix, iy, iz)]
        return 0

    def voxel_to_world(self, ix: int, iy: int, iz: int) -> Vec3:
        return Vec3(
            self.bounds.min_point.x + (ix + 0.5) * self.resolution,
            self.bounds.min_point.y + (iy + 0.5) * self.resolution,
            self.bounds.min_point.z + (iz + 0.5) * self.resolution,
        )

    def world_to_voxel(self, p: Vec3) -> Tuple[int, int, int]:
        ix = int((p.x - self.bounds.min_point.x) / self.resolution)
        iy = int((p.y - self.bounds.min_point.y) / self.resolution)
        iz = int((p.z - self.bounds.min_point.z) / self.resolution)
        return (
            max(0, min(ix, self.grid_x - 1)),
            max(0, min(iy, self.grid_y - 1)),
            max(0, min(iz, self.grid_z - 1)),
        )

    def mark_surface(self, mesh_data: MeshData, wall_thickness: float) -> None:
        """Rasterize triangles into the voxel grid and mark surface voxels."""
        thick_voxels = max(1, int(math.ceil(wall_thickness / self.resolution)))
        verts = mesh_data.vertices
        tris = mesh_data.triangles
        for tri in tris:
            v0 = Vec3(*verts[tri[0]])
            v1 = Vec3(*verts[tri[1]])
            v2 = Vec3(*verts[tri[2]])
            edge0 = v1 - v0
            edge1 = v2 - v0
            face_normal = edge0.cross(edge1)
            fn_len = face_normal.length()
            if fn_len < 1e-12:
                continue
            face_normal = face_normal * (1.0 / fn_len)
            tri_min = Vec3(
                min(v0.x, v1.x, v2.x),
                min(v0.y, v1.y, v2.y),
                min(v0.z, v1.z, v2.z),
            )
            tri_max = Vec3(
                max(v0.x, v1.x, v2.x),
                max(v0.y, v1.y, v2.y),
                max(v0.z, v1.z, v2.z),
            )
            ix_min = max(
                0, int((tri_min.x - self.bounds.min_point.x) / self.resolution) - 1
            )
            iy_min = max(
                0, int((tri_min.y - self.bounds.min_point.y) / self.resolution) - 1
            )
            iz_min = max(
                0, int((tri_min.z - self.bounds.min_point.z) / self.resolution) - 1
            )
            ix_max = min(
                self.grid_x - 1,
                int((tri_max.x - self.bounds.min_point.x) / self.resolution) + 1,
            )
            iy_max = min(
                self.grid_y - 1,
                int((tri_max.y - self.bounds.min_point.y) / self.resolution) + 1,
            )
            iz_max = min(
                self.grid_z - 1,
                int((tri_max.z - self.bounds.min_point.z) / self.resolution) + 1,
            )
            for iz in range(iz_min, iz_max + 1):
                for iy in range(iy_min, iy_max + 1):
                    for ix in range(ix_min, ix_max + 1):
                        wp = self.voxel_to_world(ix, iy, iz)
                        if self._point_in_triangle(wp, v0, v1, v2):
                            for dz in range(-thick_voxels, thick_voxels + 1):
                                for dy in range(-thick_voxels, thick_voxels + 1):
                                    for dx in range(-thick_voxels, thick_voxels + 1):
                                        if (
                                            dx * dx + dy * dy + dz * dz
                                            <= thick_voxels * thick_voxels
                                        ):
                                            self._set_voxel(
                                                self.voxels,
                                                ix + dx,
                                                iy + dy,
                                                iz + dz,
                                                1,
                                            )
                            self._set_voxel(self.surface_mask, ix, iy, iz, 1)

    def _point_in_triangle(self, p: Vec3, v0: Vec3, v1: Vec3, v2: Vec3) -> bool:
        e0 = v1 - v0
        e1 = v2 - v0
        d = p - v0
        dot00 = e0.dot(e0)
        dot01 = e0.dot(e1)
        dot11 = e1.dot(e1)
        dot0d = e0.dot(d)
        dot1d = e1.dot(d)
        denom = dot00 * dot11 - dot01 * dot01
        if abs(denom) < 1e-12:
            return False
        inv = 1.0 / denom
        u = (dot11 * dot0d - dot01 * dot1d) * inv
        v = (dot00 * dot1d - dot01 * dot0d) * inv
        return u >= -0.001 and v >= -0.001 and (u + v) <= 1.001

    def flood_fill_from_exterior(self) -> None:
        """Mark all voxels reachable from outside the bounding box as exterior."""
        queue: deque[Tuple[int, int, int]] = deque()
        for ix in range(self.grid_x):
            for iy in range(self.grid_y):
                for iz in [0, self.grid_z - 1]:
                    if self._get_voxel(self.voxels, ix, iy, iz) == 0:
                        if self._get_voxel(self.exterior_mask, ix, iy, iz) == 0:
                            self._set_voxel(self.exterior_mask, ix, iy, iz, 1)
                            queue.append((ix, iy, iz))
        for ix in range(self.grid_x):
            for iz in range(self.grid_z):
                for iy in [0, self.grid_y - 1]:
                    if self._get_voxel(self.voxels, ix, iy, iz) == 0:
                        if self._get_voxel(self.exterior_mask, ix, iy, iz) == 0:
                            self._set_voxel(self.exterior_mask, ix, iy, iz, 1)
                            queue.append((ix, iy, iz))
        for iy in range(self.grid_y):
            for iz in range(self.grid_z):
                for ix in [0, self.grid_x - 1]:
                    if self._get_voxel(self.voxels, ix, iy, iz) == 0:
                        if self._get_voxel(self.exterior_mask, ix, iy, iz) == 0:
                            self._set_voxel(self.exterior_mask, ix, iy, iz, 1)
                            queue.append((ix, iy, iz))

        while queue:
            cx, cy, cz = queue.popleft()
            for dx, dy, dz in [
                (1, 0, 0),
                (-1, 0, 0),
                (0, 1, 0),
                (0, -1, 0),
                (0, 0, 1),
                (0, 0, -1),
            ]:
                nx, ny, nz = cx + dx, cy + dy, cz + dz
                if (
                    0 <= nx < self.grid_x
                    and 0 <= ny < self.grid_y
                    and 0 <= nz < self.grid_z
                ):
                    if self._get_voxel(self.voxels, nx, ny, nz) == 0:
                        if self._get_voxel(self.exterior_mask, nx, ny, nz) == 0:
                            self._set_voxel(self.exterior_mask, nx, ny, nz, 1)
                            queue.append((nx, ny, nz))

    def get_interior_voxels(self) -> List[Tuple[int, int, int]]:
        """Return voxels that are inside the mesh but not on the surface."""
        result: List[Tuple[int, int, int]] = []
        total = self.grid_x * self.grid_y * self.grid_z
        for idx in range(total):
            if self.voxels[idx] == 1 and self.exterior_mask[idx] == 0:
                result.append(self._index_to_xyz(idx))
        return result

    def get_cavities(self) -> List[Cavity]:
        """Find connected components of interior voxels, each is a cavity."""
        visited: Set[int] = set()
        total = self.grid_x * self.grid_y * self.grid_z
        cavities: List[Cavity] = []

        for idx in range(total):
            if idx in visited:
                continue
            if self.voxels[idx] != 1 or self.exterior_mask[idx] != 0:
                continue
            component: List[Tuple[int, int, int]] = []
            queue: deque[int] = deque([idx])
            visited.add(idx)
            while queue:
                ci = queue.popleft()
                component.append(self._index_to_xyz(ci))
                ix, iy, iz = self._index_to_xyz(ci)
                for dx, dy, dz in [
                    (1, 0, 0),
                    (-1, 0, 0),
                    (0, 1, 0),
                    (0, -1, 0),
                    (0, 0, 1),
                    (0, 0, -1),
                ]:
                    nx, ny, nz = ix + dx, iy + dy, iz + dz
                    if (
                        0 <= nx < self.grid_x
                        and 0 <= ny < self.grid_y
                        and 0 <= nz < self.grid_z
                    ):
                        ni = self._xyz_to_index(nx, ny, nz)
                        if (
                            ni not in visited
                            and self.voxels[ni] == 1
                            and self.exterior_mask[ni] == 0
                        ):
                            visited.add(ni)
                            queue.append(ni)

            if not component:
                continue

            xs = [self.voxel_to_world(ix, iy, iz).x for ix, iy, iz in component]
            ys = [self.voxel_to_world(ix, iy, iz).y for ix, iy, iz in component]
            zs = [self.voxel_to_world(ix, iy, iz).z for ix, iy, iz in component]
            center = Vec3(sum(xs) / len(xs), sum(ys) / len(ys), sum(zs) / len(zs))
            vol = len(component) * (self.resolution**3)
            bb = AABB(
                min_point=Vec3(min(xs), min(ys), min(zs)),
                max_point=Vec3(max(xs), max(ys), max(zs)),
            )
            lowest = Vec3(
                center.x,
                center.y,
                min(zs),
            )
            cavities.append(
                Cavity(
                    center_point=center,
                    volume=vol,
                    bounding_box=bb,
                    lowest_point=lowest,
                    connected_regions=1,
                    voxel_indices=component,
                )
            )

        cavities.sort(key=lambda c: c.volume, reverse=True)
        return cavities


class MeshHollower:
    """Hollows a solid mesh to create a shell suitable for resin 3D printing."""

    def hollow(
        self, mesh_data: MeshData, settings: Optional[Dict] = None
    ) -> HollowResult:
        if settings is None:
            settings = {}
        wall_thickness = settings.get("wall_thickness", 2.0)
        infill_percentage = settings.get("infill_percentage", 0)
        method_name = settings.get("hollow_method", HollowMethod.OFFSET)
        if isinstance(method_name, str):
            method_name = HollowMethod(method_name)

        if method_name == HollowMethod.VOXEL:
            resolution = settings.get("voxel_resolution", wall_thickness / 2.0)
            hollowed = self.voxel_hollow(mesh_data, resolution, wall_thickness)
        else:
            hollowed = self.offset_hollow(mesh_data, wall_thickness)

        cavity_detector = CavityDetector()
        cavities = cavity_detector.detect_cavities(
            mesh_data, resolution=wall_thickness / 2.0
        )

        drain_gen = DrainHoleGenerator()
        drain_holes = drain_gen.generate_drain_holes(cavities, mesh_data, settings)

        original_vol = self._estimate_mesh_volume(mesh_data)
        hollowed_vol = self._estimate_mesh_volume(hollowed)
        saved_vol = original_vol - hollowed_vol
        volume_saved_pct = (
            (saved_vol / original_vol * 100.0) if original_vol > 0 else 0.0
        )
        resin_saved_ml = saved_vol * 0.001

        if infill_percentage > 0:
            hollowed = self.generate_infill_pattern(
                hollowed,
                InfillPattern(settings.get("infill_pattern", InfillPattern.GRID.value)),
                infill_percentage,
            )

        return HollowResult(
            original_mesh=mesh_data,
            hollowed_mesh=hollowed,
            cavities=cavities,
            drain_holes=drain_holes,
            volume_saved=volume_saved_pct,
            estimated_resin_saved=resin_saved_ml,
        )

    def offset_hollow(self, mesh_data: MeshData, wall_thickness: float) -> MeshData:
        """Offset each triangle inward along its normal by wall_thickness."""
        verts = list(mesh_data.vertices)
        new_verts: List[List[float]] = []
        vert_map: Dict[int, int] = {}
        new_triangles: List[List[int]] = []

        normals = self._compute_vertex_normals(mesh_data)

        for i, v in enumerate(verts):
            n = Vec3(*normals[i])
            if n.length() < 1e-12:
                n = Vec3(0.0, 1.0, 0.0)
            offset_v = Vec3(*v) - n * wall_thickness
            vert_map[i] = len(new_verts)
            new_verts.append([offset_v.x, offset_v.y, offset_v.z])

        for tri in mesh_data.triangles:
            new_tri = [vert_map[tri[0]], vert_map[tri[1]], vert_map[tri[2]]]
            new_triangles.append(new_tri)

        result = MeshData(
            vertices=new_verts,
            triangles=new_triangles,
            normals=mesh_data.normals,
        )
        if hasattr(mesh_data, "colors") and mesh_data.colors:
            result.colors = mesh_data.colors
        return result

    def voxel_hollow(
        self, mesh_data: MeshData, resolution: float, wall_thickness: float
    ) -> MeshData:
        """Voxel-based hollowing: fill volume, remove interior voxels beyond wall_thickness."""
        vg = VoxelGrid(mesh_data, resolution)
        vg.mark_surface(mesh_data, wall_thickness)
        vg.flood_fill_from_exterior()

        interior = vg.get_interior_voxels()
        interior_set: Set[Tuple[int, int, int]] = set(interior)

        wall_voxels: Set[Tuple[int, int, int]] = set()
        for ix, iy, iz in interior:
            wall_voxels.add((ix, iy, iz))

        shell_thickness_voxels = max(1, int(math.ceil(wall_thickness / resolution)))

        for ix, iy, iz in interior:
            is_shell = False
            for dx in range(-shell_thickness_voxels, shell_thickness_voxels + 1):
                for dy in range(-shell_thickness_voxels, shell_thickness_voxels + 1):
                    for dz in range(
                        -shell_thickness_voxels, shell_thickness_voxels + 1
                    ):
                        if (
                            dx * dx + dy * dy + dz * dz
                            <= shell_thickness_voxels * shell_thickness_voxels
                        ):
                            nx, ny, nz = ix + dx, iy + dy, iz + dz
                            if (nx, ny, nz) not in interior_set:
                                is_shell = True
                                break
                    if is_shell:
                        break
                if is_shell:
                    break
            if not is_shell:
                wall_voxels.discard((ix, iy, iz))

        voxels_to_keep = set()
        for ix, iy, iz in vg.get_interior_voxels():
            if (ix, iy, iz) not in wall_voxels:
                continue
            voxels_to_keep.add((ix, iy, iz))
        total = vg.grid_x * vg.grid_y * vg.grid_z
        for idx in range(total):
            if vg.voxels[idx] == 1 and vg.exterior_mask[idx] == 0:
                ix, iy, iz = vg._index_to_xyz(idx)
                if (ix, iy, iz) in voxels_to_keep:
                    pass
                else:
                    vg.voxels[idx] = 0

        new_verts, new_tris = self._marching_cubes(vg)
        if not new_tris:
            return self.offset_hollow(mesh_data, wall_thickness)

        return MeshData(
            vertices=new_verts,
            triangles=new_tris,
            normals=[],
        )

    def _marching_cubes(
        self, vg: VoxelGrid
    ) -> Tuple[List[List[float]], List[List[int]]]:
        """Simplified surface extraction from voxel grid using a naive approach."""
        verts: List[List[float]] = []
        tris: List[List[int]] = []
        vert_cache: Dict[Tuple[int, int, int, int], int] = {}

        def get_vert(ix: int, iy: int, iz: int, axis: int) -> int:
            key = (ix, iy, iz, axis)
            if key in vert_cache:
                return vert_cache[key]
            wp = vg.voxel_to_world(ix, iy, iz)
            if axis == 0:
                wp = wp + Vec3(vg.resolution * 0.5, 0, 0)
            elif axis == 1:
                wp = wp + Vec3(0, vg.resolution * 0.5, 0)
            else:
                wp = wp + Vec3(0, 0, vg.resolution * 0.5)
            idx = len(verts)
            verts.append([wp.x, wp.y, wp.z])
            vert_cache[key] = idx
            return idx

        for iz in range(vg.grid_z):
            for iy in range(vg.grid_y):
                for ix in range(vg.grid_x):
                    if vg._get_voxel(vg.voxels, ix, iy, iz) == 0:
                        continue
                    if vg._get_voxel(vg.exterior_mask, ix, iy, iz) == 1:
                        continue

                    neighbors = [
                        (ix + 1, iy, iz, 0),
                        (ix - 1, iy, iz, 0),
                        (ix, iy + 1, iz, 1),
                        (ix, iy - 1, iz, 1),
                        (ix, iy, iz + 1, 2),
                        (ix, iy, iz - 1, 2),
                    ]
                    for nx, ny, nz, axis in neighbors:
                        if (
                            vg._get_voxel(vg.voxels, nx, ny, nz) == 0
                            or vg._get_voxel(vg.exterior_mask, nx, ny, nz) == 1
                        ):
                            v0 = get_vert(ix, iy, iz, axis)
                            if axis == 0:
                                v1 = get_vert(ix, iy, iz + 1, axis)
                                v2 = get_vert(ix, iy + 1, iz, axis)
                                v3 = get_vert(ix, iy + 1, iz + 1, axis)
                            elif axis == 1:
                                v1 = get_vert(ix + 1, iy, iz, axis)
                                v2 = get_vert(ix, iy, iz + 1, axis)
                                v3 = get_vert(ix + 1, iy, iz + 1, axis)
                            else:
                                v1 = get_vert(ix + 1, iy, iz, axis)
                                v2 = get_vert(ix, iy + 1, iz, axis)
                                v3 = get_vert(ix + 1, iy + 1, iz, axis)

                            if axis in (0, 2):
                                tris.append([v0, v1, v2])
                                tris.append([v1, v3, v2])
                            else:
                                tris.append([v0, v2, v1])
                                tris.append([v1, v2, v3])

        return verts, tris

    def generate_infill_pattern(
        self, hollow_mesh: MeshData, pattern: InfillPattern, density: float
    ) -> MeshData:
        """Add internal support structures inside a hollow mesh."""
        if density <= 0:
            return hollow_mesh

        bounds = self._compute_mesh_bounds(hollow_mesh)
        spacing = max(0.5, 10.0 * (1.0 - density / 100.0))

        infill_verts: List[List[float]] = []
        infill_tris: List[List[int]] = []
        base_idx = len(hollow_mesh.vertices)

        if pattern == InfillPattern.GRID:
            infill_verts, infill_tris = self._generate_grid_infill(
                bounds, spacing, base_idx
            )
        elif pattern == InfillPattern.GYROID:
            infill_verts, infill_tris = self._generate_gyroid_infill(
                bounds, spacing, base_idx
            )
        elif pattern == InfillPattern.HONEYCOMB:
            infill_verts, infill_tris = self._generate_honeycomb_infill(
                bounds, spacing, base_idx
            )

        all_verts = list(hollow_mesh.vertices) + infill_verts
        all_tris = list(hollow_mesh.triangles) + infill_tris

        return MeshData(
            vertices=all_verts,
            triangles=all_tris,
            normals=hollow_mesh.normals,
        )

    def _generate_grid_infill(
        self, bounds: AABB, spacing: float, base_idx: int
    ) -> Tuple[List[List[float]], List[List[int]]]:
        verts: List[List[float]] = []
        tris: List[List[int]] = []
        thickness = spacing * 0.15
        sx = bounds.min_point.x
        sy = bounds.min_point.y
        sz = bounds.min_point.z
        size = bounds.size()

        x = sx
        while x <= bounds.max_point.x:
            self._add_beam(
                verts,
                tris,
                base_idx,
                Vec3(x, sy, sz),
                Vec3(x, bounds.max_point.y, sz),
                thickness,
            )
            base_idx += 8
            x += spacing

        y = sy
        while y <= bounds.max_point.y:
            self._add_beam(
                verts,
                tris,
                base_idx,
                Vec3(sx, y, sz),
                Vec3(bounds.max_point.x, y, sz),
                thickness,
            )
            base_idx += 8
            y += spacing

        z = sz
        while z <= bounds.max_point.z:
            self._add_beam(
                verts,
                tris,
                base_idx,
                Vec3(sx, sy, z),
                Vec3(sx, sy, z),
                thickness,
            )
            base_idx += 8
            z += spacing

        return verts, tris

    def _generate_gyroid_infill(
        self, bounds: AABB, spacing: float, base_idx: int
    ) -> Tuple[List[List[float]], List[List[int]]]:
        verts: List[List[float]] = []
        tris: List[List[int]] = []
        freq = math.pi * 2.0 / spacing
        step = spacing * 0.25
        cx = bounds.center().x
        cy = bounds.center().y
        cz = bounds.center().z
        radius = step * 0.2

        x = bounds.min_point.x
        while x <= bounds.max_point.x:
            y = bounds.min_point.y
            while y <= bounds.max_point.y:
                z = bounds.min_point.z
                while z <= bounds.max_point.z:
                    val = (
                        math.sin(freq * x) * math.cos(freq * y)
                        + math.sin(freq * y) * math.cos(freq * z)
                        + math.sin(freq * z) * math.cos(freq * x)
                    )
                    if abs(val) < 0.3:
                        self._add_sphere_marker(
                            verts,
                            tris,
                            base_idx,
                            Vec3(x, y, z),
                            radius,
                        )
                        base_idx += 8
                    z += step
                y += step
            x += step

        return verts, tris

    def _generate_honeycomb_infill(
        self, bounds: AABB, spacing: float, base_idx: int
    ) -> Tuple[List[List[float]], List[List[int]]]:
        verts: List[List[float]] = []
        tris: List[List[int]] = []
        thickness = spacing * 0.1
        row_height = spacing * 0.866

        z = bounds.min_point.z
        row = 0
        while z <= bounds.max_point.z:
            offset_x = (spacing * 0.5) if (row % 2) else 0.0
            x = bounds.min_point.x + offset_x
            while x <= bounds.max_point.x:
                self._add_beam(
                    verts,
                    tris,
                    base_idx,
                    Vec3(x, bounds.min_point.y, z),
                    Vec3(x, bounds.max_point.y, z),
                    thickness,
                )
                base_idx += 8
                x += spacing
            z += row_height
            row += 1

        return verts, tris

    def _add_beam(
        self,
        verts: List[List[float]],
        tris: List[List[int]],
        base: int,
        start: Vec3,
        end: Vec3,
        thickness: float,
    ) -> None:
        dir_vec = end - start
        ln = dir_vec.length()
        if ln < 1e-6:
            return
        direction = dir_vec.normalized()
        up = Vec3(0, 0, 1)
        if abs(direction.dot(up)) > 0.99:
            up = Vec3(1, 0, 0)
        right = direction.cross(up).normalized() * thickness
        fwd = direction.cross(right).normalized() * thickness

        corners = [
            start + right + fwd,
            start + right - fwd,
            start - right - fwd,
            start - right + fwd,
            end + right + fwd,
            end + right - fwd,
            end - right - fwd,
            end - right + fwd,
        ]
        for c in corners:
            verts.append([c.x, c.y, c.z])

        faces = [
            [0, 1, 5, 4],
            [2, 3, 7, 6],
            [0, 4, 7, 3],
            [1, 5, 6, 2],
            [0, 3, 2, 1],
            [4, 7, 6, 5],
        ]
        for f in faces:
            tris.append([base + f[0], base + f[1], base + f[2]])
            tris.append([base + f[0], base + f[2], base + f[3]])

    def _add_sphere_marker(
        self,
        verts: List[List[float]],
        tris: List[List[int]],
        base: int,
        center: Vec3,
        radius: float,
    ) -> None:
        n_lat, n_lon = 3, 4
        for i in range(n_lat + 1):
            lat = math.pi * i / n_lat - math.pi / 2.0
            for j in range(n_lon):
                lon = 2.0 * math.pi * j / n_lon
                x = center.x + radius * math.cos(lat) * math.cos(lon)
                y = center.y + radius * math.cos(lat) * math.sin(lon)
                z = center.z + radius * math.sin(lat)
                verts.append([x, y, z])

        for i in range(n_lat):
            for j in range(n_lon):
                p0 = base + i * n_lon + j
                p1 = base + i * n_lon + (j + 1) % n_lon
                p2 = base + (i + 1) * n_lon + j
                p3 = base + (i + 1) * n_lon + (j + 1) % n_lon
                tris.append([p0, p1, p2])
                tris.append([p1, p3, p2])

    def boolean_subtract(self, outer_mesh: MeshData, inner_mesh: MeshData) -> MeshData:
        """Simplified boolean subtraction: remove triangles from outer mesh that are inside inner mesh."""
        keep_triangles: List[List[int]] = []
        for tri in outer_mesh.triangles:
            v0 = Vec3(*outer_mesh.vertices[tri[0]])
            v1 = Vec3(*outer_mesh.vertices[tri[1]])
            v2 = Vec3(*outer_mesh.vertices[tri[2]])
            centroid = Vec3(
                (v0.x + v1.x + v2.x) / 3.0,
                (v0.y + v1.y + v2.y) / 3.0,
                (v0.z + v1.z + v2.z) / 3.0,
            )
            inside = self._point_inside_mesh(centroid, inner_mesh)
            if not inside:
                keep_triangles.append(tri)

        return MeshData(
            vertices=list(outer_mesh.vertices),
            triangles=keep_triangles,
            normals=outer_mesh.normals,
        )

    def _point_inside_mesh(self, point: Vec3, mesh: MeshData) -> bool:
        """Ray-cast test: count intersections along +x direction."""
        if not mesh.triangles:
            return False
        count = 0
        ray_dir = Vec3(1, 0, 0)
        for tri in mesh.triangles:
            v0 = Vec3(*mesh.vertices[tri[0]])
            v1 = Vec3(*mesh.vertices[tri[1]])
            v2 = Vec3(*mesh.vertices[tri[2]])
            hit = self._ray_triangle_intersect(point, ray_dir, v0, v1, v2)
            if hit is not None:
                count += 1
        return count % 2 == 1

    def _ray_triangle_intersect(
        self, origin: Vec3, direction: Vec3, v0: Vec3, v1: Vec3, v2: Vec3
    ) -> Optional[Vec3]:
        e1 = v1 - v0
        e2 = v2 - v0
        h = direction.cross(e2)
        a = e1.dot(h)
        if -1e-8 < a < 1e-8:
            return None
        f = 1.0 / a
        s = origin - v0
        u = f * s.dot(h)
        if u < 0.0 or u > 1.0:
            return None
        q = s.cross(e1)
        v = f * direction.dot(q)
        if v < 0.0 or u + v > 1.0:
            return None
        t = f * e2.dot(q)
        if t > 1e-6:
            return origin + direction * t
        return None

    def _compute_vertex_normals(self, mesh: MeshData) -> List[List[float]]:
        vert_count = len(mesh.vertices)
        normals: List[List[float]] = [[0.0, 0.0, 0.0] for _ in range(vert_count)]

        for tri in mesh.triangles:
            v0 = Vec3(*mesh.vertices[tri[0]])
            v1 = Vec3(*mesh.vertices[tri[1]])
            v2 = Vec3(*mesh.vertices[tri[2]])
            e1 = v1 - v0
            e2 = v2 - v0
            fn = e1.cross(e2)
            for vi in tri:
                normals[vi][0] += fn.x
                normals[vi][1] += fn.y
                normals[vi][2] += fn.z

        for n in normals:
            ln = math.sqrt(n[0] ** 2 + n[1] ** 2 + n[2] ** 2)
            if ln > 1e-12:
                n[0] /= ln
                n[1] /= ln
                n[2] /= ln

        return normals

    def _compute_mesh_bounds(self, mesh: MeshData) -> AABB:
        if not mesh.vertices:
            return AABB(Vec3(0, 0, 0), Vec3(1, 1, 1))
        xs = [v[0] for v in mesh.vertices]
        ys = [v[1] for v in mesh.vertices]
        zs = [v[2] for v in mesh.vertices]
        return AABB(
            min_point=Vec3(min(xs), min(ys), min(zs)),
            max_point=Vec3(max(xs), max(ys), max(zs)),
        )

    def _estimate_mesh_volume(self, mesh: MeshData) -> float:
        volume = 0.0
        for tri in mesh.triangles:
            v0 = Vec3(*mesh.vertices[tri[0]])
            v1 = Vec3(*mesh.vertices[tri[1]])
            v2 = Vec3(*mesh.vertices[tri[2]])
            volume += v0.dot(v1.cross(v2)) / 6.0
        return abs(volume)


class CavityDetector:
    """Detects internal cavities in a mesh that would trap uncured resin."""

    def detect_cavities(
        self, mesh_data: MeshData, resolution: float = 0.5
    ) -> List[Cavity]:
        vg = VoxelGrid(mesh_data, resolution)
        vg.mark_surface(mesh_data, wall_thickness=resolution)
        vg.flood_fill_from_exterior()
        cavities = vg.get_cavities()
        return cavities

    def find_trapped_regions(
        self, mesh_data: MeshData, resolution: float = 0.5
    ) -> List[Cavity]:
        all_cavities = self.detect_cavities(mesh_data, resolution)
        trapped: List[Cavity] = []
        for c in all_cavities:
            if c.volume > resolution**3 * 2:
                trapped.append(c)
        return trapped

    def has_enclosed_voids(self, mesh_data: MeshData, resolution: float = 1.0) -> bool:
        cavities = self.detect_cavities(mesh_data, resolution)
        return len(cavities) > 0


class DrainHoleGenerator:
    """Generates drain holes for resin 3D printing to allow resin drainage."""

    def generate_drain_holes(
        self,
        cavities: List[Cavity],
        mesh_data: MeshData,
        settings: Optional[Dict] = None,
    ) -> List[DrainHole]:
        if settings is None:
            settings = {}
        default_radius = settings.get("drain_hole_radius", 2.0)
        min_holes_per_cavity = settings.get("min_holes_per_cavity", 2)
        critical_regions = settings.get("critical_regions", [])
        holes: List[DrainHole] = []

        for cavity in cavities:
            if cavity.volume < 1.0:
                continue

            directions = self._compute_drain_directions(cavity, mesh_data)
            placed: List[Vec3] = []
            attempts = 0
            max_attempts = 50

            while len(placed) < min_holes_per_cavity and attempts < max_attempts:
                attempts += 1
                candidate = self._find_hole_position(
                    cavity, directions, placed, mesh_data, critical_regions
                )
                if candidate is None:
                    break

                pos, normal = candidate
                hole = DrainHole(
                    position=pos,
                    direction=normal,
                    radius=default_radius,
                )
                holes.append(hole)
                placed.append(pos)

        return holes

    def drill_holes(self, mesh_data: MeshData, holes: List[DrainHole]) -> MeshData:
        """Boolean subtract cylinders from the mesh to create drain holes."""
        if not holes:
            return mesh_data

        keep_triangles: List[List[int]] = []
        for tri in mesh_data.triangles:
            v0 = Vec3(*mesh_data.vertices[tri[0]])
            v1 = Vec3(*mesh_data.vertices[tri[1]])
            v2 = Vec3(*mesh_data.vertices[tri[2]])
            centroid = Vec3(
                (v0.x + v1.x + v2.x) / 3.0,
                (v0.y + v1.y + v2.y) / 3.0,
                (v0.z + v1.z + v2.z) / 3.0,
            )
            in_hole = False
            for hole in holes:
                if self._point_in_drill_cylinder(centroid, hole):
                    in_hole = True
                    break
            if not in_hole:
                keep_triangles.append(tri)

        return MeshData(
            vertices=list(mesh_data.vertices),
            triangles=keep_triangles,
            normals=mesh_data.normals,
        )

    def _compute_drain_directions(
        self, cavity: Cavity, mesh_data: MeshData
    ) -> List[Vec3]:
        directions: List[Vec3] = []
        directions.append(Vec3(0.0, 0.0, -1.0))
        directions.append(Vec3(0.0, 0.0, 1.0))

        center = cavity.center_point
        for v in mesh_data.vertices:
            vp = Vec3(*v)
            d = vp - center
            ln = d.length()
            if ln > 1e-6:
                directions.append(d * (1.0 / ln))

        unique: List[Vec3] = []
        for d in directions:
            is_dup = False
            for u in unique:
                if d.dot(u) > 0.95:
                    is_dup = True
                    break
            if not is_dup:
                unique.append(d)
        return unique

    def _find_hole_position(
        self,
        cavity: Cavity,
        directions: List[Vec3],
        existing: List[Vec3],
        mesh_data: MeshData,
        critical_regions: List[AABB],
    ) -> Optional[Tuple[Vec3, Vec3]]:
        if cavity.voxel_indices and len(cavity.voxel_indices) > 0:
            center = cavity.center_point
            min_dist = float("inf")
            best_pos = None
            best_normal = None

            for ix, iy, iz in cavity.voxel_indices:
                vg = VoxelGrid(mesh_data, 0.5)
                wp = vg.voxel_to_world(ix, iy, iz)
                if self._is_in_critical_region(wp, critical_regions):
                    continue
                dist_to_existing = (
                    min((wp - e).length() for e in existing)
                    if existing
                    else float("inf")
                )
                if dist_to_existing < 1.0:
                    continue

                outward = wp - center
                ln = outward.length()
                if ln < 1e-6:
                    continue
                normal = outward * (1.0 / ln)

                score = dist_to_existing
                if score < min_dist:
                    min_dist = score
                    best_pos = wp
                    best_normal = normal

            if best_pos is not None and best_normal is not None:
                return best_pos, best_normal

        return cavity.lowest_point, Vec3(0, 0, -1)

    def _is_in_critical_region(self, point: Vec3, regions: List[AABB]) -> bool:
        for r in regions:
            if r.contains_point(point):
                return True
        return False

    def _point_in_drill_cylinder(self, point: Vec3, hole: DrainHole) -> bool:
        to_point = point - hole.position
        along_axis = to_point.dot(hole.direction)
        if along_axis < 0:
            return False
        cylinder_length = 50.0
        if along_axis > cylinder_length:
            return False
        perp = to_point - hole.direction * along_axis
        return perp.length() <= hole.radius
