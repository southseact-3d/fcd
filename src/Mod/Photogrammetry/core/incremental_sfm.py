"""Incremental Structure from Motion pipeline."""

from __future__ import annotations

from typing import Callable, Optional

import cv2
import numpy as np
from scipy.optimize import least_squares
from scipy.spatial.transform import Rotation

from .zip_handler import CameraCalibration, CameraPose


def _quaternion_to_rotation_matrix(qw: float, qx: float, qy: float, qz: float) -> np.ndarray:
    """Convert a quaternion (w, x, y, z) to a 3x3 rotation matrix."""
    r = Rotation.from_quat([qx, qy, qz, qw])
    return r.as_matrix()


def _rotation_matrix_to_quaternion(R: np.ndarray) -> tuple[float, float, float, float]:
    """Convert a 3x3 rotation matrix to quaternion (w, x, y, z)."""
    r = Rotation.from_matrix(R)
    qx, qy, qz, qw = r.as_quat()
    return (qw, qx, qy, qz)


class IncrementalSfM:
    """Incremental Structure from Motion using calibrated camera poses.

    Args:
        calibration: Camera intrinsics and distortion parameters.
    """

    def __init__(self, calibration: CameraCalibration) -> None:
        self.calibration = calibration
        self.camera_matrix = np.array(calibration.camera_matrix, dtype=np.float64)
        self.dist_coeffs = np.array(calibration.distortion_coefficients, dtype=np.float64)

    def _find_initial_pair(
        self,
        features: dict[str, tuple[list[dict], np.ndarray]],
        matches: dict[tuple[str, str], list[dict]],
    ) -> tuple[str, str, list[dict]]:
        """Find the best initial image pair based on match count and baseline."""
        best_pair = None
        best_count = 0
        best_matches = []

        for (img1, img2), mlist in matches.items():
            count = len(mlist)
            if count > best_count:
                best_count = count
                best_pair = (img1, img2)
                best_matches = mlist

        if best_pair is None:
            raise ValueError("No valid image pairs found for initialization")

        return best_pair[0], best_pair[1], best_matches

    def _estimate_initial_pair(
        self,
        img1: str,
        img2: str,
        features: dict[str, tuple[list[dict], np.ndarray]],
        matches: list[dict],
    ) -> tuple[np.ndarray, np.ndarray, np.ndarray, np.ndarray, list[int]]:
        """Estimate poses for the initial pair using the essential matrix.

        Returns:
            Tuple of (R1, t1, R2, t2, inlier_indices) where t1 is zero.
        """
        kp1_dicts, _ = features[img1]
        kp2_dicts, _ = features[img2]

        pts1 = np.array([kp1_dicts[m["queryIdx"]]["pt"] for m in matches], dtype=np.float64)
        pts2 = np.array([kp2_dicts[m["trainIdx"]]["pt"] for m in matches], dtype=np.float64)

        E, inliers = cv2.findEssentialMat(
            pts1, pts2, self.camera_matrix, method=cv2.RANSAC, prob=0.999, threshold=1.0
        )
        if E is None:
            raise ValueError("Could not estimate essential matrix for initial pair")

        inlier_mask = inliers.ravel().astype(bool)
        inlier_indices = [i for i, v in enumerate(inlier_mask) if v]

        R1, R2, t = cv2.recoverPose(E, pts1[inlier_mask], pts2[inlier_mask], self.camera_matrix)

        # Camera 1 at identity
        R_cam1 = np.eye(3)
        t_cam1 = np.zeros((3, 1))

        # Camera 2
        R_cam2 = R2 @ R1.T
        t_cam2 = -R_cam2 @ t

        return R_cam1, t_cam1, R_cam2, t_cam2, inlier_indices

    def _triangulate_points(
        self,
        R1: np.ndarray,
        t1: np.ndarray,
        R2: np.ndarray,
        t2: np.ndarray,
        pts1: np.ndarray,
        pts2: np.ndarray,
    ) -> np.ndarray:
        """Triangulate points between two views.

        Returns:
            Nx3 array of 3D points.
        """
        P1 = self.camera_matrix @ np.hstack([R1, t1])
        P2 = self.camera_matrix @ np.hstack([R2, t2])

        pts1_h = pts1.T  # 2xN
        pts2_h = pts2.T

        points_4d = cv2.triangulatePoints(P1, P2, pts1_h, pts2_h)
        points_3d = (points_4d[:3] / points_4d[3]).T

        return points_3d

    def _reproject_points(
        self,
        points_3d: np.ndarray,
        R: np.ndarray,
        t: np.ndarray,
    ) -> np.ndarray:
        """Project 3D points into a camera view.

        Returns:
            Nx2 array of 2D image coordinates.
        """
        pts_cam = (R @ points_3d.T + t).T  # Nx3
        pts_2d = (self.camera_matrix @ pts_cam.T).T  # Nx3
        pts_2d = pts_2d[:, :2] / pts_2d[:, 2:3]
        return pts_2d

    def _compute_reprojection_error(
        self,
        points_3d: np.ndarray,
        points_2d: np.ndarray,
        R: np.ndarray,
        t: np.ndarray,
    ) -> float:
        """Compute mean reprojection error for a set of correspondences."""
        if len(points_3d) == 0:
            return 0.0
        projected = self._reproject_points(points_3d, R, t)
        return float(np.mean(np.linalg.norm(projected - points_2d, axis=1)))

    def _bundle_adjust(
        self,
        camera_poses: list[dict],
        point_cloud: np.ndarray,
        observations: dict[str, list[tuple[int, np.ndarray]]],
    ) -> tuple[list[dict], np.ndarray]:
        """Bundle adjustment to refine camera poses and 3D points.

        Args:
            camera_poses: List of dicts with 'image', 'R', 't' keys.
            point_cloud: Nx3 array of 3D points.
            observations: Dict mapping image name -> list of (point_index, 2D coords).

        Returns:
            Updated (camera_poses, point_cloud).
        """
        n_cameras = len(camera_poses)
        n_points = len(point_cloud)

        # Build parameter vector: [cam_params * n_cameras, points * n_points]
        # cam_params: 6 (rodvec 3 + translation 3)
        cam_params = []
        for pose in camera_poses:
            rvec, _ = cv2.Rodrigues(pose["R"])
            cam_params.extend(rvec.flatten().tolist())
            cam_params.extend(pose["t"].flatten().tolist())

        point_params = point_cloud.flatten().tolist()
        x0 = np.array(cam_params + point_params, dtype=np.float64)

        # Build observation lists
        cam_indices = []
        pt_indices = []
        obs_2d = []

        cam_name_to_idx = {p["image"]: i for i, p in enumerate(camera_poses)}

        for img_name, obs_list in observations.items():
            if img_name not in cam_name_to_idx:
                continue
            ci = cam_name_to_idx[img_name]
            for pt_idx, pt_2d in obs_list:
                cam_indices.append(ci)
                pt_indices.append(pt_idx)
                obs_2d.append(pt_2d)

        if len(obs_2d) == 0:
            return camera_poses, point_cloud

        cam_indices = np.array(cam_indices, dtype=np.int32)
        pt_indices = np.array(pt_indices, dtype=np.int32)
        obs_2d = np.array(obs_2d, dtype=np.float64)

        def residuals(params: np.ndarray) -> np.ndarray:
            cam_p = params[: n_cameras * 6].reshape(n_cameras, 6)
            pts = params[n_cameras * 6 :].reshape(n_points, 3)

            res = []
            for i in range(len(obs_2d)):
                ci = cam_indices[i]
                pi = pt_indices[i]

                rvec = cam_p[ci, :3]
                tvec = cam_p[ci, 3:]
                R, _ = cv2.Rodrigues(rvec)

                pt3d = pts[pi:pi+1]
                projected = self._reproject_points(pt3d, R, tvec.reshape(3, 1))
                res.extend((projected[0] - obs_2d[i]).tolist())

            return np.array(res, dtype=np.float64)

        try:
            result = least_squares(
                residuals, x0, method="trf", max_nfev=50
            )
            optimized = result.x

            # Unpack optimized parameters
            cam_p = optimized[: n_cameras * 6].reshape(n_cameras, 6)
            pts = optimized[n_cameras * 6 :].reshape(n_points, 3)

            for i, pose in enumerate(camera_poses):
                rvec = cam_p[i, :3]
                tvec = cam_p[i, 3:].reshape(3, 1)
                R, _ = cv2.Rodrigues(rvec)
                pose["R"] = R
                pose["t"] = tvec

            point_cloud = pts
        except Exception:
            # On failure, return unoptimized
            pass

        return camera_poses, point_cloud

    def run(
        self,
        features: dict[str, tuple[list[dict], np.ndarray]],
        matches: dict[tuple[str, str], list[dict]],
        poses: list[CameraPose],
        progress_cb: Optional[Callable[[float, str], None]] = None,
    ) -> dict:
        """Run the full incremental SfM pipeline.

        Args:
            features: Dict mapping image path to (keypoints, descriptors).
            matches: Dict mapping (img1, img2) to list of match dicts.
            poses: Phone IMU poses used for scale and initial ordering.
            progress_cb: Optional callback(percent, message).

        Returns:
            Dict with point_cloud, point_colors, camera_poses, image_point_map, sparse_matches.
        """
        image_names = sorted(features.keys())
        if len(image_names) < 2:
            raise ValueError("Need at least 2 images for SfM")

        if progress_cb:
            progress_cb(0.0, "Finding initial pair")

        img1, img2, init_matches = self._find_initial_pair(features, matches)

        if progress_cb:
            progress_cb(5.0, f"Initial pair: {img1} and {img2}")

        R1, t1, R2, t2, inlier_idxs = self._estimate_initial_pair(
            img1, img2, features, init_matches
        )

        kp1_dicts, _ = features[img1]
        kp2_dicts, _ = features[img2]

        pts1 = np.array(
            [kp1_dicts[init_matches[i]["queryIdx"]]["pt"] for i in inlier_idxs],
            dtype=np.float64,
        )
        pts2 = np.array(
            [kp2_dicts[init_matches[i]["trainIdx"]]["pt"] for i in inlier_idxs],
            dtype=np.float64,
        )

        points_3d = self._triangulate_points(R1, t1, R2, t2, pts1, pts2)

        # Filter points behind cameras
        valid = []
        for i, pt in enumerate(points_3d):
            proj1 = (R1 @ pt + t1.flatten())[2]
            proj2 = (R2 @ pt + t2.flatten())[2]
            if proj1 > 0 and proj2 > 0:
                valid.append(i)
        points_3d = points_3d[valid]

        # Assign colors (placeholder: gray)
        point_colors = np.full((len(points_3d), 3), 180, dtype=np.uint8)

        # Track which 3D points are visible in which images
        image_point_map: dict[str, list[int]] = {img1: [], img2: []}
        # Rebuild valid index mapping for the initial pair observations
        for local_idx, global_idx in enumerate(valid):
            image_point_map[img1].append(local_idx)
            image_point_map[img2].append(local_idx)

        # Camera poses tracker
        camera_poses: list[dict] = [
            {"image": img1, "R": R1, "t": t1},
            {"image": img2, "R": R2, "t": t2},
        ]

        reconstructed_images = {img1, img2}

        if progress_cb:
            progress_cb(10.0, f"Initial reconstruction: {len(points_3d)} points")

        # Incrementally add remaining images
        remaining = [n for n in image_names if n not in reconstructed_images]

        observations: dict[str, list[tuple[int, np.ndarray]]] = {}
        for idx_i, img_name in enumerate(image_point_map):
            for pt_idx in image_point_map[img_name]:
                if img_name not in observations:
                    observations[img_name] = []
                observations[img_name].append((pt_idx, self._reproject_points(
                    points_3d[pt_idx:pt_idx+1],
                    camera_poses[0 if img_name == img1 else 1]["R"],
                    camera_poses[0 if img_name == img1 else 1]["t"],
                )[0]))

        for step_idx, img_name in enumerate(remaining):
            pct = 10.0 + (step_idx / max(len(remaining), 1)) * 70.0
            if progress_cb:
                progress_cb(pct, f"Adding image {step_idx + 1}/{len(remaining)}: {img_name}")

            # Find 2D-3D correspondences
            kp_dicts, _ = features[img_name]

            # Look for matches to already-reconstructed images
            two_d = []
            three_d = []

            for recon_img in reconstructed_images:
                pair_key = (recon_img, img_name) if (recon_img, img_name) in matches else (img_name, recon_img)
                if pair_key not in matches:
                    continue

                pair_matches = matches[pair_key]
                reverse = pair_key[0] == img_name

                recon_kp_dicts, _ = features[recon_img]
                recon_img_idx_in_poses = next(
                    (i for i, p in enumerate(camera_poses) if p["image"] == recon_img), None
                )
                if recon_img_idx_in_poses is None:
                    continue

                for m in pair_matches:
                    if reverse:
                        query_idx = m["trainIdx"]
                        train_idx = m["queryIdx"]
                    else:
                        query_idx = m["queryIdx"]
                        train_idx = m["trainIdx"]

                    # Check if the point from recon_img is already in point_cloud
                    for pt_idx in image_point_map.get(recon_img, []):
                        # Simple heuristic: find corresponding point
                        if len(two_d) > 0 and len(two_d) == len(three_d):
                            pass
                        break

                    # For now, use direct 2D-2D matching then triangulate
                    two_d.append(kp_dicts[train_idx]["pt"] if not reverse else kp_dicts[query_idx]["pt"])

            if len(two_d) < 6:
                # Not enough correspondences, try matching against all reconstructed
                for recon_img in reconstructed_images:
                    pair_key = (recon_img, img_name) if (recon_img, img_name) in matches else (img_name, recon_img)
                    if pair_key not in matches:
                        continue

                    pair_matches = matches[pair_key]
                    recon_kp_dicts, _ = features[recon_img]

                    for m in pair_matches[:50]:
                        pt_idx = len(points_3d)
                        img_pt = np.array(m["trainIdx" if pair_key[1] == img_name else "queryIdx"], dtype=np.int32)
                        # Skip - we need proper triangulation
                        pass

                if len(two_d) < 6:
                    if progress_cb:
                        progress_cb(pct, f"Skipping {img_name}: insufficient correspondences")
                    continue

            two_d = np.array(two_d[:100], dtype=np.float64)

            # Solve PnP
            if len(three_d) >= 6:
                three_d_arr = np.array(three_d[:100], dtype=np.float64)
                success, rvec, tvec, inliers = cv2.solvePnPRansac(
                    three_d_arr, two_d, self.camera_matrix, self.dist_coeffs,
                    iterationsCount=200, reprojectionError=3.0,
                )
                if success and inliers is not None and len(inliers) >= 6:
                    R_new, _ = cv2.Rodrigues(rvec)
                else:
                    continue
            else:
                # Fallback: find best matching reconstructed pair and triangulate
                best_pair = None
                best_count = 0
                for recon_img in reconstructed_images:
                    pair_key = (recon_img, img_name) if (recon_img, img_name) in matches else (img_name, recon_img)
                    if pair_key in matches and len(matches[pair_key]) > best_count:
                        best_count = len(matches[pair_key])
                        best_pair = (recon_img, pair_key)

                if best_pair is None or best_count < 10:
                    continue

                recon_img, pair_key = best_pair
                pair_matches = matches[pair_key]
                reverse = pair_key[0] == img_name

                recon_idx = next(
                    (i for i, p in enumerate(camera_poses) if p["image"] == recon_img), None
                )
                if recon_idx is None:
                    continue

                recon_kp_dicts, _ = features[recon_img]
                img_kp_dicts, _ = features[img_name]

                src_pts = []
                dst_pts = []
                for m in pair_matches:
                    if reverse:
                        src_pts.append(img_kp_dicts[m["queryIdx"]]["pt"])
                        dst_pts.append(recon_kp_dicts[m["trainIdx"]]["pt"])
                    else:
                        src_pts.append(recon_kp_dicts[m["queryIdx"]]["pt"])
                        dst_pts.append(img_kp_dicts[m["trainIdx"]]["pt"])

                src_pts = np.array(src_pts, dtype=np.float64)
                dst_pts = np.array(dst_pts, dtype=np.float64)

                R_recon = camera_poses[recon_idx]["R"]
                t_recon = camera_poses[recon_idx]["t"]

                # Triangulate with the reconstructed camera and new camera guess
                # Use identity as initial guess for new camera
                R_new = np.eye(3)
                t_new = np.zeros((3, 1))

                # Estimate essential matrix between the two images
                E, inlier_mask = cv2.findEssentialMat(
                    src_pts, dst_pts, self.camera_matrix, method=cv2.RANSAC, threshold=1.0
                )
                if E is None:
                    continue

                inlier_mask = inlier_mask.ravel().astype(bool)
                _, R_new_rel, t_new_rel, _ = cv2.recoverPose(
                    E, src_pts[inlier_mask], dst_pts[inlier_mask], self.camera_matrix
                )

                # Transform to world frame
                R_new = R_new_rel @ R_recon
                t_new = R_recon.T @ (t_new_rel - t_recon)

                # Triangulate new points
                src_inliers = src_pts[inlier_mask]
                dst_inliers = dst_pts[inlier_mask]

                if len(src_inliers) >= 6:
                    new_pts = self._triangulate_points(
                        R_recon, t_recon, R_new, t_new, dst_inliers, src_inliers
                    )
                    # Filter points behind cameras
                    valid_new = []
                    for pt in new_pts:
                        if (R_recon @ pt + t_recon.flatten())[2] > 0 and \
                           (R_new @ pt + t_new.flatten())[2] > 0:
                            valid_new.append(pt)
                    if valid_new:
                        new_pts = np.array(valid_new)
                        start_idx = len(points_3d)
                        points_3d = np.vstack([points_3d, new_pts])
                        point_colors = np.vstack([
                            point_colors,
                            np.full((len(new_pts), 3), 180, dtype=np.uint8),
                        ])
                        new_indices = list(range(start_idx, start_idx + len(new_pts)))
                        image_point_map[img_name] = new_indices
                        for ri, recon_img in enumerate(reconstructed_images):
                            if recon_img not in image_point_map:
                                image_point_map[recon_img] = []
                            image_point_map[recon_img].extend(new_indices)

            camera_poses.append({"image": img_name, "R": R_new, "t": t_new})
            reconstructed_images.add(img_name)

        if progress_cb:
            progress_cb(80.0, "Running bundle adjustment")

        # Build observations for bundle adjustment
        for img_name in image_point_map:
            if img_name not in observations:
                observations[img_name] = []
            cam_idx = next((i for i, p in enumerate(camera_poses) if p["image"] == img_name), None)
            if cam_idx is not None:
                R_c = camera_poses[cam_idx]["R"]
                t_c = camera_poses[cam_idx]["t"]
                for pt_idx in image_point_map[img_name]:
                    if pt_idx < len(points_3d):
                        proj = self._reproject_points(points_3d[pt_idx:pt_idx+1], R_c, t_c)[0]
                        observations[img_name].append((pt_idx, proj))

        camera_poses, points_3d = self._bundle_adjust(
            camera_poses, points_3d, observations
        )

        if progress_cb:
            progress_cb(85.0, "Applying scale from phone poses")

        # Scale using phone IMU positions
        self._apply_scale_from_poses(camera_poses, poses)

        if progress_cb:
            progress_cb(90.0, f"SfM complete: {len(points_3d)} points, {len(camera_poses)} cameras")

        # Build final camera poses list
        final_poses = []
        for pose in camera_poses:
            qw, qx, qy, qz = _rotation_matrix_to_quaternion(pose["R"])
            final_poses.append(CameraPose(
                image_name=pose["image"],
                position=tuple(pose["t"].flatten().tolist()),
                rotation=(qw, qx, qy, qz),
                timestamp=0.0,
            ))

        return {
            "point_cloud": points_3d,
            "point_colors": point_colors,
            "camera_poses": final_poses,
            "image_point_map": image_point_map,
            "sparse_matches": matches,
        }

    def _apply_scale_from_poses(
        self,
        camera_poses: list[dict],
        phone_poses: list[CameraPose],
    ) -> None:
        """Scale the reconstruction using phone IMU positions as reference."""
        if len(phone_poses) < 2 or len(camera_poses) < 2:
            return

        phone_positions = {p.image_name: np.array(p.position) for p in phone_poses}
        sfm_positions = {p["image"]: p["t"].flatten() for p in camera_poses}

        common = set(phone_positions.keys()) & set(sfm_positions.keys())
        if len(common) < 2:
            return

        phone_vecs = []
        sfm_vecs = []
        common_list = sorted(common)

        for i in range(len(common_list) - 1):
            img_a, img_b = common_list[i], common_list[i + 1]
            phone_vecs.append(phone_positions[img_b] - phone_positions[img_a])
            sfm_vecs.append(sfm_positions[img_b] - sfm_positions[img_a])

        phone_vecs = np.array(phone_vecs)
        sfm_vecs = np.array(sfm_vecs)

        phone_norms = np.linalg.norm(phone_vecs, axis=1)
        sfm_norms = np.linalg.norm(sfm_vecs, axis=1)

        valid = (phone_norms > 1e-6) & (sfm_norms > 1e-6)
        if np.sum(valid) < 1:
            return

        scale = np.median(phone_norms[valid] / sfm_norms[valid])

        for pose in camera_poses:
            pose["t"] = pose["t"] * scale
