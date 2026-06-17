"""Tungsten CAD in-app updater.

Checks for dev patch builds from GitHub Actions and applies them.
Designed for fast dev iteration — only downloads changed files, not a full installer.
"""

import json
import os
import shutil
import subprocess
import sys
import tempfile
import time
import urllib.request
import urllib.error
import zipfile
from datetime import datetime, timezone
from pathlib import Path


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
GITHUB_REPO = "liamh/tungsten-cad"  # owner/repo
WORKFLOW_NAME = "ci_dev_patch.yml"   # workflow to check for patches
ARTIFACT_PATTERN = "dev-patch-"     # artifact name prefix
BACKUP_DIR_NAME = "update_backup"


def get_install_dir() -> Path:
    """Get the FreeCAD install directory (parent of Mod/Update/)."""
    # src/Mod/Update/updater.py → go up 4 levels to get install root
    update_dir = Path(__file__).parent
    mod_dir = update_dir.parent
    install_dir = mod_dir.parent  # src/
    # For installed builds, we need the actual install dir, not source tree
    # The install dir is where bin/ and Mod/ exist together
    candidate = install_dir.parent
    if (candidate / "bin").is_dir() and (candidate / "Mod").is_dir():
        return candidate
    # Fallback: assume we're running from the source tree, use build output
    return candidate


def get_version_file() -> Path:
    """Get the path to the local version.txt."""
    return get_install_dir() / "version.txt"


def read_local_version() -> dict | None:
    """Read the local version info from version.txt.

    Returns dict with commit_sha, timestamp, branch, or None if not found.
    """
    vfile = get_version_file()
    if not vfile.exists():
        return None

    try:
        with open(vfile) as f:
            content = f.read().strip()
            # Try JSON format first
            try:
                return json.loads(content)
            except json.JSONDecodeError:
                # Fallback: plain SHA file (one line)
                return {"commit_sha": content.splitlines()[0].strip()}
    except Exception:
        return None


def write_local_version(info: dict):
    """Write version info to the local version.txt."""
    vfile = get_version_file()
    try:
        with open(vfile, "w") as f:
            json.dump(info, f, indent=2)
    except Exception as e:
        print(f"Warning: Could not write version file: {e}")


# ---------------------------------------------------------------------------
# GitHub API interaction
# ---------------------------------------------------------------------------
def _gh_api(endpoint: str) -> dict | None:
    """Call GitHub API using gh CLI (preferred) or urllib fallback."""
    # Try gh CLI first
    try:
        result = subprocess.run(
            ["gh", "api", endpoint, "--jq", "."],
            capture_output=True, text=True, timeout=30,
        )
        if result.returncode == 0:
            return json.loads(result.stdout)
    except (FileNotFoundError, subprocess.TimeoutExpired, json.JSONDecodeError):
        pass

    # Fallback to urllib (unauthenticated, rate-limited)
    url = f"https://api.github.com/{endpoint}"
    try:
        req = urllib.request.Request(url, headers={"Accept": "application/vnd.github.v3+json"})
        with urllib.request.urlopen(req, timeout=30) as resp:
            return json.loads(resp.read())
    except (urllib.error.URLError, urllib.error.HTTPError, json.JSONDecodeError) as e:
        print(f"GitHub API error: {e}")
        return None


def get_latest_patch_info() -> dict | None:
    """Get info about the latest dev patch from GitHub Actions.

    Returns dict with:
        run_id: int
        run_number: int
        artifact_id: int
        artifact_name: str
        commit_sha: str
        created_at: str
    """
    # List recent workflow runs
    data = _gh_api(f"repos/{GITHUB_REPO}/actions/workflows/{WORKFLOW_NAME}/runs?per_page=5&status=completed")
    if not data or "workflow_runs" not in data:
        return None

    for run in data["workflow_runs"]:
        if run.get("conclusion") != "success":
            continue

        run_id = run["id"]
        run_number = run["run_number"]
        head_sha = run.get("head_sha", "")
        created_at = run.get("created_at", "")

        # Get artifacts for this run
        artifacts = _gh_api(f"repos/{GITHUB_REPO}/actions/runs/{run_id}/artifacts")
        if not artifacts or "artifacts" not in artifacts:
            continue

        for artifact in artifacts["artifacts"]:
            name = artifact.get("name", "")
            if name.startswith(ARTIFACT_PATTERN) and not name.endswith("-Logs"):
                return {
                    "run_id": run_id,
                    "run_number": run_number,
                    "artifact_id": artifact["id"],
                    "artifact_name": name,
                    "commit_sha": head_sha,
                    "created_at": created_at,
                }

    return None


