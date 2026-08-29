"""Handles zip file extraction, validation, and metadata parsing for scan projects."""

import json
import shutil
import tempfile
import zipfile
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any


@dataclass
class CameraPose:
    """Per-image estimated camera pose."""

    image_name: str
    position: tuple[float, float, float]
    rotation: tuple[float, float, float, float]
    timestamp: float


@dataclass
class CameraCalibration:
    """Phone camera intrinsics."""

    camera_matrix: list[list[float]]
    distortion_coefficients: list[float]
    image_size: tuple[int, int]
    make: str
    model: str


@dataclass
class ScanProject:
    """Complete scan project container."""

    images_dir: Path
    poses: list[CameraPose]
    calibration: CameraCalibration
    project_metadata: dict[str, Any]
    image_paths: list[Path]


def validate_zip(zip_path: str) -> bool:
    """Check that a zip file has the required scan project structure.

    Args:
        zip_path: Path to the zip file.

    Returns:
        True if the zip contains the required files and directories.

    Raises:
        FileNotFoundError: If the zip file does not exist.
        ValueError: If the zip is corrupt or missing required entries.
    """
    path = Path(zip_path)
    if not path.exists():
        raise FileNotFoundError(f"Zip file not found: {zip_path}")

    try:
        with zipfile.ZipFile(path, "r") as zf:
            names = zf.namelist()
    except zipfile.BadZipFile as exc:
        raise ValueError(f"Corrupt or invalid zip file: {zip_path}") from exc

    required_files = {"poses.json", "calibration.json", "project.json"}
    found_files = {Path(n).name for n in names}

    if not required_files.issubset(found_files):
        missing = required_files - found_files
        raise ValueError(f"Zip missing required files: {', '.join(sorted(missing))}")

    has_images = any(n.startswith("images/") and not Path(n).is_dir() for n in names)
    if not has_images:
        raise ValueError("Zip contains no images/ directory entries")

    return True


def extract_zip(zip_path: str, output_dir: str) -> str:
    """Extract a zip file to a temporary directory.

    Args:
        zip_path: Path to the zip file.
        output_dir: Parent directory for extraction.

    Returns:
        Path to the extracted project root directory.
    """
    path = Path(zip_path)
    if not path.exists():
        raise FileNotFoundError(f"Zip file not found: {zip_path}")

    dest = Path(output_dir)
    dest.mkdir(parents=True, exist_ok=True)

    with zipfile.ZipFile(path, "r") as zf:
        zf.extractall(dest)

    return str(dest)


def load_poses(zip_path: str) -> list[CameraPose]:
    """Parse poses.json from the zip archive.

    Args:
        zip_path: Path to the zip file.

    Returns:
        List of CameraPose objects.
    """
    with zipfile.ZipFile(zip_path, "r") as zf:
        with zf.open("poses.json") as f:
            data = json.load(f)

    poses = []
    for entry in data["poses"]:
        poses.append(
            CameraPose(
                image_name=entry["image"],
                position=tuple(entry["position"]),
                rotation=tuple(entry["rotation"]),
                timestamp=entry["timestamp"],
            )
        )
    return poses


def load_calibration(zip_path: str) -> CameraCalibration:
    """Parse calibration.json from the zip archive.

    Args:
        zip_path: Path to the zip file.

    Returns:
        CameraCalibration object.
    """
    with zipfile.ZipFile(zip_path, "r") as zf:
        with zf.open("calibration.json") as f:
            data = json.load(f)

    return CameraCalibration(
        camera_matrix=data["camera_matrix"],
        distortion_coefficients=data["distortion_coefficients"],
        image_size=tuple(data["image_size"]),
        make=data.get("make", ""),
        model=data.get("model", ""),
    )


def load_project_metadata(zip_path: str) -> dict[str, Any]:
    """Parse project.json from the zip archive.

    Args:
        zip_path: Path to the zip file.

    Returns:
        Raw project metadata dictionary.
    """
    with zipfile.ZipFile(zip_path, "r") as zf:
        with zf.open("project.json") as f:
            return json.load(f)


def load_scan_project(zip_path: str) -> ScanProject:
    """Full load pipeline: validate, extract, and parse a scan project zip.

    Args:
        zip_path: Path to the zip file.

    Returns:
        Populated ScanProject dataclass.

    Raises:
        FileNotFoundError: If the zip does not exist.
        ValueError: If validation fails or required data is missing.
    """
    path = Path(zip_path)
    if not path.exists():
        raise FileNotFoundError(f"Zip file not found: {zip_path}")

    validate_zip(zip_path)

    tmp_dir = tempfile.mkdtemp(prefix="scan_project_")
    try:
        extract_zip(zip_path, tmp_dir)

        # Determine the actual root inside the extracted directory.
        # The zip may contain a top-level folder or files directly.
        extract_path = Path(tmp_dir)
        subdirs = [d for d in extract_path.iterdir() if d.is_dir() and d.name != "__MACOSX"]
        if len(subdirs) == 1 and (subdirs[0] / "poses.json").exists():
            root = subdirs[0]
        elif (extract_path / "poses.json").exists():
            root = extract_path
        else:
            # Search one level deeper
            for candidate in extract_path.rglob("poses.json"):
                root = candidate.parent
                break
            else:
                raise ValueError("Could not locate project root after extraction")

        poses = load_poses(str(Path(zip_path)))
        calibration = load_calibration(str(Path(zip_path)))
        metadata = load_project_metadata(str(zip_path))

        images_dir = root / "images"
        if not images_dir.exists():
            raise ValueError(f"Images directory not found: {images_dir}")

        image_paths = sorted(
            p for p in images_dir.iterdir()
            if p.suffix.lower() in (".jpg", ".jpeg", ".png", ".tiff", ".tif", ".bmp")
        )

        return ScanProject(
            images_dir=images_dir,
            poses=poses,
            calibration=calibration,
            project_metadata=metadata,
            image_paths=image_paths,
        )
    except Exception:
        shutil.rmtree(tmp_dir, ignore_errors=True)
        raise
