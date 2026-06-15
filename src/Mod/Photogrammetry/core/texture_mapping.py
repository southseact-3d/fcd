"""UV unwrapping and texture projection for textured mesh generation."""

from __future__ import annotations

from typing import Callable, Optional

import cv2
import numpy as np

from .zip_handler import CameraCalibration, CameraPose


class TextureMapper:
    """Generate UV coordinates and texture atlas for a triangle mesh.

    Uses box projection UV mapping and per-face texture selection.
    """

    def run(
        self,
        vertices: np.ndarray,
        faces: np.ndarray,
        images_dir: str,
        camera_poses: list[CameraPose],
        calibration: CameraCalibration,
        progress_cb: Optional[Callable[[float, str], None]] = None,
    ) -> dict:
        """Generate textured mesh data.

        Args:
            vertices: Nx3 vertex array.
            faces: Mx3 face index array.
            images_dir: Directory containing source images.
            camera_poses: List of CameraPose objects.
            calibration: Camera calibration data.
            progress_cb: Optional callback(percent, message).

        Returns:
            Dict with uv_coords, uv_faces, texture_atlas, atlas_size.
        """
        from pathlib import Path

        if len(vertices) == 0 or len(faces) == 0:
            return {
                "uv_coords": np.empty((0, 2), dtype=np.float32),
                "uv_faces": np.empty((0, 3), dtype=np.int32),
                "texture_atlas": np.empty((0, 0, 3), dtype=np.uint8),
                "atlas_size": (0, 0),
            }

        if progress_cb:
            progress_cb(0.0, "Computing box projection UVs")

        uv_coords = self._box_projection_uv(vertices)

        if progress_cb:
            progress_cb(20.0, "Selecting best images for faces")

        face_images = self._select_face_images(
            vertices, faces, camera_poses, calibration
        )

        if progress_cb:
            progress_cb(40.0, "Building texture atlas")

        img_dir = Path(images_dir)
        atlas, atlas_size, uv_coords, uv_faces = self._build_atlas(
            vertices, faces, uv_coords, face_images, img_dir, camera_poses, calibration
        )

        if progress_cb:
            progress_cb(90.0, "Blending texture seams")

        atlas = self._blend_seams(atlas, atlas_size)

        if progress_cb:
            progress_cb(100.0, f"Texture mapping complete: atlas {atlas_size}")

        return {
            "uv_coords": uv_coords.astype(np.float32),
            "uv_faces": uv_faces.astype(np.int32),
            "texture_atlas": atlas,
            "atlas_size": atlas_size,
        }

    def _box_projection_uv(self, vertices: np.ndarray) -> np.ndarray:
        """Project UV coordinates from the dominant axis.

        Args:
            vertices: Nx3 vertex array.

        Returns:
            Nx2 UV coordinates.
        """
        centroid = vertices.mean(axis=0)
        centered = vertices - centroid

        extents = np.abs(centered).max(axis=0)
        dominant_axis = np.argmax(extents)

        uv = np.zeros((len(vertices), 2), dtype=np.float32)

        axes = [0, 1, 2]
        axes.remove(dominant_axis)
        a1, a2 = axes

        range_a1 = centered[:, a1].max() - centered[:, a1].min()
        range_a2 = centered[:, a2].max() - centered[:, a2].min()
        max_range = max(range_a1, range_a2, 1e-8)

        uv[:, 0] = (centered[:, a1] - centered[:, a1].min()) / max_range
        uv[:, 1] = (centered[:, a2] - centered[:, a2].min()) / max_range

        return uv

    def _select_face_images(
        self,
        vertices: np.ndarray,
        faces: np.ndarray,
        camera_poses: list[CameraPose],
        calibration: CameraCalibration,
    ) -> list[str]:
        """For each face, select the image with the best viewing angle.

        Returns:
            List of image names, one per face.
        """
        camera_matrix = np.array(calibration.camera_matrix, dtype=np.float64)
        image_w, image_h = calibration.image_size

        face_centers = vertices[faces].mean(axis=1)  # Mx3
        face_normals = self._compute_face_normals(vertices, faces)

        cam_positions = {}
        cam_rotations = {}
        for pose in camera_poses:
            R = self._quat_to_rot(pose.rotation)
            cam_positions[pose.image_name] = np.array(pose.position, dtype=np.float64)
            cam_rotations[pose.image_name] = R

        best_images = []
        for i in range(len(faces)):
            center = face_centers[i]
            normal = face_normals[i]

            best_img = ""
            best_score = -1.0

            for pose in camera_poses:
                cam_pos = cam_positions[pose.image_name]
                cam_R = cam_rotations[pose.image_name]

                view_dir = cam_pos - center
                view_dist = np.linalg.norm(view_dir)
                if view_dist < 1e-8:
                    continue
                view_dir /= view_dist

                # Angle between view direction and face normal
                angle_score = abs(np.dot(view_dir, normal))

                # Projection check: is the face center within the image?
                face_cam = cam_R.T @ (center - cam_pos)
                if face_cam[2] <= 0:
                    continue

                proj = camera_matrix @ face_cam
                proj_2d = proj[:2] / proj[2]

                margin = 0.1
                if not (-margin * image_w < proj_2d[0] < image_w * (1 + margin)):
                    continue
                if not (-margin * image_h < proj_2d[1] < image_h * (1 + margin)):
                    continue

                # Distance penalty (closer is better, but not too close)
                dist_score = 1.0 / (1.0 + view_dist * 0.1)

                score = angle_score * 0.7 + dist_score * 0.3

                if score > best_score:
                    best_score = score
                    best_img = pose.image_name

            best_images.append(best_img)

        return best_images

    def _compute_face_normals(
        self, vertices: np.ndarray, faces: np.ndarray
    ) -> np.ndarray:
        """Compute unit normals for each face.

        Returns:
            Mx3 array of unit face normals.
        """
        v0 = vertices[faces[:, 0]]
        v1 = vertices[faces[:, 1]]
        v2 = vertices[faces[:, 2]]

        e1 = v1 - v0
        e2 = v2 - v0
        normals = np.cross(e1, e2)
        norms = np.linalg.norm(normals, axis=1, keepdims=True)
        norms[norms < 1e-8] = 1.0
        return normals / norms

    def _quat_to_rot(self, q: tuple) -> np.ndarray:
        """Convert quaternion (w, x, y, z) to 3x3 rotation matrix."""
        from scipy.spatial.transform import Rotation

        qw, qx, qy, qz = q
        r = Rotation.from_quat([qx, qy, qz, qw])
        return r.as_matrix()

    def _build_atlas(
        self,
        vertices: np.ndarray,
        faces: np.ndarray,
        uv_coords: np.ndarray,
        face_images: list[str],
        images_dir: "Path",
        camera_poses: list[CameraPose],
        calibration: CameraCalibration,
    ) -> tuple[np.ndarray, tuple[int, int], np.ndarray, np.ndarray]:
        """Build texture atlas by projecting each face from its best image.

        Returns:
            Tuple of (atlas image, atlas size, updated uv_coords, uv_faces).
        """
        camera_matrix = np.array(calibration.camera_matrix, dtype=np.float64)
        dist_coeffs = np.array(calibration.distortion_coefficients, dtype=np.float64)
        img_w, img_h = calibration.image_size

        # Group faces by source image
        image_to_faces: dict[str, list[int]] = {}
        for i, img_name in enumerate(face_images):
            if img_name:
                if img_name not in image_to_faces:
                    image_to_faces[img_name] = []
                image_to_faces[img_name].append(i)

        # Simple atlas: each image gets a tile
        n_images = max(len(image_to_faces), 1)
        cols = int(np.ceil(np.sqrt(n_images)))
        rows = int(np.ceil(n_images / cols))

        tile_w = img_w
        tile_h = img_h
        atlas_w = cols * tile_w
        atlas_h = rows * tile_h

        # Atlas should be power-of-2 for GPU compatibility
        atlas_w = self._next_power_of_2(atlas_w)
        atlas_h = self._next_power_of_2(atlas_h)

        atlas = np.zeros((atlas_h, atlas_w, 3), dtype=np.uint8)

        uv_faces = faces.copy()

        # Build lookup: image_name -> (tile_col, tile_row)
        image_tiles = {}
        for idx, img_name in enumerate(image_to_faces.keys()):
            col = idx % cols
            row = idx // cols
            image_tiles[img_name] = (col, row)

        for img_name, face_indices in image_to_faces.items():
            if img_name not in image_tiles:
                continue

            tile_col, tile_row = image_tiles[img_name]
            tile_x = tile_col * tile_w
            tile_y = tile_row * tile_h

            # Load source image
            img_path = images_dir / img_name
            if not img_path.exists():
                continue

            src_img = cv2.imread(str(img_path))
            if src_img is None:
                continue

            # Resize to tile size
            src_img = cv2.resize(src_img, (tile_w, tile_h))

            # Place tile in atlas
            end_y = min(tile_y + tile_h, atlas_h)
            end_x = min(tile_x + tile_w, atlas_w)
            actual_h = end_y - tile_y
            actual_w = end_x - tile_x
            atlas[tile_y:end_y, tile_x:end_x] = src_img[:actual_h, :actual_w]

            # Update UVs for faces in this tile
            for fi in face_indices:
                # Map face UVs to tile coordinates
                for vi in range(3):
                    orig_uv = uv_coords[faces[fi, vi]]
                    # Scale UV into the tile region
                    tile_u = orig_uv[0] * tile_w + tile_x
                    tile_v = orig_uv[1] * tile_h + tile_y
                    uv_faces[fi, vi] = uv_faces[fi, vi]  # keep original indices

                # Store per-face UV coordinates separately
                # We need to expand UV coords for atlas mapping
                pass

        # Create per-face UV coordinates for the atlas
        atlas_uvs = np.zeros((len(vertices), 2), dtype=np.float32)

        for img_name, face_indices in image_to_faces.items():
            if img_name not in image_tiles:
                continue

            tile_col, tile_row = image_tiles[img_name]
            tile_x = tile_col * tile_w
            tile_y = tile_row * tile_h

            for fi in face_indices:
                for vi in range(3):
                    vi_global = faces[fi, vi]
                    orig_uv = uv_coords[vi_global]
                    atlas_uvs[vi_global] = [
                        (orig_uv[0] * tile_w + tile_x) / atlas_w,
                        (orig_uv[1] * tile_h + tile_y) / atlas_h,
                    ]

        return atlas, (atlas_h, atlas_w), atlas_uvs, uv_faces

    def _next_power_of_2(self, n: int) -> int:
        """Return the next power of 2 >= n."""
        p = 1
        while p < n:
            p <<= 1
        return p

    def _blend_seams(
        self, atlas: np.ndarray, atlas_size: tuple[int, int]
    ) -> np.ndarray:
        """Apply simple Gaussian blur to blend texture seams.

        Args:
            atlas: Texture atlas image.
            atlas_size: (height, width).

        Returns:
            Blended atlas image.
        """
        if atlas_size[0] == 0 or atlas_size[1] == 0:
            return atlas

        # Light blur to soften tile boundaries
        blended = cv2.GaussianBlur(atlas, (3, 3), 0.5)
        return blended