def download_artifact(artifact_id: int, dest_dir: str) -> str | None:
    """Download an artifact's zip file.

    Returns the path to the downloaded zip, or None on failure.
    """
    # Try gh CLI first
    zip_path = os.path.join(dest_dir, "patch.zip")

    try:
        # Use gh to download the artifact
        result = subprocess.run(
            [
                "gh", "api",
                f"repos/{GITHUB_REPO}/actions/artifacts/{artifact_id}/zip",
                "--jq", ".",
            ],
            capture_output=True, timeout=120,
        )
        if result.returncode == 0:
            with open(zip_path, "wb") as f:
                f.write(result.stdout)
            return zip_path
    except (FileNotFoundError, subprocess.TimeoutExpired):
        pass

    # Fallback to urllib
    url = f"https://api.github.com/repos/{GITHUB_REPO}/actions/artifacts/{artifact_id}/zip"
    try:
        req = urllib.request.Request(url, headers={"Accept": "application/vnd.github.v3+json"})
        with urllib.request.urlopen(req, timeout=120) as resp:
            with open(zip_path, "wb") as f:
                shutil.copyfileobj(resp, f)
        return zip_path
    except (urllib.error.URLError, urllib.error.HTTPError) as e:
        print(f"Download error: {e}")
        return None


# ---------------------------------------------------------------------------
# Patch application
# ---------------------------------------------------------------------------
def backup_files(install_dir: Path, file_list: list[str], run_number: int) -> Path:
    """Back up files before overwriting them.

    Returns the backup directory path.
    """
    backup_dir = install_dir / BACKUP_DIR_NAME / str(run_number)
    backup_dir.mkdir(parents=True, exist_ok=True)

    for rel_path in file_list:
        src = install_dir / rel_path
        if src.exists():
            dst = backup_dir / rel_path
            dst.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(src, dst)

    return backup_dir


def apply_patch(zip_path: str, install_dir: Path) -> tuple[bool, list[str], dict | None]:
    """Apply a patch zip over the install directory.

    Returns (success, list_of_applied_files, version_info).
    """
    applied_files = []
    version_info = None

    try:
        with zipfile.ZipFile(zip_path, "r") as zf:
            # Read version info first
            if "version.txt" in zf.namelist():
                version_info = json.loads(zf.read("version.txt"))

            # Extract all files except version.txt
            for name in zf.namelist():
                if name == "version.txt":
                    continue

                target = install_dir / name
                target.parent.mkdir(parents=True, exist_ok=True)

                # Try direct write, handle locked files
                try:
                    with zf.open(name) as src, open(target, "wb") as dst:
                        shutil.copyfileobj(src, dst)
                    applied_files.append(name)
                except PermissionError:
                    # File is locked — try MoveFileEx for reboot-time replacement
                    if sys.platform == "win32":
                        success = _schedule_reboot_replace(zip_path, name, target)
                        if success:
                            applied_files.append(name + " (pending reboot)")
                        else:
                            print(f"Warning: Could not replace locked file: {name}")
                    else:
                        print(f"Warning: Could not replace: {name}")

    except (zipfile.BadZipFile, json.JSONDecodeError) as e:
        print(f"Patch error: {e}")
        return False, [], None

    return True, applied_files, version_info


def _schedule_reboot_replace(zip_path: str, arc_name: str, target: Path) -> bool:
    """Schedule a file replacement on reboot using MoveFileEx on Windows."""
    try:
        import ctypes
        from ctypes import wintypes

        kernel32 = ctypes.windll.kernel32

        # Extract to a temp location first
        temp_dir = tempfile.mkdtemp(prefix="tungsten_update_")
        temp_file = os.path.join(temp_dir, os.path.basename(target))

        with zipfile.ZipFile(zip_path, "r") as zf:
            with zf.open(arc_name) as src, open(temp_file, "wb") as dst:
                shutil.copyfileobj(src, dst)

        # MOVEFILE_DELAY_UNTIL_REBOOT = 0x4
        # MOVEFILE_REPLACE_EXISTING = 0x1
        flags = 0x4 | 0x1

        result = kernel32.MoveFileExW(
            temp_file,
            str(target),
            wintypes.DWORD(flags),
        )

        if result:
            return True

        # MoveFileEx failed — clean up
        shutil.rmtree(temp_dir, ignore_errors=True)
        return False

    except Exception as e:
        print(f"MoveFileEx failed: {e}")
        return False


