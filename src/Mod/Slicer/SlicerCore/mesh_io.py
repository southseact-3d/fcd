# mesh_io.py
#
# Mesh I/O module for TungstenCAD slicer workbench.
# Handles loading, importing, exporting, and basic operations on triangle meshes.
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

"""Mesh I/O utilities for loading, exporting, and manipulating triangle meshes."""

from __future__ import annotations

import math
import os
import tempfile
import uuid
from dataclasses import dataclass, field
from pathlib import Path
from typing import List, Optional, Sequence, Tuple

try:
    import FreeCAD
    import FreeCADGui
    import Mesh
    import MeshPart
except ImportError as exc:
    raise ImportError(
        "FreeCAD modules are required. This module must run inside a FreeCAD environment."
    ) from exc

SUPPORTED_IMPORT_FORMATS = {".stl", ".obj", ".3mf", ".ply", ".off", ".bms"}
TEMP_DIR_PREFIX = "tungstencad_mesh_"


# ---------------------------------------------------------------------------
# Data classes
# ---------------------------------------------------------------------------


@dataclass
class BBox:
    """Axis-aligned bounding box."""

    min_x: float
    min_y: float
    min_z: float
    max_x: float
    max_y: float
    max_z: float

    @property
    def size_x(self) -> float:
        return self.max_x - self.min_x

    @property
    def size_y(self) -> float:
        return self.max_y - self.min_y

    @property
    def size_z(self) -> float:
        return self.max_z - self.min_z

    @property
    def center(self) -> Tuple[float, float, float]:
        return (
            (self.min_x + self.max_x) / 2.0,
            (self.min_y + self.max_y) / 2.0,
            (self.min_z + self.max_z) / 2.0,
        )

    @property
    def volume(self) -> float:
        return self.size_x * self.size_y * self.size_z

    def __repr__(self) -> str:
        return (
            f"BBox(({self.min_x:.3f}, {self.min_y:.3f}, {self.min_z:.3f}) -> "
            f"({self.max_x:.3f}, {self.max_y:.3f}, {self.max_z:.3f}))"
        )


