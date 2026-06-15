"""Feature matching between image pairs using OpenCV."""

from typing import Callable, Optional

import cv2
import numpy as np


class FeatureMatcher:
    """Match descriptors between image pairs using FLANN or brute-force.

    Args:
        method: Matching method, either "flann" or "bf".
        ratio_thresh: Lowe's ratio test threshold.
    """

    def __init__(self, method: str = "flann", ratio_thresh: float = 0.75) -> None:
        if method not in ("flann", "bf"):
            raise ValueError(f"Unsupported method '{method}', use 'flann' or 'bf'")

        self.method = method
        self.ratio_thresh = ratio_thresh
        self._matcher = self._create_matcher(method)

    def _create_matcher(self, method: str) -> cv2.DescriptorMatcher:
        """Create the underlying OpenCV descriptor matcher."""
        if method == "flann":
            # KDTree index for SIFT (float descriptors), LSH for ORB (byte descriptors)
            index_params = dict(algorithm=1, trees=5)  # FLANN_INDEX_KDTREE
            search_params = dict(checks=50)
            return cv2.FlannBasedMatcher(index_params, search_params)
        else:
            return cv2.BFMatcher(cv2.NORM_L2, crossCheck=False)

    def _set_for_descriptors(self, descriptors: np.ndarray) -> None:
        """Reconfigure matcher if descriptor type changes between calls."""
        if descriptors is None or len(descriptors) == 0:
            return

        is_float = descriptors.dtype in (np.float32, np.float64)
        if self.method == "flann" and not is_float:
            # Switch to LSH for binary descriptors
            index_params = dict(table_number=6, key_size=12, multi_probe_level=1)
            search_params = dict(checks=50)
            self._matcher = cv2.FlannBasedMatcher(index_params, search_params)
        elif self.method == "flann" and is_float:
            index_params = dict(algorithm=1, trees=5)
            search_params = dict(checks=50)
            self._matcher = cv2.FlannBasedMatcher(index_params, search_params)

    def match(self, desc1: np.ndarray, desc2: np.ndarray) -> list[dict]:
        """Match descriptors between two images with Lowe's ratio test.

        Args:
            desc1: Descriptors from the first image.
            desc2: Descriptors from the second image.

        Returns:
            List of match dicts with 'queryIdx', 'trainIdx', 'distance'.
        """
        if desc1 is None or desc2 is None:
            return []
        if len(desc1) == 0 or len(desc2) == 0:
            return []

        self._set_for_descriptors(desc1)

        k = min(2, len(desc2))
        if k < 2:
            return []

        raw_matches = self._matcher.knnMatch(desc1, desc2, k=k)

        good = []
        for match_group in raw_matches:
            if len(match_group) == 2:
                m, n = match_group
                if m.distance < self.ratio_thresh * n.distance:
                    good.append(
                        {
                            "queryIdx": m.queryIdx,
                            "trainIdx": m.trainIdx,
                            "distance": float(m.distance),
                        }
                    )

        return good

    def match_pairs(
        self,
        features: dict[str, tuple[list[dict], np.ndarray]],
        pairs: list[tuple[str, str]],
        progress_cb: Optional[Callable[[float, str], None]] = None,
    ) -> dict[tuple[str, str], list[dict]]:
        """Match all image pairs and return the results.

        Args:
            features: Dict mapping image path to (keypoints, descriptors).
            pairs: List of (image_path_1, image_path_2) tuples.
            progress_cb: Optional callback(percent, message).

        Returns:
            Dict mapping (img1, img2) to list of match dicts.
        """
        results = {}
        total = len(pairs)

        for idx, (img1, img2) in enumerate(pairs):
            if progress_cb:
                pct = (idx / total) * 100.0
                progress_cb(pct, f"Matching pair {idx + 1}/{total}")

            if img1 not in features or img2 not in features:
                results[(img1, img2)] = []
                continue

            _, desc1 = features[img1]
            _, desc2 = features[img2]

            matches = self.match(desc1, desc2)
            results[(img1, img2)] = matches

        if progress_cb:
            progress_cb(100.0, f"Matching complete: {len(pairs)} pairs processed")

        return results