def cleanup_old_backups(install_dir: Path, keep_count: int = 5):
    """Remove old backup directories, keeping only the most recent ones."""
    backup_root = install_dir / BACKUP_DIR_NAME
    if not backup_root.exists():
        return

    backups = sorted(
        [d for d in backup_root.iterdir() if d.is_dir()],
        key=lambda d: int(d.name) if d.name.isdigit() else 0,
        reverse=True,
    )

    for old in backups[keep_count:]:
        try:
            shutil.rmtree(old)
        except Exception:
            pass


# ---------------------------------------------------------------------------
# High-level API
# ---------------------------------------------------------------------------
def check_for_updates() -> dict:
    """Check if a dev patch is available.

    Returns dict with:
        available: bool
        current_sha: str | None
        latest_sha: str | None
        run_number: int | None
        created_at: str | None
        message: str
    """
    local_version = read_local_version()
    current_sha = local_version.get("commit_sha") if local_version else None

    patch_info = get_latest_patch_info()

    if not patch_info:
        return {
            "available": False,
            "current_sha": current_sha,
            "latest_sha": None,
            "run_number": None,
            "created_at": None,
            "message": "Could not check for updates (GitHub API unavailable or no builds found).",
        }

    latest_sha = patch_info["commit_sha"]

    if current_sha and current_sha == latest_sha:
        return {
            "available": False,
            "current_sha": current_sha,
            "latest_sha": latest_sha,
            "run_number": patch_info["run_number"],
            "created_at": patch_info["created_at"],
            "message": f"Already up to date (build #{patch_info['run_number']}).",
        }

    return {
        "available": True,
        "current_sha": current_sha,
        "latest_sha": latest_sha,
        "run_number": patch_info["run_number"],
        "created_at": patch_info["created_at"],
        "artifact_id": patch_info["artifact_id"],
        "message": (
            f"Update available: build #{patch_info['run_number']}\n"
            f"Current: {current_sha[:8] if current_sha else 'unknown'}\n"
            f"Latest:  {latest_sha[:8]}"
        ),
    }


def download_and_apply_patch(progress_callback=None) -> tuple[bool, str]:
    """Download the latest patch and apply it.

    Args:
        progress_callback: Optional callable(message: str) for progress updates.

    Returns:
        (success, message)
    """
    def report(msg):
        if progress_callback:
            progress_callback(msg)
        print(msg)

    # Get latest patch info
    patch_info = get_latest_patch_info()
    if not patch_info:
        return False, "Could not find latest patch build on GitHub."

    report(f"Downloading build #{patch_info['run_number']}...")

    # Download to temp directory
    with tempfile.TemporaryDirectory(prefix="tungsten_patch_") as tmp_dir:
        zip_path = download_artifact(patch_info["artifact_id"], tmp_dir)
        if not zip_path:
            return False, "Failed to download patch artifact."

        report("Download complete. Applying patch...")

        install_dir = get_install_dir()

        # Read version info from the zip to know which files changed
        try:
            with zipfile.ZipFile(zip_path, "r") as zf:
                if "version.txt" in zf.namelist():
                    ver = json.loads(zf.read("version.txt"))
                    changed_files = ver.get("files", [])
                else:
                    changed_files = [n for n in zf.namelist() if n != "version.txt"]
        except Exception:
            changed_files = []

        # Back up files that will be overwritten
        if changed_files:
            backup_dir = backup_files(install_dir, changed_files, patch_info["run_number"])
            report(f"Backed up {len(changed_files)} file(s) to {backup_dir}")

        # Apply the patch
        success, applied, version_info = apply_patch(zip_path, install_dir)

        if not success:
            return False, "Failed to extract patch files."

        # Write new version info
        if version_info:
            write_local_version(version_info)

        # Clean up old backups
        cleanup_old_backups(install_dir)

        report(f"Patch applied successfully ({len(applied)} files updated).")

        needs_reboot = any("(pending reboot)" in f for f in applied)
        if needs_reboot:
            msg = (
                "Some files are in use and will be replaced on next reboot.\n"
                "Please restart your computer to complete the update."
            )
        else:
            msg = (
                "Update applied successfully!\n"
                "Restart FreeCAD to use the new version."
            )

        return True, msg
