"""QThread worker for dense reconstruction."""

import os
import traceback
from typing import Any

import numpy as np
from PySide6 import QtCore


class DenseWorker(QtCore.QThread):
    """Background thread for dense point cloud reconstruction.

    Uses multi-view stereo to generate a dense point cloud from
    images and sparse camera poses.

    Signals:
        progress(int, str): Emitted with percent (0-100) and status message.
        finished(dict): Emitted with dense_cloud and colors on success.
        error(str): Emitted with traceback on failure.
    """

    progress = QtCore.Signal(int, str)
    finished = QtCore.Signal(dict)
    error = QtCore.Signal(str)

    def __init__(
        self,
        images_dir: str,
        camera_poses: dict,
        calibration: dict,
        settings: dict,
        parent: Any = None,
    ) -> None:
        super().__init__(parent)
        self.images_dir = images_dir
        self.camera_poses = camera_poses
        self.calibration = calibration
        self.settings = settings

    def run(self) -> None:
        try:
            import cv2

            _mod_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            if _mod_dir not in os.sys.path:
                os.sys.path.insert(0, _mod_dir)

            self.progress.emit(5, "Loading images and camera poses...")

            image_extensions = (".jpg", ".jpeg", ".png", ".bmp", ".tiff")
            image_paths = sorted(
                os.path.join(self.images_dir, f)
                for f in os.listdir(self.images_dir)
                if f.lower().endswith(image_extensions)
            )

            if not image_paths:
                raise FileNotFoundError(
                    f"No images found in directory: {self.images_dir}"
                )

            images = []
            for path in image_paths:
                img = cv2.imread(path)
                if img is not None:
                    images.append(img)

            if len(images) < 2:
                raise ValueError(f"Need at least 2 images, got {len(images)}")

            # Build camera intrinsics
            calibrated = self.calibration is not None and bool(self.calibration)
            fx = self.calibration.get("fx", 1000.0) if calibrated else 1000.0
            fy = self.calibration.get("fy", 1000.0) if calibrated else 1000.0
            cx = self.calibration.get("cx", 640.0) if calibrated else 640.0
            cy = self.calibration.get("cy", 480.0) if calibrated else 480.0
            camera_matrix = np.array(
                [[fx, 0, cx], [0, fy, cy], [0, 0, 1]], dtype=np.float64
            )

            # Build SGBM matcher
            block_size = self.settings.get("block_size", 5)
            num_disparities = self.settings.get("num_disparities", 128)
            uniqueness_ratio = self.settings.get("uniqueness_ratio", 10)

            if num_disparities % 16 != 0:
                num_disparities = ((num_disparities // 16) + 1) * 16

            stereo = cv2.StereoSGBM_create(
                minDisparity=0,
                numDisparities=num_disparities,
                blockSize=block_size,
                P1=8 * 3 * block_size ** 2,
                P2=32 * 3 * block_size ** 2,
                disp12MaxDiff=1,
                uniquenessRatio=uniqueness_ratio,
                speckleWindowSize=50,
                speckleRange=32,
                preFilterCap=63,
                mode=cv2.STEREO_SGBM_MODE_SGBM_3WAY,
            )
            wls_filter = cv2.ximgproc.createDisparityWLSFilter(stereo)
            right_matcher = cv2.ximgproc.createRightMatcher(stereo)

            all_points = []
            all_colors = []
            total_pairs = len(images) - 1

            for i in range(len(images) - 1):
                pct = 10 + int((i / total_pairs) * 75)
                self.progress.emit(
                    pct, f"Computing disparity {i + 1}/{total_pairs}"
                )

                gray_l = cv2.cvtColor(images[i], cv2.COLOR_BGR2GRAY)
                gray_r = cv2.cvtColor(images[i + 1], cv2.COLOR_BGR2GRAY)

                disp_l = stereo.compute(gray_l, gray_r)
                disp_r = right_matcher.compute(gray_r, gray_l)
                disp_filtered = wls_filter.filter(disp_l, gray_l, disparity_map_right=disp_r)

                # Reproject to 3D
                points_3d = cv2.reprojectImageTo3D(
                    disp_filtered, cv2.eye(4, 4, dtype=np.float64)
                )
                mask = disp_filtered > 0
                pts = points_3d[mask]
                colors = images[i][mask]

                if len(pts) > 0:
                    all_points.append(pts.astype(np.float64))
                    all_colors.append(colors[:, ::-1].astype(np.uint8))

            self.progress.emit(90, "Combining dense point clouds...")

            if all_points:
                dense_cloud = np.vstack(all_points)
                dense_colors = np.vstack(all_colors)
            else:
                dense_cloud = np.empty((0, 3), dtype=np.float64)
                dense_colors = np.empty((0, 3), dtype=np.uint8)

            # Downsample if too many points
            max_points = self.settings.get("max_dense_points", 2_000_000)
            if len(dense_cloud) > max_points:
                self.progress.emit(92, f"Downsampling to {max_points} points...")
                indices = np.random.choice(len(dense_cloud), max_points, replace=False)
                dense_cloud = dense_cloud[indices]
                dense_colors = dense_colors[indices]

            self.progress.emit(100, f"Dense reconstruction complete: {len(dense_cloud)} points")
            self.finished.emit({
                "dense_cloud": dense_cloud,
                "colors": dense_colors,
                "num_points": len(dense_cloud),
            })

        except Exception as e:
            tb = traceback.format_exc()
            self.error.emit(f"{e}\n{tb}")