@dataclass
class MeshData:
    """Container for triangle mesh geometry.

    Attributes:
        vertices: Flat list of vertex coordinates [x0, y0, z0, x1, y1, z1, ...].
        triangles: Flat list of triangle indices [i0, i1, i2, i3, i4, i5, ...].
        normals: Per-face normals as flat list [nx0, ny0, nz0, ...]. May be empty.
        name: Optional human-readable name for the mesh.
        source_path: Original file path if imported from disk.
    """

    vertices: List[float] = field(default_factory=list)
    triangles: List[int] = field(default_factory=list)
    normals: List[float] = field(default_factory=list)
    name: str = ""
    source_path: Optional[str] = None

    # -- Derived properties ---------------------------------------------------

    @property
    def vertex_count(self) -> int:
        return len(self.vertices) // 3

    @property
    def triangle_count(self) -> int:
        return len(self.triangles) // 3

    @property
    def has_normals(self) -> bool:
        return len(self.normals) == len(self.vertices)

    def get_vertex(self, index: int) -> Tuple[float, float, float]:
        i = index * 3
        return (self.vertices[i], self.vertices[i + 1], self.vertices[i + 2])

    def get_triangle(self, index: int) -> Tuple[int, int, int]:
        i = index * 3
        return (self.triangles[i], self.triangles[i + 1], self.triangles[i + 2])

    def get_face_normal(self, index: int) -> Tuple[float, float, float]:
        i = index * 3
        return (self.normals[i], self.normals[i + 1], self.normals[i + 2])

    # -- Geometric queries ----------------------------------------------------

    def bounding_box(self) -> BBox:
        """Compute the axis-aligned bounding box."""
        if self.vertex_count == 0:
            return BBox(0, 0, 0, 0, 0, 0)

        verts = self.vertices
        min_x = min_y = min_z = float("inf")
        max_x = max_y = max_z = float("-inf")

        for i in range(0, len(verts), 3):
            x, y, z = verts[i], verts[i + 1], verts[i + 2]
            if x < min_x:
                min_x = x
            if y < min_y:
                min_y = y
            if z < min_z:
                min_z = z
            if x > max_x:
                max_x = x
            if y > max_y:
                max_y = y
            if z > max_z:
                max_z = z

        return BBox(min_x, min_y, min_z, max_x, max_y, max_z)

    def volume(self) -> float:
        """Compute the signed volume of the mesh using the divergence theorem.

        Returns the total enclosed volume (always positive for closed meshes).
        Returns 0.0 for degenerate or open meshes.
        """
        if self.triangle_count == 0:
            return 0.0

        total = 0.0
        tris = self.triangles
        verts = self.vertices

        for i in range(0, len(tris), 3):
            i0, i1, i2 = tris[i], tris[i + 1], tris[i + 2]
            v0x, v0y, v0z = verts[i0 * 3], verts[i0 * 3 + 1], verts[i0 * 3 + 2]
            v1x, v1y, v1z = verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2]
            v2x, v2y, v2z = verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2]

            # Signed volume of tetrahedron formed with the origin
            total += (
                v0x * (v1y * v2z - v2y * v1z)
                - v1x * (v0y * v2z - v2y * v0z)
                + v2x * (v0y * v1z - v1y * v0z)
            )

        return abs(total) / 6.0

    def surface_area(self) -> float:
        """Compute the total surface area of the mesh."""
        if self.triangle_count == 0:
            return 0.0

        total = 0.0
        tris = self.triangles
        verts = self.vertices

        for i in range(0, len(tris), 3):
            i0, i1, i2 = tris[i], tris[i + 1], tris[i + 2]
            v0x, v0y, v0z = verts[i0 * 3], verts[i0 * 3 + 1], verts[i0 * 3 + 2]
            v1x, v1y, v1z = verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2]
            v2x, v2y, v2z = verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2]

            # Cross product of edge vectors
            ex1, ey1, ez1 = v1x - v0x, v1y - v0y, v1z - v0z
            ex2, ey2, ez2 = v2x - v0x, v2y - v0y, v2z - v0z
            cx = ey1 * ez2 - ez1 * ey2
            cy = ez1 * ex2 - ex1 * ez2
            cz = ex1 * ey2 - ey1 * ex2

            total += math.sqrt(cx * cx + cy * cy + cz * cz)

        return total * 0.5

    # -- Normal computation ---------------------------------------------------

    def compute_normals(self) -> None:
        """Compute per-face normals and store them in self.normals."""
        normals: List[float] = []
        tris = self.triangles
        verts = self.vertices

        for i in range(0, len(tris), 3):
            i0, i1, i2 = tris[i], tris[i + 1], tris[i + 2]
            v0x, v0y, v0z = verts[i0 * 3], verts[i0 * 3 + 1], verts[i0 * 3 + 2]
            v1x, v1y, v1z = verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2]
            v2x, v2y, v2z = verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2]

            ex1, ey1, ez1 = v1x - v0x, v1y - v0y, v1z - v0z
            ex2, ey2, ez2 = v2x - v0x, v2y - v0y, v2z - v0z
            nx = ey1 * ez2 - ez1 * ey2
            ny = ez1 * ex2 - ex1 * ez2
            nz = ex1 * ey2 - ey1 * ex2

            length = math.sqrt(nx * nx + ny * ny + nz * nz)
            if length > 1e-12:
                normals.extend([nx / length, ny / length, nz / length])
            else:
                normals.extend([0.0, 0.0, 1.0])

        self.normals = normals

    # -- Mesh repair ----------------------------------------------------------

    def remove_degenerate_faces(self) -> int:
        """Remove faces with zero area or duplicate vertex indices.

        Returns:
            Number of faces removed.
        """
        if self.triangle_count == 0:
            return 0

        verts = self.vertices
        new_tris: List[int] = []
        removed = 0

        for i in range(0, len(self.triangles), 3):
            i0, i1, i2 = self.triangles[i], self.triangles[i + 1], self.triangles[i + 2]

            # Skip duplicate indices
            if i0 == i1 or i1 == i2 or i0 == i2:
                removed += 1
                continue

            v0 = (verts[i0 * 3], verts[i0 * 3 + 1], verts[i0 * 3 + 2])
            v1 = (verts[i1 * 3], verts[i1 * 3 + 1], verts[i1 * 3 + 2])
            v2 = (verts[i2 * 3], verts[i2 * 3 + 1], verts[i2 * 3 + 2])

            # Check for zero-area face
            ex1 = v1[0] - v0[0]
            ey1 = v1[1] - v0[1]
            ez1 = v1[2] - v0[2]
            ex2 = v2[0] - v0[0]
            ey2 = v2[1] - v0[1]
            ez2 = v2[2] - v0[2]
            cx = ey1 * ez2 - ez1 * ey2
            cy = ez1 * ex2 - ex1 * ez2
            cz = ex1 * ey2 - ey1 * ex2
            area_sq = cx * cx + cy * cy + cz * cz

            if area_sq < 1e-24:
                removed += 1
                continue

            new_tris.extend([i0, i1, i2])

        if removed > 0:
            self.triangles = new_tris
            self.normals = []  # Invalidate normals

        return removed

    def remove_duplicate_vertices(self, tolerance: float = 1e-6) -> int:
        """Merge vertices that are within *tolerance* distance of each other.

        Returns:
            Number of vertices removed.
        """
        if self.vertex_count == 0:
            return 0

        verts = self.vertices
        n = self.vertex_count
        mapping = list(range(n))
        used: List[bool] = [False] * n
        new_verts: List[float] = []
        new_index = 0

        for i in range(n):
            if used[i]:
                continue
            ix, iy, iz = verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]
            mapping[i] = new_index

            for j in range(i + 1, n):
                if used[j]:
                    continue
                jx, jy, jz = verts[j * 3], verts[j * 3 + 1], verts[j * 3 + 2]
                dx = ix - jx
                dy = iy - jy
                dz = iz - jz
                if dx * dx + dy * dy + dz * dz < tolerance * tolerance:
                    used[j] = True
                    mapping[j] = new_index

            new_verts.extend([ix, iy, iz])
            new_index += 1

        if new_index < n:
            self.vertices = new_verts
            self.triangles = [mapping[t] for t in self.triangles]
            self.normals = []
            return n - new_index

        return 0

    def repair(self) -> dict:
        """Run basic mesh repair and return a report dict.

        Returns:
            Dictionary with repair statistics:
                - degenerate_faces_removed
                - duplicate_vertices_removed
        """
        deg = self.remove_degenerate_faces()
        dup = self.remove_duplicate_vertices()
        return {
            "degenerate_faces_removed": deg,
            "duplicate_vertices_removed": dup,
        }

    # -- Conversion helpers ---------------------------------------------------

    def to_freecad_mesh(self) -> Mesh.Mesh:
        """Create a FreeCAD Mesh object from this data."""
        return Mesh.Mesh((self.vertices, self.triangles))

    @classmethod
    def from_freecad_mesh(cls, mesh_obj: Mesh.Mesh, name: str = "") -> MeshData:
        """Extract MeshData from a FreeCAD Mesh.Mesh object."""
        pts = mesh_obj.getPoints()
        verts: List[float] = []
        for pt in pts:
            verts.extend([pt.x, pt.y, pt.z])

        facets = mesh_obj.Facets
        tris: List[int] = []
        for facet in facets:
            idx = facet.PointIndices
            tris.extend(idx)

        return cls(vertices=verts, triangles=tris, name=name)

    @classmethod
    def from_mesh_object(cls, obj, name: str = "") -> Optional[MeshData]:
        """Create MeshData from a FreeCAD document object.

        Handles Part::Feature objects by tessellating them, and Mesh::Feature
        objects by reading their mesh directly.

        Args:
            obj: A FreeCAD document object.
            name: Optional override for the mesh name.

        Returns:
            MeshData or None if the object cannot be converted to a mesh.
        """
        if obj is None:
            return None

        obj_name = name or obj.Label

        # Direct mesh object
        if hasattr(obj, "Mesh") and isinstance(obj.Mesh, Mesh.Mesh):
            return cls.from_freecad_mesh(obj.Mesh, name=obj_name)

        # Part::Feature — tessellate
        if hasattr(obj, "Shape") and obj.Shape is not None:
            try:
                mesh = Mesh.Mesh()
                mesh.addFacets(
                    obj.Shape.tessellate(0.1)  # linear deflection
                )
                if mesh.CountFacets > 0:
                    return cls.from_freecad_mesh(mesh, name=obj_name)
            except Exception:
                return None

        return None


