"""QThread worker for the full SfM pipeline (feature extraction + matching + sparse reconstruction)."""

import os
import traceback
from typing import Any

from PySide6 import QtCore


class SfMWorker(QtCore.QThread):
    """Background thread for Structure-from-Motion pipeline.

    Runs feature extraction, feature matching, and incremental SfM
    sequentially, emitting progress updates throughout.

    Signals:
        progress(int, str): Emitted with percent (0-100) and status message.
        finished(dict): Emitted with results on success.
        error(str): Emitted with traceback on failure.
    """

    progress = QtCore.Signal(int, str)
    finished = QtCore.Signal(dict)
    error = QtCore.Signal(str)

    def __init__(
        self,
        images_dir: str,
        poses: list,
        calibration: dict,
        settings: dict,
        parent: Any = None,
    ) -> None:
        super().__init__(parent)
        self.images_dir = images_dir
        self.poses = poses
        self.calibration = calibration
        self.settings = settings

    def run(self) -> None:
        try:
            import cv2
            import numpy as np

            from core.feature_extractor import FeatureExtractor

            # Resolve core modules relative to this file's parent package
            _mod_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
            if _mod_dir not in os.sys.path:
                os.sys.path.insert(0, _mod_dir)

            from core.feature_extractor import FeatureExtractor

            method = self.settings.get("method", "sift")
            max_features = self.settings.get("max_features", 10000)
            ratio_thresh = self.settings.get("ratio_thresh", 0.75)

            # --- Stage 1: Feature Extraction (10-30%) ---
            self.progress.emit(10, "Starting feature extraction...")

            extractor = FeatureExtractor(method=method, max_features=max_features)

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

            features: dict[str, tuple[list[dict], np.ndarray]] = {}
            total = len(image_paths)
            for idx, path in enumerate(image_paths):
                pct = 10 + int((idx / total) * 20)
                self.progress.emit(pct, f"Extracting features: {idx + 1}/{total}")
                try:
                    kps, desc = extractor.extract(path)
                    features[path] = (kps, desc)
                except ValueError:
                    continue

            self.progress.emit(30, f"Feature extraction complete: {len(features)} images")

            if len(features) < 2:
                raise ValueError(
                    f"Need at least 2 images with features, got {len(features)}"
                )

            # --- Stage 2: Feature Matching (30-50%) ---
            self.progress.emit(32, "Starting feature matching...")

            bf = cv2.BFMatcher() if method == "orb" else cv2.BFMatcher(cv2.NORM_L2)
            matches_all: dict[tuple[str, str], list] = {}
            paths = list(features.keys())

            pair_count = (len(paths) * (len(paths) - 1)) // 2
            pair_idx = 0

            for i in range(len(paths)):
                for j in range(i + 1, len(paths)):
                    pair_idx += 1
                    pct = 32 + int((pair_idx / pair_count) * 18)
                    self.progress.emit(
                        pct,
                        f"Matching pair {pair_idx}/{pair_count}",
                    )

                    desc_a = features[paths[i]][1]
                    desc_b = features[paths[j]][1]

                    if desc_a.size == 0 or desc_b.size == 0:
                        continue

                    raw_matches = bf.knnMatch(desc_a, desc_b, k=2)

                    good = []
                    for m_list in raw_matches:
                        if len(m_list) == 2:
                            m, n = m_list
                            if m.distance < ratio_thresh * n.distance:
                                good.append(m)

                    if len(good) >= 10:
                        matches_all[(paths[i], paths[j])] = good

            self.progress.emit(50, f"Matching complete: {len(matches_all)} pairs")

            # --- Stage 3: Sparse Reconstruction (50-90%) ---
            self.progress.emit(52, "Starting sparse reconstruction...")

            # Build initial camera poses from zip metadata if available
            camera_poses: dict[str, dict] = {}
            if self.poses:
                for pose in self.poses:
                    fname = pose.get("file", "")
                    if fname:
                        camera_poses[fname] = {
                            "position": pose.get("position", [0, 0, 0]),
                            "rotation": pose.get("rotation", [0, 0, 0]),
                        }

            # Simple incremental reconstruction
            points_3d = []
            point_colors = []
            registered_poses: dict[str, dict] = {}

            calibrated = self.calibration is not None and bool(self.calibration)
            fx = self.calibration.get("fx", 1000.0) if calibrated else 1000.0
            fy = self.calibration.get("fy", 1000.0) if calibrated else 1000.0
            cx = self.calibration.get("cx", 640.0) if calibrated else 640.0
            cy = self.calibration.get("cy", 480.0) if calibrated else 480.0
            camera_matrix = np.array([[fx, 0, cx], [0, fy, cy], [0, 0, 1]],
                                     dtype=np.float64)
            dist_coeffs = np.zeros(4, dtype=np.float64)

            total_pairs = max(len(matches_all), 1)
            for pair_idx, (pair, match_list) in enumerate(matches_all.items()):
                pct = 55 + int((pair_idx / total_pairs) * 34)
                self.progress.emit(pct, f"Reconstructing pair {pair_idx + 1}/{total_pairs}")

                img_a, img_b = pair
                kps_a = features[img_a][0]
                kps_b = features[img_b][0]

                pts_a = np.float64([kps_a[m.queryIdx]["pt"] for m in match_list])
                pts_b = np.float64([kps_b[m.trainIdx]["pt"] for m in match_list])

                E, mask = cv2.findEssentialMat(
                    pts_a, pts_b, camera_matrix,
                    method=cv2.RANSAC, prob=0.999, threshold=1.0,
                )
                if E is None or mask is None:
                    continue

                inlier_mask = mask.ravel().astype(bool)
                pts_a = pts_a[inlier_mask]
                pts_b = pts_b[inlier_mask]

                if len(pts_a) < 8:
                    continue

                _, R, t, mask_pose = cv2.recoverPose(
                    E, pts_a, pts_b, camera_matrix
                )

                # Triangulate points
                P1 = camera_matrix @ np.hstack([np.eye(3), np.zeros((3, 1))])
                P2 = camera_matrix @ np.hstack([R, t])
                pts4d = cv2.triangulatePoints(P1, P2, pts_a.T, pts_b.T)
                pts3d = (pts4d[:3] / pts4d[3]).T

                # Keep points in front of both cameras
                valid = (pts4d[3] > 0)
                pts3d = pts3d[valid]

                if len(pts3d) > 0:
                    points_3d.append(pts3d)
                    # Assign mean color placeholder
                    colors = np.full((len(pts3d), 3), 180, dtype=np.uint8)
                    point_colors.append(colors)

                if img_a not in registered_poses:
                    registered_poses[img_a] = {"position": [0, 0, 0],
                                                "rotation": [0, 0, 0]}
                if img_b not in registered_poses:
                    pos = t.ravel().tolist()
                    rot = cv2.Rodrigues(R)[0].ravel().tolist()
                    registered_poses[img_b] = {"position": pos, "rotation": rot}

            if points_3d:
                merged_points = np.vstack(points_3d)
                merged_colors = np.vstack(point_colors)
            else:
                merged_points = np.empty((0, 3), dtype=np.float64)
                merged_colors = np.empty((0, 3), dtype=np.uint8)

            self.progress.emit(90, "Sparse reconstruction complete")

            results = {
                "point_cloud": merged_points,
                "point_colors": merged_colors,
                "camera_poses": registered_poses,
                "features": {
                    path: kps for path, (kps, _) in features.items()
                },
                "matches": {
                    f"{a}->{b}": len(m)
                    for (a, b), m in matches_all.items()
                },
                "calibration": self.calibration,
                "num_images": len(features),
                "num_points": len(merged_points),
            }

            self.progress.emit(100, "SfM pipeline complete")
            self.finished.emit(results)

        except Exception as e:
            tb = traceback.format_exc()
            self.error.emit(f"{e}\n{tb}")
