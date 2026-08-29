"""QThread worker for mesh reconstruction."""

import traceback
from typing import Any

import numpy as np
from PySide6 import QtCore


class MeshWorker(QtCore.QThread):
    """Background thread for mesh generation from a dense point cloud.

    Runs Poisson or Delaunay surface reconstruction and optional
    mesh simplification.

    Signals:
        progress(int, str): Emitted with percent (0-100) and status message.
        finished(dict): Emitted with vertices, faces, vertex_colors, normals.
        error(str): Emitted with traceback on failure.
    """

    progress = QtCore.Signal(int, str)
    finished = QtCore.Signal(dict)
    error = QtCore.Signal(str)

    def __init__(
        self,
        point_cloud: np.ndarray,
        colors: np.ndarray,
        settings: dict,
        parent: Any = None,
    ) -> None:
        super().__init__(parent)
        self.point_cloud = point_cloud
        self.colors = colors
        self.settings = settings

    def run(self) -> None:
        try:
            import open3d as o3d

            self.progress.emit(5, "Preparing point cloud...")

            if len(self.point_cloud) == 0:
                raise ValueError("Point cloud is empty")

            pcd = o3d.geometry.PointCloud()
            pcd.points = o3d.utility.Vector3dVector(self.point_cloud)
            pcd.colors = o3d.utility.Vector3dVector(
                self.colors.astype(np.float64) / 255.0
            )

            # Statistical outlier removal
            self.progress.emit(10, "Removing outliers...")
            _, inlier_idx = pcd.remove_statistical_outlier(
                nb_neighbors=self.settings.get("nb_neighbors", 20),
                std_ratio=self.settings.get("std_ratio", 2.0),
            )
            pcd = pcd.select_by_index(inlier_idx)

            # Estimate normals
            self.progress.emit(25, "Estimating normals...")
            pcd.estimate_normals(
                search_param=o3d.geometry.KDTreeSearchParamHybrid(
                    radius=self.settings.get("normal_radius", 0.1),
                    max_nn=self.settings.get("normal_max_nn", 30),
                )
            )
            pcd.orient_normals_consistent_tangent_plane(k=15)

            method = self.settings.get("method", "poisson")

            if method == "poisson":
                # --- Poisson reconstruction ---
                self.progress.emit(40, "Running Poisson surface reconstruction...")
                depth = self.settings.get("poisson_depth", 9)
                mesh, densities = o3d.geometry.TriangleMesh.create_from_point_cloud_poisson(
                    pcd, depth=depth
                )

                # Trim low-density vertices
                self.progress.emit(70, "Trimming low-density regions...")
                densities = np.asarray(densities)
                density_threshold = np.quantile(
                    densities, self.settings.get("density_threshold", 0.05)
                )
                vertices_to_remove = densities < density_threshold
                mesh.remove_vertices_by_mask(vertices_to_remove)

            elif method == "delaunay":
                # --- Delaunay / Convex Hull ---
                self.progress.emit(40, "Computing convex hull mesh...")
                mesh = o3d.geometry.TriangleMesh.create_from_point_cloud_alpha_shape(
                    pcd, alpha=self.settings.get("alpha", 0.5)
                )

            else:
                raise ValueError(f"Unsupported mesh method: {method}")

            # Simplify mesh
            target_faces = self.settings.get("target_faces", 0)
            if target_faces > 0 and len(mesh.triangles) > target_faces:
                self.progress.emit(80, f"Simplifying mesh to {target_faces} faces...")
                mesh = mesh.simplify_quadric_decimation(target_number_of_triangles=target_faces)

            # Smooth
            if self.settings.get("smooth_iterations", 0) > 0:
                self.progress.emit(88, "Smoothing mesh...")
                mesh = mesh.filter_smooth_laplacian(
                    number_of_iterations=self.settings["smooth_iterations"],
                    lambda_filter=self.settings.get("lambda_filter", 0.5),
                )
                mesh.compute_vertex_normals()

            # Compute per-vertex colors via nearest-neighbor lookup
            self.progress.emit(92, "Computing vertex colors...")
            mesh_vertex_colors = self._sample_colors(mesh, pcd)

            mesh.vertex_colors = o3d.utility.Vector3dVector(mesh_vertex_colors)
            mesh.compute_vertex_normals()

            vertices = np.asarray(mesh.vertices).astype(np.float64)
            faces = np.asarray(mesh.triangles).astype(np.int32)
            vertex_colors = np.asarray(mesh.vertex_colors).astype(np.float64)
            normals = np.asarray(mesh.vertex_normals).astype(np.float64)

            self.progress.emit(100, f"Mesh complete: {len(vertices)} vertices, {len(faces)} faces")
            self.finished.emit({
                "vertices": vertices,
                "faces": faces,
                "vertex_colors": vertex_colors,
                "normals": normals,
            })

        except Exception as e:
            tb = traceback.format_exc()
            self.error.emit(f"{e}\n{tb}")

    def _sample_colors(
        self, mesh: Any, pcd: Any
    ) -> np.ndarray:
        """Sample colors from the point cloud onto mesh vertices."""
        import open3d as o3d

        mesh_vertices = np.asarray(mesh.vertices)
        pcd_points = np.asarray(pcd.points)
        pcd_colors = np.asarray(pcd.colors)

        tree = o3d.geometry.KDTreeFlann(pcd)
        vertex_colors = np.zeros((len(mesh_vertices), 3), dtype=np.float64)

        for i, v in enumerate(mesh_vertices):
            _, idx, _ = tree.search_knn_vector_3d(v, 1)
            vertex_colors[i] = pcd_colors[idx[0]]

        return vertex_colors