# ---------------------------------------------------------------------------
# Selection / document queries
# ---------------------------------------------------------------------------


def _get_active_document():
    """Return the active FreeCAD document or raise."""
    doc = FreeCAD.ActiveDocument
    if doc is None:
        raise RuntimeError("No active FreeCAD document.")
    return doc


def get_selected_meshes() -> List[MeshData]:
    """Get mesh data from all currently selected objects in the active document.

    Returns:
        List of MeshData for each selectable object that can be converted.
    """
    doc = _get_active_document()
    results: List[MeshData] = []

    if FreeCADGui is None:
        return results

    selection = FreeCADGui.Selection.getSelection()
    for sel in selection:
        obj = doc.getObject(sel.ObjectName)
        if obj is None:
            continue
        md = MeshData.from_mesh_object(obj)
        if md is not None:
            results.append(md)

    return results


def get_document_meshes() -> List[MeshData]:
    """Get mesh data from all mesh-convertible objects in the active document.

    Scans for Mesh::Feature and Part::Feature objects.

    Returns:
        List of MeshData for every convertible object.
    """
    doc = _get_active_document()
    results: List[MeshData] = []

    for obj in doc.Objects:
        md = MeshData.from_mesh_object(obj)
        if md is not None:
            results.append(md)

    return results


