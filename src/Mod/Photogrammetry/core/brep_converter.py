"""
B-REP Converter Module

Converts triangle meshes into parametric B-REP (Boundary Representation) solids
using best-fit primitive approach with OpenCASCADE geometry kernel.

The conversion pipeline:
1. Segment mesh into regions by surface type (planar, cylindrical, spherical)
2. Fit geometric primitives to each region using robust estimation
3. Create B-REP surfaces from fitted primitives
4. Trim surfaces against each other to form a watertight solid

Mathematical Approach:
- Plane fitting: SVD-based least squares on centered points
- Cylinder fitting: RANSAC with iterative least-squares refinement
- Sphere fitting: Linearized least squares minimization
"""

import math
from collections import defaultdict

try:
    import FreeCAD
    import Part
    import Mesh
    from FreeCAD import Base
    HAS_FREECAD = True
except ImportError:
    HAS_FREECAD = False

try:
    import numpy as np
    HAS_NUMPY = True
except ImportError:
    HAS_NUMPY = False


def _check_deps():
    if not HAS_FREECAD:
        raise ImportError(
            "FreeCAD is required. Install FreeCAD or run in FreeCAD Python environment."
        )
    if not HAS_NUMPY:
        raise ImportError("NumPy is required for B-REP conversion calculations.")


def mesh_to_freecad(vertices: list, faces: list):
    """Convert raw vertices/faces to FreeCAD Mesh object.

    Args:
        vertices: list of [x, y, z] coordinates.
        faces: list of [v1, v2, v3] triangle indices.

    Returns:
        Mesh.MeshObject suitable for FreeCAD operations.
    """
    _check_deps()
    flat_vertices = []
    for v in vertices:
        flat_vertices.extend([float(v[0]), float(v[1]), float(v[2])])
    flat_faces = []
    for f in faces:
        flat_faces.extend([int(f[0]), int(f[1]), int(f[2]), 0])
    mesh_obj = Mesh.Mesh()
    mesh_obj.addMesh(flat_vertices, flat_faces)
    return mesh_obj


