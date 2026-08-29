"""Surface mesh generation from point clouds using Ball-Pivoting Algorithm."""

from __future__ import annotations

from typing import Callable, Optional

import cv2
import numpy as np
from scipy.spatial import cKDTree


class MeshReconstructor:
    """Generate triangle meshes from point clouds.

    Supports Ball-Pivoting Algorithm for surface reconstruction.
    """

    def run(
        self,
        point_cloud: np.ndarray,
        colors: np.ndarray,
        method: str = "ball_pivot",
        progress_cb: Optional[Callable[[float, str], None]] = None,
    ) -> dict:
        """Generate mesh from point cloud.

        Args:
            point_cloud: Nx3 array of 3D points.
            colors: Nx3 array of point colors.
            method: Reconstruction method, currently only "ball_pivot".
            progress_cb: Optional callback(percent, message).

        Returns:
            Dict with vertices, faces, vertex_colors, normals.
        """
        if method != "ball_pivot":
            raise ValueError(f"Unsupported method '{method}', use 'ball_pivot'")

        if len(point_cloud) < 3:
            return {
                "vertices": np.empty((0, 3), dtype=np.float32),
                "faces": np.empty((0, 3), dtype=np.int32),
                "vertex_colors": np.empty((0, 3), dtype=np.uint8),
                "normals": np.empty((0, 3), dtype=np.float32),
            }

        if progress_cb:
            progress_cb(0.0, "Estimating normals")

        normals = self._estimate_normals(point_cloud)

        if progress_cb:
            progress_cb(20.0, "Computing ball radius")

        radius = self._compute_ball_radius(point_cloud)

        if progress_cb:
            progress_cb(25.0, "Building KDTree")

        tree = cKDTree(point_cloud)

        if progress_cb:
            progress_cb(30.0, "Finding seed triangles")

        seeds, seed_faces = self._find_seed_triangles(point_cloud, normals, tree, radius)

        if progress_cb:
            progress_cb(40.0, f"Growing mesh from {len(seed_faces)} seed triangles")

        vertices, faces = self._grow_mesh(
            point_cloud, normals, tree, seeds, seed_faces, radius, progress_cb
        )

        if progress_cb:
            progress_cb(85.0, "Removing duplicates and non-manifold edges")

        vertices, faces = self._clean_mesh(vertices, faces, point_cloud)

        vertex_colors = np.array(
            [colors[i] if i < len(colors) else [180, 180, 180] for i in range(len(vertices))],
            dtype=np.uint8,
        ) if len(colors) > 0 else np.full((len(vertices), 3), 180, dtype=np.uint8)

        vertex_normals = np.zeros((len(vertices), 3), dtype=np.float32)
        for i, face in enumerate(faces):
            for vi in face:
                vertex_normals[vi] += normals[face[0]]  # approximate
        norms = np.linalg.norm(vertex_normals, axis=1, keepdims=True)
        norms[norms < 1e-8] = 1.0
        vertex_normals /= norms

        if progress_cb:
            progress_cb(90.0, "Applying Laplacian smoothing")

        vertices = self._laplacian_smooth(vertices, faces, iterations=2)

        if progress_cb:
            progress_cb(100.0, f"Mesh complete: {len(vertices)} vertices, {len(faces)} faces")

        return {
            "vertices": vertices.astype(np.float32),
            "faces": faces.astype(np.int32),
            "vertex_colors": vertex_colors,
            "normals": vertex_normals.astype(np.float32),
        }

    def _estimate_normals(self, points: np.ndarray, k: int = 30) -> np.ndarray:
        """Estimate normals using PCA on local neighborhoods.

        Args:
            points: Nx3 point array.
            k: Number of neighbors for PCA.

        Returns:
            Nx3 normal array.
        """
        tree = cKDTree(points)
        _, neighbors = tree.query(points, k=min(k + 1, len(points)))

        normals = np.zeros_like(points, dtype=np.float32)

        for i in range(len(points)):
            pts = points[neighbors[i]]
            centroid = pts.mean(axis=0)
            centered = pts - centroid

            cov = centered.T @ centered
            eigenvalues, eigenvectors = np.linalg.eigh(cov)

            normal = eigenvectors[:, 0]
            if normal[2] > 0:
                normal = -normal

            normals[i] = normal

        return normals

    def _compute_ball_radius(self, points: np.ndarray, multiplier: float = 2.5) -> float:
        """Compute ball radius from average nearest-neighbor distance.

        Args:
            points: Nx3 point array.
            multiplier: Multiplier on median neighbor distance.

        Returns:
            Ball radius.
        """
        tree = cKDTree(points)
        dists, _ = tree.query(points, k=2)
        median_dist = np.median(dists[:, 1])
        return median_dist * multiplier

    def _find_seed_triangles(
        self,
        points: np.ndarray,
        normals: np.ndarray,
        tree: cKDTree,
        radius: float,
    ) -> tuple[list[int], list[tuple[int, int, int]]]:
        """Find seed triangles for mesh growing.

        Returns:
            Tuple of (seed vertex indices, list of seed face tuples).
        """
        seed_faces = []
        seed_vertices = set()

        # Find clusters of 3 points within ball radius
        n = len(points)
        checked = set()

        for i in range(min(n, 500)):
            neighbors = tree.query_ball_point(points[i], radius)
            neighbors = [j for j in neighbors if j != i]

            if len(neighbors) < 2:
                continue

            for j_idx in range(min(len(neighbors), 10)):
                for k_idx in range(j_idx + 1, min(len(neighbors), 10)):
                    j = neighbors[j_idx]
                    k = neighbors[k_idx]

                    key = tuple(sorted([i, j, k]))
                    if key in checked:
                        continue
                    checked.add(key)

                    if tree.query_ball_point(
                        (points[i] + points[j] + points[k]) / 3.0, radius * 0.5
                    ):
                        # Check normals are consistent
                        edge1 = points[j] - points[i]
                        edge2 = points[k] - points[i]
                        face_normal = np.cross(edge1, edge2)
                        face_norm_len = np.linalg.norm(face_normal)
                        if face_norm_len < 1e-8:
                            continue
                        face_normal /= face_norm_len

                        avg_normal = (normals[i] + normals[j] + normals[k]) / 3.0
                        avg_normal /= np.linalg.norm(avg_normal) + 1e-8

                        if abs(np.dot(face_normal, avg_normal)) > 0.3:
                            seed_faces.append((i, j, k))
                            seed_vertices.update([i, j, k])

                            if len(seed_faces) >= 50:
                                return list(seed_vertices), seed_faces

        return list(seed_vertices), seed_faces

    def _grow_mesh(
        self,
        points: np.ndarray,
        normals: np.ndarray,
        tree: cKDTree,
        seed_vertices: list[int],
        seed_faces: list[tuple[int, int, int]],
        radius: float,
        progress_cb: Optional[Callable[[float, str], None]] = None,
    ) -> tuple[list[int], list[tuple[int, int, int]]]:
        """Grow mesh from seed triangles using ball-pivoting.

        Returns:
            Tuple of (vertex indices used, list of face tuples).
        """
        all_faces = list(seed_faces)
        used_vertices = set(seed_vertices)
        boundary_edges = set()

        for face in seed_faces:
            for e in [(face[0], face[1]), (face[1], face[2]), (face[2], face[0])]:
                boundary_edges.add(e)

        iterations = 0
        max_iterations = len(points) * 2

        while boundary_edges and iterations < max_iterations:
            iterations += 1

            if iterations % 1000 == 0 and progress_cb:
                pct = 40.0 + min(45.0, (iterations / max_iterations) * 45.0)
                progress_cb(pct, f"Growing mesh: {len(all_faces)} faces, {len(used_vertices)} vertices")

            edge = boundary_edges.pop()
            v1, v2 = edge

            # Find candidate third vertex
            mid = (points[v1] + points[v2]) / 2.0
            candidates = tree.query_ball_point(mid, radius * 1.5)

            best_v3 = None
            best_score = -1

            for v3 in candidates:
                if v3 in (v1, v2):
                    continue

                key = tuple(sorted([v1, v2, v3]))
                if any(tuple(sorted(f)) == key for f in all_faces[-100:]):
                    continue

                edge1 = points[v2] - points[v1]
                edge2 = points[v3] - points[v1]
                cross = np.cross(edge1, edge2)
                cross_len = np.linalg.norm(cross)
                if cross_len < 1e-8:
                    continue
                face_normal = cross / cross_len

                avg_normal = (normals[v1] + normals[v2] + normals[v3]) / 3.0
                avg_norm_len = np.linalg.norm(avg_normal)
                if avg_norm_len < 1e-8:
                    continue
                avg_normal /= avg_norm_len

                score = abs(np.dot(face_normal, avg_normal))
                if score > best_score:
                    best_score = score
                    best_v3 = v3

            if best_v3 is not None and best_score > 0.2:
                new_face = (v1, v2, best_v3)
                all_faces.append(new_face)
                used_vertices.update([v1, v2, best_v3])

                for e in [(v1, best_v3), (best_v3, v2)]:
                    rev = (e[1], e[0])
                    if rev not in boundary_edges:
                        boundary_edges.add(e)

        return list(used_vertices), all_faces

    def _clean_mesh(
        self,
        vertex_indices: list[int],
        faces: list[tuple[int, int, int]],
        points: np.ndarray,
    ) -> tuple[np.ndarray, list[tuple[int, int, int]]]:
        """Remove duplicate faces and re-index vertices.

        Returns:
            Tuple of (vertex array Nx3, cleaned face list).
        """
        if not faces:
            return np.empty((0, 3), dtype=np.float32), []

        # Remove duplicate faces
        seen = set()
        unique_faces = []
        for f in faces:
            key = tuple(sorted(f))
            if key not in seen:
                seen.add(key)
                unique_faces.append(f)

        # Re-index vertices to only those used
        used_verts = sorted(set(v for f in unique_faces for v in f))
        old_to_new = {old: new for new, old in enumerate(used_verts)}

        vertices = np.array([points[i] for i in used_verts], dtype=np.float32)
        reindexed = [(old_to_new[a], old_to_new[b], old_to_new[c]) for a, b, c in unique_faces]

        return vertices, reindexed

    def _laplacian_smooth(
        self,
        vertices: np.ndarray,
        faces: list[tuple[int, int, int]],
        iterations: int = 2,
        factor: float = 0.3,
    ) -> np.ndarray:
        """Apply simple Laplacian smoothing.

        Args:
            vertices: Nx3 vertex array.
            faces: List of face tuples.
            iterations: Number of smoothing iterations.
            factor: Blending factor.

        Returns:
            Smoothed vertex array.
        """
        if len(vertices) == 0 or not faces:
            return vertices

        adjacency: dict[int, set[int]] = {i: set() for i in range(len(vertices))}
        for a, b, c in faces:
            adjacency[a].update([b, c])
            adjacency[b].update([a, c])
            adjacency[c].update([a, b])

        smoothed = vertices.copy()
        for _ in range(iterations):
            new_verts = smoothed.copy()
            for i in range(len(smoothed)):
                neighbors = list(adjacency.get(i, set()))
                if neighbors:
                    avg = smoothed[neighbors].mean(axis=0)
                    new_verts[i] = smoothed[i] + factor * (avg - smoothed[i])
            smoothed = new_verts

        return smoothed
