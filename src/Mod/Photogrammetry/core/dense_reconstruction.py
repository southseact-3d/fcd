"""Dense point cloud generation from SfM results using stereo matching."""

from __future__ import annotations

from typing import Callable, Optional

import cv2
import numpy as np

from .zip_handler import CameraCalibration


class DenseReconstructor:
    """Generate dense point clouds from calibrated image pairs.

    Args:
        calibration: Camera intrinsics and distortion parameters.
    """

    def __init__(self, calibration: CameraCalibration) -> None:
        self.calibration = calibration
        self.camera_matrix = np.array(calibration.camera_matrix, dtype=np.float64)
        self.dist_coeffs = np.array(calibration.distortion_coefficients, dtype=np.float64)
        self.image_size = calibration.image_size

    def _compute_stereo_params(
        self, R: np.ndarray, t: np.ndarray
    ) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
        """Compute stereo rectification matrices.

        Returns:
            Tuple of (R1, R2, Q) for the rectified pair.
        """
        R1, R2, P1, P2, Q, _, _ = cv2.stereoRectify(
            self.camera_matrix, self.dist_coeffs,
            self.camera_matrix, self.dist_coeffs,
            self.image_size, R, t,
            alpha=0,
        )
        return R1, R2, Q

    def _rectify_and_match(
        self,
        img1: np.ndarray,
        img2: np.ndarray,
        R1: np.ndarray,
        R2: np.ndarray,
        Q: np.ndarray,
    ) -> tuple[np.ndarray, np.ndarray]:
        """Rectify images and compute stereo disparity.

        Returns:
            Tuple of (disparity map, masked points 3D).
        """
        w, h = self.image_size

        map1x, map1y = cv2.initUndistortRectifyMap(
            self.camera_matrix, self.dist_coeffs, R1,
            self.camera_matrix, (w, h), cv2.CV_32FC1
        )
        map2x, map2y = cv2.initUndistortRectifyMap(
            self.camera_matrix, self.dist_coeffs, R2,
            self.camera_matrix, (w, h), cv2.CV_32FC1
        )

        rect1 = cv2.remap(img1, map1x, map1y, cv2.INTER_LINEAR)
        rect2 = cv2.remap(img2, map2x, map2y, cv2.INTER_LINEAR)

        gray1 = cv2.cvtColor(rect1, cv2.COLOR_BGR2GRAY) if len(rect1.shape) == 3 else rect1
        gray2 = cv2.cvtColor(rect2, cv2.COLOR_BGR2GRAY) if len(rect2.shape) == 3 else rect2

        num_disp = 128
        blockSize = 5
        stereo = cv2.StereoSGBM_create(
            minDisparity=0,
            numDisparities=num_disp,
            blockSize=blockSize,
            P1=8 * 3 * blockSize ** 2,
            P2=32 * 3 * blockSize ** 2,
            disp12MaxDiff=1,
            uniquenessRatio=10,
            speckleWindowSize=100,
            speckleRange=32,
            preFilterCap=63,
            mode=cv2.STEREO_SGBM_MODE_SGBM_3WAY,
        )

        disparity = stereo.compute(gray1, gray2).astype(np.float32) / 16.0

        # Back-project to 3D
        points_3d = cv2.reprojectImageTo3D(disparity, Q)

        return disparity, points_3d

    def _filter_and_color(
        self,
        points_3d: np.ndarray,
        img1: np.ndarray,
        disparity: np.ndarray,
    ) -> tuple[np.ndarray, np.ndarray]:
        """Filter invalid points and assign colors from the reference image.

        Returns:
            Tuple of (points Nx3, colors Nx3).
        """
        mask = (disparity > 0.5) & np.isfinite(points_3d[:, :, 2])
        z = points_3d[:, :, 2]
        mask &= (z > 0) & (z < 100)

        valid_coords = np.where(mask)
        if len(valid_coords[0]) == 0:
            return np.empty((0, 3), dtype=np.float32), np.empty((0, 3), dtype=np.uint8)

        pts = points_3d[valid_coords]
        colors = img1[valid_coords]

        if len(colors.shape) == 2:
            colors = cv2.cvtColor(colors.reshape(-1, 1), cv2.COLOR_GRAY2BGR).reshape(-1, 3)

        return pts.astype(np.float32), colors.astype(np.uint8)

    def _statistical_outlier_removal(
        self,
        points: np.ndarray,
        colors: np.ndarray,
        k_neighbors: int = 20,
        std_ratio: float = 2.0,
    ) -> tuple[np.ndarray, np.ndarray]:
        """Remove statistical outliers based on local neighborhood distance.

        Args:
            points: Nx3 point array.
            colors: Nx3 color array.
            k_neighbors: Number of neighbors for mean distance computation.
            std_ratio: Standard deviation threshold.

        Returns:
            Filtered (points, colors).
        """
        from scipy.spatial import cKDTree

        if len(points) < k_neighbors:
            return points, colors

        tree = cKDTree(points)
        distances, _ = tree.query(points, k=k_neighbors + 1)
        mean_dists = distances[:, 1:].mean(axis=1)

        mu = mean_dists.mean()
        sigma = mean_dists.std()

        mask = mean_dists < mu + std_ratio * sigma
        return points[mask], colors[mask]

    def _voxel_downsample(
        self,
        points: np.ndarray,
        colors: np.ndarray,
        voxel_size: float = 0.01,
    ) -> tuple[np.ndarray, np.ndarray]:
        """Downsample point cloud by averaging points within voxels.

        Args:
            points: Nx3 point array.
            colors: Nx3 color array.
            voxel_size: Size of each voxel.

        Returns:
            Downsampled (points, colors).
        """
        if len(points) == 0:
            return points, colors

        voxel_indices = np.floor(points / voxel_size).astype(np.int32)

        # Use a dict to aggregate voxels
        voxel_map: dict[tuple, list[int]] = {}
        for i, vi in enumerate(voxel_indices):
            key = (int(vi[0]), int(vi[1]), int(vi[2]))
            if key not in voxel_map:
                voxel_map[key] = []
            voxel_map[key].append(i)

        new_points = []
        new_colors = []
        for indices in voxel_map.values():
            pts = points[indices]
            cols = colors[indices]
            new_points.append(pts.mean(axis=0))
            new_colors.append(cols.mean(axis=0).astype(np.uint8))

        return np.array(new_points, dtype=np.float32), np.array(new_colors, dtype=np.uint8)

    def run(
        self,
        images_dir: str,
        camera_poses: list[dict],
        progress_cb: Optional[Callable[[float, str], None]] = None,
    ) -> dict:
        """Generate dense point cloud from image pairs.

        Args:
            images_dir: Directory containing the source images.
            camera_poses: List of dicts with 'image', 'R', 't' keys from SfM.
            progress_cb: Optional callback(percent, message).

        Returns:
            Dict with 'dense_cloud' (Nx3) and 'colors' (Nx3).
        """
        from pathlib import Path

        img_dir = Path(images_dir)
        all_points = []
        all_colors = []

        # Find neighboring pairs based on pose similarity
        pairs = []
        for i in range(len(camera_poses)):
            for j in range(i + 1, min(i + 4, len(camera_poses))):
                pairs.append((i, j))

        total = len(pairs)

        for idx, (i, j) in enumerate(pairs):
            if progress_cb:
                pct = (idx / max(total, 1)) * 90.0
                progress_cb(pct, f"Processing pair {idx + 1}/{total}")

            pose_a = camera_poses[i]
            pose_b = camera_poses[j]

            img_path_a = img_dir / pose_a["image"]
            img_path_b = img_dir / pose_b["image"]

            if not img_path_a.exists() or not img_path_b.exists():
                continue

            img1 = cv2.imread(str(img_path_a))
            img2 = cv2.imread(str(img_path_b))

            if img1 is None or img2 is None:
                continue

            R_rel = pose_b["R"] @ pose_a["R"].T
            t_rel = pose_a["R"].T @ (pose_b["t"] - pose_a["t"])

            try:
                R1, R2, Q = self._compute_stereo_params(R_rel, t_rel)

                disparity, points_3d = self._rectify_and_match(img1, img2, R1, R2, Q)
                pts, cols = self._filter_and_color(points_3d, img1, disparity)

                if len(pts) > 0:
                    # Transform points to world frame
                    pts_world = (pose_a["R"] @ pts.T + pose_a["t"]).T
                    all_points.append(pts_world)
                    all_colors.append(cols)
            except cv2.error:
                continue

        if progress_cb:
            progress_cb(90.0, "Merging and filtering dense cloud")

        if not all_points:
            return {"dense_cloud": np.empty((0, 3), dtype=np.float32),
                    "colors": np.empty((0, 3), dtype=np.uint8)}

        merged_pts = np.vstack(all_points)
        merged_cols = np.vstack(all_colors)

        merged_pts, merged_cols = self._statistical_outlier_removal(merged_pts, merged_cols)

        # Auto voxel size based on density
        if len(merged_pts) > 100000:
            from scipy.spatial import cKDTree
            tree = cKDTree(merged_pts)
            dists, _ = tree.query(merged_pts, k=2)
            median_dist = np.median(dists[:, 1])
            voxel_size = median_dist * 2.0
            merged_pts, merged_cols = self._voxel_downsample(merged_pts, merged_cols, voxel_size)

        if progress_cb:
            progress_cb(100.0, f"Dense reconstruction complete: {len(merged_pts)} points")

        return {
            "dense_cloud": merged_pts,
            "colors": merged_cols,
        }