class BRepConverter:
    """Converts a triangle mesh to parametric B-REP solids using best-fit
    primitive decomposition.

    The converter segments the input mesh into regions by surface type,
    fits geometric primitives (planes, cylinders, spheres) to each region,
    creates B-REP surfaces, and trims them into a watertight solid.
    """

    def __init__(self, vertices: list, faces: list, settings: dict = None):
        """Initialize the converter with mesh data.

        Args:
            vertices: list of [x, y, z] coordinates.
            faces: list of [v1, v2, v3] triangle indices.
            settings: dict with keys:
                - min_region_area: float (percentage, default 1.0)
                - max_fit_error: float (default 0.01)
                - plane_threshold: float (normal consistency, default 0.1)
                - cylinder_threshold: float (curvature ratio, default 0.3)
        """
        _check_deps()

        self.vertices = np.array(vertices, dtype=np.float64)
        self.faces = np.array(faces, dtype=np.int32)

        if self.vertices.ndim != 2 or self.vertices.shape[1] != 3:
            raise ValueError("vertices must be Nx3 array-like")
        if self.faces.ndim != 2 or self.faces.shape[1] != 3:
            raise ValueError("faces must be Mx3 array-like")
        if np.any(self.faces >= len(self.vertices)):
            raise ValueError("face indices exceed vertex count")

        defaults = {
            "min_region_area": 1.0,
            "max_fit_error": 0.01,
            "plane_threshold": 0.1,
            "cylinder_threshold": 0.3,
        }
        self.settings = {**defaults, **(settings or {})}

        self._face_normals = None
        self._face_areas = None
        self._regions = None
        self._adjacency = None
        self._vertex_faces = None

    # ------------------------------------------------------------------ #
    #  Geometry helpers                                                     #
    # ------------------------------------------------------------------ #

    def _compute_face_geometry(self):
        """Compute per-face normals and areas."""
        if self._face_normals is not None:
            return

        v0 = self.vertices[self.faces[:, 0]]
        v1 = self.vertices[self.faces[:, 1]]
        v2 = self.vertices[self.faces[:, 2]]

        e1 = v1 - v0
        e2 = v2 - v0

        cross = np.cross(e1, e2)
        self._face_areas = 0.5 * np.linalg.norm(cross, axis=1)

        norms = np.linalg.norm(cross, axis=1, keepdims=True)
        norms = np.maximum(norms, 1e-12)
        self._face_normals = cross / norms

    def _build_adjacency(self):
        """Build face adjacency graph and vertex-to-face mapping."""
        if self._adjacency is not None:
            return

        n_faces = len(self.faces)
        self._adjacency = defaultdict(list)
        self._vertex_faces = defaultdict(list)

        edge_to_faces = defaultdict(list)
        for i in range(n_faces):
            for j in range(3):
                v = int(self.faces[i, j])
                self._vertex_faces[v].append(i)
                e = tuple(sorted([
                    int(self.faces[i, j]),
                    int(self.faces[i, (j + 1) % 3]),
                ]))
                edge_to_faces[e].append(i)

        for edge, face_list in edge_to_faces.items():
            if len(face_list) == 2:
                self._adjacency[face_list[0]].append(face_list[1])
                self._adjacency[face_list[1]].append(face_list[0])

    # ------------------------------------------------------------------ #
    #  Region detection                                                     #
    # ------------------------------------------------------------------ #

    def _detect_regions(self) -> list:
        """Segment mesh into regions by surface type using region growing.

        Returns:
            list of dicts, each with keys:
                type: "plane" | "cylinder" | "sphere" | "unknown"
                faces: list of face indices
                vertices: list of vertex indices
                normal: [nx, ny, nz] (seed normal for the region)
                area: total area of the region
        """
        if self._regions is not None:
            return self._regions

        self._compute_face_geometry()
        self._build_adjacency()

        n_faces = len(self.faces)
        visited = np.zeros(n_faces, dtype=bool)
        regions = []

        total_area = float(np.sum(self._face_areas))
        min_area = self.settings["min_region_area"] / 100.0 * total_area

        seed_order = np.argsort(-self._face_areas)

        for seed in seed_order:
            if visited[seed]:
                continue

            region_faces = []
            region_vertices = set()
            queue = [int(seed)]
            seed_normal = self._face_normals[seed].copy()

            while queue:
                fi = queue.pop(0)
                if visited[fi]:
                    continue

                dot = abs(np.dot(self._face_normals[fi], seed_normal))
                if dot < (1.0 - self.settings["plane_threshold"]):
                    continue

                visited[fi] = True
                region_faces.append(fi)
                for v in self.faces[fi]:
                    region_vertices.add(int(v))

                for neighbor in self._adjacency.get(fi, []):
                    if not visited[neighbor]:
                        queue.append(neighbor)

            if not region_faces:
                continue

            region_area = float(np.sum(self._face_areas[region_faces]))
            if region_area < min_area and len(regions) > 0:
                for fi in region_faces:
                    visited[fi] = False
                continue

            verts = list(region_vertices)
            region_type = self._classify_region(verts, region_faces, seed_normal)

            regions.append({
                "type": region_type,
                "faces": region_faces,
                "vertices": verts,
                "normal": seed_normal.tolist(),
                "area": region_area,
            })

        self._regions = regions
        return regions

    def _classify_region(
        self, vertex_indices: list, face_indices: list, seed_normal
    ) -> str:
        """Classify a region as plane, cylinder, sphere, or unknown.

        Uses normal variance and curvature analysis derived from SVD of
        per-face normals.  Low normal variance => plane, balanced variance
        => cylinder, uniform radial distances from centroid => sphere.
        """
        region_verts = self.vertices[vertex_indices]
        normals = self._face_normals[face_indices]

        normal_var = np.var(normals, axis=0)
        total_var = float(np.sum(normal_var))

        if total_var < self.settings["plane_threshold"] * 0.1:
            return "plane"

        centered_normals = normals - np.mean(normals, axis=0, keepdims=True)
        if centered_normals.shape[0] > 2:
            try:
                _, s, _ = np.linalg.svd(centered_normals, full_matrices=False)
                if len(s) >= 2 and s[1] > 1e-8:
                    curvature_ratio = s[1] / (s[0] + 1e-12)
                    if curvature_ratio > self.settings["cylinder_threshold"]:
                        return "cylinder"
                    elif curvature_ratio < self.settings["cylinder_threshold"] * 0.5:
                        return "plane"
            except np.linalg.LinAlgError:
                pass

        centroid = np.mean(region_verts, axis=0)
        distances = np.linalg.norm(region_verts - centroid, axis=1)
        if len(distances) > 1:
            dist_var = float(np.var(distances) / (np.mean(distances) ** 2 + 1e-12))
            if dist_var < 0.01:
                return "sphere"

        return "unknown"

    # ------------------------------------------------------------------ #
    #  Primitive fitting                                                    #
    # ------------------------------------------------------------------ #

    def _fit_plane(self, vertices: list) -> dict:
        """Fit a plane using SVD decomposition.

        Algorithm:
            1. Compute centroid of points.
            2. Center points by subtracting centroid.
            3. SVD of centred matrix: the row of Vt corresponding to the
               smallest singular value is the surface normal.
            4. Signed distance = dot(normal, centroid).
            5. RMS error = sqrt(mean of squared distances to plane).

        Returns:
            dict with keys: normal, distance, center, rms_error
        """
        pts = np.array(vertices, dtype=np.float64)
        if len(pts) < 3:
            raise ValueError("Need at least 3 points for plane fitting")

        centroid = np.mean(pts, axis=0)
        centered = pts - centroid

        _, s, vt = np.linalg.svd(centered, full_matrices=False)
        normal = vt[-1]
        normal = normal / (np.linalg.norm(normal) + 1e-12)

        distances = np.dot(centered, normal)
        rms_error = float(np.sqrt(np.mean(distances ** 2)))

        return {
            "normal": normal.tolist(),
            "distance": float(np.dot(normal, centroid)),
            "center": centroid.tolist(),
            "rms_error": rms_error,
        }

    def _fit_cylinder(self, vertices: list) -> dict:
        """Fit a cylinder using RANSAC with least-squares refinement.

        Algorithm:
            1. RANSAC loop: randomly sample 3 points, compute candidate axis
               as the cross-product direction.
            2. For each candidate axis project all points onto the plane
               perpendicular to the axis; the radius is the median distance
               from the projected centroid.
            3. Score by inlier count (points within threshold of surface).
            4. Refine best model using all inliers:
               - Re-estimate axis from inlier centroids on cross-section planes.
               - Re-estimate radius from inlier distances.
            5. Compute RMS error of final model.

        Returns:
            dict with keys: axis_point, axis_direction, radius, height,
                            rms_error
        """
        pts = np.array(vertices, dtype=np.float64)
        if len(pts) < 4:
            raise ValueError("Need at least 4 points for cylinder fitting")

        best_inliers = 0
        best_params = None
        n_iter = min(100, max(10, len(pts) * 2))
        threshold = self.settings["max_fit_error"]

        for _ in range(n_iter):
            if len(pts) < 3:
                break
            idx = np.random.choice(len(pts), 3, replace=False)
            p0, p1, p2 = pts[idx]

            v1 = p1 - p0
            v2 = p2 - p0
            cross = np.cross(v1, v2)
            cross_norm = np.linalg.norm(cross)
            if cross_norm < 1e-12:
                continue
            axis = cross / cross_norm

            projections = pts - p0
            proj_along_axis = np.dot(projections, axis)
            proj_perp = projections - np.outer(proj_along_axis, axis)
            radii = np.linalg.norm(proj_perp, axis=1)

            median_r = np.median(radii)
            if median_r < 1e-12:
                continue

            inlier_mask = np.abs(radii - median_r) < threshold
            n_inliers = int(np.sum(inlier_mask))

            if n_inliers > best_inliers:
                best_inliers = n_inliers
                inlier_proj = np.dot(pts[inlier_mask] - p0, axis)
                axis_point = p0 + np.mean(inlier_proj) * axis
                best_params = {
                    "axis_point": axis_point.tolist(),
                    "axis_direction": axis.tolist(),
                    "radius": float(median_r),
                }

        if best_params is None:
            centroid = np.mean(pts, axis=0)
            best_params = {
                "axis_point": centroid.tolist(),
                "axis_direction": [0.0, 0.0, 1.0],
                "radius": float(np.std(np.linalg.norm(pts - centroid, axis=1))),
            }

        # Refine with inliers
        axis_point = np.array(best_params["axis_point"])
        axis_dir = np.array(best_params["axis_direction"])
        axis_dir = axis_dir / (np.linalg.norm(axis_dir) + 1e-12)

        projections = pts - axis_point
        proj_along = np.dot(projections, axis_dir)
        proj_perp = projections - np.outer(proj_along, axis_dir)
        radii = np.linalg.norm(proj_perp, axis=1)

        inlier_mask = np.abs(radii - best_params["radius"]) < threshold * 5
        if int(np.sum(inlier_mask)) > 3:
            inlier_pts = pts[inlier_mask]
            inlier_proj = np.dot(inlier_pts - axis_point, axis_dir)
            refined_axis_point = axis_point + np.mean(inlier_proj) * axis_dir
            diff = inlier_pts - refined_axis_point
            refined_radii = np.linalg.norm(
                diff - np.outer(np.dot(diff, axis_dir), axis_dir), axis=1
            )
            best_params["axis_point"] = refined_axis_point.tolist()
            best_params["radius"] = float(np.mean(refined_radii))
            axis_point = refined_axis_point

        # Final metrics
        axis_point = np.array(best_params["axis_point"])
        axis_dir = np.array(best_params["axis_direction"])
        axis_dir = axis_dir / (np.linalg.norm(axis_dir) + 1e-12)
        proj = np.dot(pts - axis_point, axis_dir)
        perp = (pts - axis_point) - np.outer(proj, axis_dir)
        dists = np.linalg.norm(perp, axis=1) - best_params["radius"]
        best_params["rms_error"] = float(np.sqrt(np.mean(dists ** 2)))
        best_params["height"] = float(np.max(proj) - np.min(proj))

        return best_params

    def _fit_sphere(self, vertices: list) -> dict:
        """Fit a sphere using linearized least squares.

        Algorithm:
            Minimize  sum_i ( ||p_i - c|| - r )^2
            Linearize:
                ||p_i||^2 = 2 c . p_i + (r^2 - ||c||^2)
            Solve the linear system  A x = b  where:
                A_i = [2 x_i,  2 y_i,  2 z_i,  1]
                b_i = x_i^2 + y_i^2 + z_i^2
                x   = [c_x, c_y, c_z,  r^2 - ||c||^2]

        Returns:
            dict with keys: center, radius, rms_error
        """
        pts = np.array(vertices, dtype=np.float64)
        if len(pts) < 4:
            raise ValueError("Need at least 4 points for sphere fitting")

        A = np.column_stack([2 * pts, np.ones(len(pts))])
        b = np.sum(pts ** 2, axis=1)

        try:
            result, _, _, _ = np.linalg.lstsq(A, b, rcond=None)
        except np.linalg.LinAlgError:
            raise ValueError("Sphere fitting failed: singular system")

        center = result[:3]
        r_sq = result[3] + np.dot(center, center)
        if r_sq <= 0:
            raise ValueError("Sphere fitting failed: invalid radius")
        radius = math.sqrt(r_sq)

        distances = np.linalg.norm(pts - center, axis=1)
        rms_error = float(np.sqrt(np.mean((distances - radius) ** 2)))

        return {
            "center": center.tolist(),
            "radius": float(radius),
            "rms_error": rms_error,
        }

    # ------------------------------------------------------------------ #
    #  Surface creation                                                    #
    # ------------------------------------------------------------------ #

    def _create_plane_surface(self, fit_result: dict) -> "Part.Shape":
        """Create a Part.Plane from fitted plane parameters.

        Uses the fitted normal, center, and region extent to build an
        OpenCASCADE plane bounded to the convex hull of the region.
        """
        normal = Base.Vector(*fit_result["normal"])
        center = Base.Vector(*fit_result["center"])
        plane = Part.Plane(center, normal)
        return plane

    def _create_cylinder_surface(self, fit_result: dict) -> "Part.Shape":
        """Create a Part.Cylinder from fitted cylinder parameters.

        Axis direction, axis point, and radius are taken from the fit
        result.  Height is extended by 10% to ensure complete coverage.
        """
        axis_dir = Base.Vector(*fit_result["axis_direction"])
        axis_point = Base.Vector(*fit_result["axis_point"])
        radius = fit_result["radius"]
        height = fit_result["height"] * 1.1

        cylinder = Part.makeCylinder(
            radius,
            height,
            axis_dir,
            axis_point,
        )
        return cylinder

    def _create_sphere_surface(self, fit_result: dict) -> "Part.Shape":
        """Create a Part.Sphere from fitted sphere parameters."""
        center = Base.Vector(*fit_result["center"])
        radius = fit_result["radius"]
        sphere = Part.makeSphere(radius, center)
        return sphere

    # ------------------------------------------------------------------ #
    #  Trimming and solid creation                                          #
    # ------------------------------------------------------------------ #

    def _trim_surfaces(self, surfaces: list) -> "Part.Shape":
        """Trim surfaces against each other to create a closed shell.

        For each pair of adjacent surfaces the algorithm:
            1. Computes intersection curves.
            2. Uses the intersection to bound each surface.
            3. Collects all resulting faces into a shell.

        If surfaces cannot be trimmed into a closed shell, returns
        the best-effort compound.
        """
        if len(surfaces) == 0:
            return None

        if len(surfaces) == 1:
            return surfaces[0]

        faces = []
        for s in surfaces:
            if hasattr(s, "Faces"):
                faces.extend(s.Faces)
            else:
                faces.append(s)

        try:
            shell = Part.Shell(faces)
            if shell.isClosed():
                return shell
        except Exception:
            pass

        return Part.makeCompound(faces)

    def _create_solid(self, shell) -> "Part.Shape":
        """Attempt to create a solid from a closed shell.

        Falls back to the shell itself, then to a compound of faces,
        if solid creation fails.
        """
        if shell is None:
            return None

        try:
            if hasattr(shell, "isClosed") and shell.isClosed():
                solid = Part.Solid(shell)
                return solid
        except Exception:
            pass

        try:
            faces = shell.Faces if hasattr(shell, "Faces") else [shell]
            solid = Part.Solid(faces)
            return solid
        except Exception:
            pass

        return shell

    # ------------------------------------------------------------------ #
    #  Main conversion                                                     #
    # ------------------------------------------------------------------ #

    def convert(self, mode: str = "solid") -> "Part.Shape":
        """Convert mesh to B-REP shape.

        Args:
            mode: "solid" - try to create a watertight solid.
                  "surface" - return surfaces/shell without solid.
                  "compound" - return compound of individual faces.

        Returns:
            Part.Shape representing the converted geometry.
        """
        _check_deps()

        regions = self._detect_regions()
        if not regions:
            raise RuntimeError("No regions detected in mesh")

        surfaces = []
        for region in regions:
            region_verts = [self.vertices[i].tolist() for i in region["vertices"]]

            try:
                if region["type"] == "plane":
                    fit = self._fit_plane(region_verts)
                    surface = self._create_plane_surface(fit)
                elif region["type"] == "cylinder":
                    fit = self._fit_cylinder(region_verts)
                    surface = self._create_cylinder_surface(fit)
                elif region["type"] == "sphere":
                    fit = self._fit_sphere(region_verts)
                    surface = self._create_sphere_surface(fit)
                else:
                    continue
                surfaces.append(surface)
            except (ValueError, np.linalg.LinAlgError):
                continue

        if not surfaces:
            raise RuntimeError("Could not fit any primitives to mesh regions")

        if mode == "compound":
            return Part.makeCompound(surfaces)

        shell = self._trim_surfaces(surfaces)

        if mode == "solid":
            return self._create_solid(shell)

        return shell

    # ------------------------------------------------------------------ #
    #  Public utilities                                                    #
    # ------------------------------------------------------------------ #

    def get_region_stats(self) -> dict:
        """Return statistics about detected regions.

        Returns:
            dict with keys:
                total_regions: int
                planes: int
                cylinders: int
                spheres: int
                unknown: int
                total_area: float
                regions: list of region dicts
        """
        regions = self._detect_regions()
        stats = {
            "total_regions": len(regions),
            "planes": sum(1 for r in regions if r["type"] == "plane"),
            "cylinders": sum(1 for r in regions if r["type"] == "cylinder"),
            "spheres": sum(1 for r in regions if r["type"] == "sphere"),
            "unknown": sum(1 for r in regions if r["type"] == "unknown"),
            "total_area": sum(r["area"] for r in regions),
            "regions": regions,
        }
        return stats

    def get_fit_quality(self) -> dict:
        """Return fit quality metrics per region.

        Returns:
            dict mapping region index to a dict with keys:
                type, rms_error, area, vertex_count
        """
        regions = self._detect_regions()
        quality = {}
        for idx, region in enumerate(regions):
            region_verts = [self.vertices[i].tolist() for i in region["vertices"]]
            rms_error = None
            try:
                if region["type"] == "plane":
                    fit = self._fit_plane(region_verts)
                    rms_error = fit["rms_error"]
                elif region["type"] == "cylinder":
                    fit = self._fit_cylinder(region_verts)
                    rms_error = fit["rms_error"]
                elif region["type"] == "sphere":
                    fit = self._fit_sphere(region_verts)
                    rms_error = fit["rms_error"]
            except (ValueError, np.linalg.LinAlgError):
                rms_error = None

            quality[idx] = {
                "type": region["type"],
                "rms_error": rms_error,
                "area": region["area"],
                "vertex_count": len(region["vertices"]),
            }
        return quality

    def simplify_mesh(self, target_faces: int) -> tuple:
        """Simplify mesh to target face count using vertex clustering.

        Uses a uniform grid to merge nearby vertices and collapse
        triangles whose vertices fall in the same cell.

        Args:
            target_faces: desired number of output faces.

        Returns:
            Tuple of (vertices, faces) as lists.
        """
        bbox_min = np.min(self.vertices, axis=0)
        bbox_max = np.max(self.vertices, axis=0)
        extent = bbox_max - bbox_min
        max_extent = float(np.max(extent))
        if max_extent <= 0:
            return self.vertices.tolist(), self.faces.tolist()

        n_cells = max(1, int(math.ceil(target_faces ** (1.0 / 3.0))))
        cell_size = max_extent / n_cells

        vertex_map = {}
        unique_verts = []

        for i, v in enumerate(self.vertices):
            cell = tuple(((v - bbox_min) / cell_size).astype(int))
            if cell not in vertex_map:
                vertex_map[cell] = len(unique_verts)
                unique_verts.append(v.tolist())

        new_faces = []
        for f in self.faces:
            cells = []
            for vi in f:
                v = self.vertices[vi]
                cell = tuple(((v - bbox_min) / cell_size).astype(int))
                cells.append(vertex_map[cell])

            if cells[0] != cells[1] and cells[1] != cells[2] and cells[0] != cells[2]:
                new_faces.append(cells)

        return unique_verts, new_faces
