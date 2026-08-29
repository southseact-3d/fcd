"""Feature detection and descriptor extraction using OpenCV."""

from typing import Callable, Optional

import cv2
import numpy as np


class FeatureExtractor:
    """Extract features from images using SIFT or ORB detectors.

    Args:
        method: Detection method, either "sift" or "orb".
        max_features: Maximum number of features to detect.
    """

    def __init__(self, method: str = "sift", max_features: int = 10000) -> None:
        if method not in ("sift", "orb"):
            raise ValueError(f"Unsupported method '{method}', use 'sift' or 'orb'")

        self.method = method
        self.max_features = max_features

        if method == "sift":
            self._detector = cv2.SIFT_create(nfeatures=max_features)
        else:
            self._detector = cv2.ORB_create(nfeatures=max_features)

    def _keypoint_to_dict(self, kp: cv2.KeyPoint) -> dict:
        """Convert an OpenCV KeyPoint to a serializable dict."""
        return {
            "pt": [float(kp.pt[0]), float(kp.pt[1])],
            "angle": float(kp.angle),
            "response": float(kp.response),
            "size": float(kp.size),
        }

    def extract(self, image_path: str) -> tuple[list[dict], np.ndarray]:
        """Detect features and compute descriptors for a single image.

        Args:
            image_path: Path to the image file.

        Returns:
            Tuple of (keypoints as list of dicts, descriptor numpy array).

        Raises:
            ValueError: If the image cannot be loaded.
        """
        img = cv2.imread(image_path, cv2.IMREAD_GRAYSCALE)
        if img is None:
            raise ValueError(f"Failed to load image: {image_path}")

        keypoints, descriptors = self._detector.detectAndCompute(img, None)

        if descriptors is None:
            descriptors = np.empty((0, 128 if self.method == "sift" else 32), dtype=np.float32)

        kp_dicts = [self._keypoint_to_dict(kp) for kp in keypoints]
        return kp_dicts, descriptors

    def extract_batch(
        self,
        image_paths: list[str],
        progress_cb: Optional[Callable[[float, str], None]] = None,
    ) -> dict[str, tuple[list[dict], np.ndarray]]:
        """Extract features from multiple images.

        Args:
            image_paths: List of image file paths.
            progress_cb: Optional callback(percent, message) for progress updates.

        Returns:
            Dict mapping image path to (keypoints, descriptors) tuple.
        """
        results = {}
        total = len(image_paths)

        for idx, path in enumerate(image_paths):
            if progress_cb:
                pct = (idx / total) * 100.0
                progress_cb(pct, f"Extracting features from image {idx + 1}/{total}")

            try:
                kps, desc = self.extract(path)
                results[path] = (kps, desc)
            except ValueError:
                # Skip images that fail to load
                if progress_cb:
                    progress_cb(
                        (idx / total) * 100.0,
                        f"Skipping unreadable image: {path}",
                    )
                continue

        if progress_cb:
            progress_cb(100.0, f"Feature extraction complete: {len(results)}/{total} images")

        return results