# ---------------------------------------------------------------------------
# File import
# ---------------------------------------------------------------------------


def _validate_import_path(filepath: str) -> Path:
    """Validate and normalise an import file path."""
    path = Path(filepath).resolve()
    if not path.exists():
        raise FileNotFoundError(f"File not found: {path}")
    if not path.is_file():
        raise ValueError(f"Not a file: {path}")
    if path.suffix.lower() not in SUPPORTED_IMPORT_FORMATS:
        raise ValueError(
            f"Unsupported format '{path.suffix}'. "
            f"Supported: {', '.join(sorted(SUPPORTED_IMPORT_FORMATS))}"
        )
    return path


def load_mesh_file(filepath: str, doc=None) -> Optional[MeshData]:
    """Import an external mesh file into a FreeCAD document.

    Args:
        filepath: Path to the mesh file (STL, OBJ, 3MF, etc.).
        doc: Optional FreeCAD document. Defaults to the active document.

    Returns:
        MeshData if import succeeded, None otherwise.
    """
    path = _validate_import_path(filepath)

    if doc is None:
        doc = _get_active_document()

    try:
        doc.openTransaction("Import mesh")
        try:
            import Mesh
            Mesh.insert(str(path), document=doc.Name)
            imported = True  # Mesh.insert returns None; object created in document
        except Exception:
            doc.abortTransaction()
            return None

        if imported is None:
            doc.abortTransaction()
            return None

        doc.commitTransaction()
        FreeCAD.ActiveDocument.recompute()

        # The imported object name is typically the filename stem
        obj_name = path.stem
        obj = doc.getObject(obj_name)
        if obj is None:
            # Fallback: scan for the most recently added mesh
            for o in reversed(doc.Objects):
                if hasattr(o, "Mesh") and isinstance(o.Mesh, Mesh.Mesh):
                    if o.Mesh.CountFacets > 0:
                        obj = o
                        break

        if obj is None:
            return None

        return MeshData.from_freecad_mesh(obj.Mesh, name=obj_name)

    except Exception:
        return None


# ---------------------------------------------------------------------------
# File export
# ---------------------------------------------------------------------------


def _temp_directory() -> Path:
    """Create or return a temporary directory for mesh exports."""
    temp_root = Path(tempfile.gettempdir())
    mesh_dir = temp_root / f"{TEMP_DIR_PREFIX}{uuid.uuid4().hex[:8]}"
    mesh_dir.mkdir(parents=True, exist_ok=True)
    return mesh_dir


def export_mesh_to_temp(
    mesh_data: MeshData,
    format: str = "stl",
    filename: Optional[str] = None,
) -> Optional[Path]:
    """Export mesh data to a temporary file for slicing.

    Args:
        mesh_data: The mesh geometry to export.
        format: Target format (extension without dot). Default is 'stl'.
        filename: Optional filename (without extension). Auto-generated if None.

    Returns:
        Path to the written temp file, or None on failure.
    """
    fmt = format.lower().lstrip(".")
    ext = f".{fmt}"
    if ext not in SUPPORTED_IMPORT_FORMATS:
        raise ValueError(
            f"Unsupported export format '{fmt}'. "
            f"Supported: {', '.join(sorted(f for f in SUPPORTED_IMPORT_FORMATS if f != '.bms'))}"
        )

    if filename is None:
        name_part = mesh_data.name or "mesh"
        safe_name = "".join(c if c.isalnum() or c in "-_" else "_" for c in name_part)
        filename = f"{safe_name}_{uuid.uuid4().hex[:6]}"

    mesh_dir = _temp_directory()
    out_path = mesh_dir / f"{filename}{ext}"

    try:
        fc_mesh = mesh_data.to_freecad_mesh()
        fc_mesh.write(str(out_path))
        return out_path
    except Exception:
        return None


def export_meshes_to_temp(
    meshes: Sequence[MeshData],
    format: str = "stl",
    prefix: str = "",
) -> List[Path]:
    """Export multiple meshes to a shared temporary directory.

    Args:
        meshes: Sequence of MeshData objects to export.
        format: Target format (extension without dot).
        prefix: Optional prefix for filenames.

    Returns:
        List of paths to the exported files. Skips meshes that fail.
    """
    results: List[Path] = []
    for idx, mesh in enumerate(meshes):
        name = f"{prefix}_{idx}" if prefix else None
        path = export_mesh_to_temp(mesh, format=format, filename=name)
        if path is not None:
            results.append(path)
    return results


# ---------------------------------------------------------------------------
# Convenience helpers
# ---------------------------------------------------------------------------


def get_mesh_from_object(obj) -> Optional[MeshData]:
    """Convenience wrapper: extract MeshData from a single FreeCAD object."""
    return MeshData.from_mesh_object(obj)


def cleanup_temp_meshes() -> int:
    """Remove temporary mesh directories created by this module.

    Returns:
        Number of directories removed.
    """
    temp_root = Path(tempfile.gettempdir())
    removed = 0

    for child in temp_root.iterdir():
        if child.is_dir() and child.name.startswith(TEMP_DIR_PREFIX):
            try:
                for f in child.iterdir():
                    f.unlink(missing_ok=True)
                child.rmdir()
                removed += 1
            except OSError:
                pass

    return removed
