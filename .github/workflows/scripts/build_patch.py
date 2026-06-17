#!/usr/bin/env python3
"""CI helper: identify changed files, build only their targets, package a patch.

This script is called by the ci_dev_patch.yml workflow. It:
1. Reads the last successful build's commit SHA from an artifact
2. Runs git diff to find changed files since that SHA
3. Maps changed files to CMake targets via target_map.py
4. Builds only those targets
5. Scans the build directory for files with new mtimes (actual outputs)
6. Packages them into patch.zip with version.txt

Usage:
    python build_patch.py --build-dir D:/FC/build/release --last-sha-file last_sha.txt
"""

import argparse
import json
import os
import shutil
import subprocess
import sys
import time
import zipfile
from datetime import datetime, timezone
from pathlib import Path

# Add the scripts directory to path so we can import target_map
sys.path.insert(0, str(Path(__file__).parent))
from target_map import map_changed_files, get_output_files_for_targets


def run(cmd: list[str], cwd: str | None = None, check: bool = True) -> subprocess.CompletedProcess:
    """Run a command and return the result."""
    print(f"  $ {' '.join(cmd)}", file=sys.stderr)
    result = subprocess.run(cmd, cwd=cwd, capture_output=True, text=True)
    if check and result.returncode != 0:
        print(f"FAILED (exit {result.returncode}):", file=sys.stderr)
        print(f"  stdout: {result.stdout}", file=sys.stderr)
        print(f"  stderr: {result.stderr}", file=sys.stderr)
        sys.exit(1)
    return result


def get_last_sha(last_sha_file: str | None, github_run_id: str | None) -> str | None:
    """Get the last successful build's commit SHA.

    Priority:
    1. File-based (last_sha.txt artifact from previous run)
    2. Git log fallback (HEAD~1)
    """
    if last_sha_file and os.path.exists(last_sha_file):
        with open(last_sha_file) as f:
            sha = f.read().strip()
            if sha:
                print(f"Last build SHA from file: {sha}", file=sys.stderr)
                return sha

    # Fallback: use git log
    result = run(["git", "log", "--format=%H", "-1", "HEAD~1"], check=False)
    if result.returncode == 0 and result.stdout.strip():
        sha = result.stdout.strip()
        print(f"Last build SHA from git log: {sha}", file=sys.stderr)
        return sha

    return None


def get_changed_files(since_sha: str | None) -> list[str]:
    """Get list of files changed since the given SHA."""
    if since_sha:
        result = run(
            ["git", "diff", "--name-only", "--diff-filter=ACMR", since_sha, "HEAD"],
            check=False,
        )
    else:
        # No previous SHA — compare HEAD to empty tree (all files)
        result = run(
            ["git", "diff", "--name-only", "--diff-filter=ACMR", "--root"],
            check=False,
        )

    if result.returncode != 0:
        print(f"git diff failed: {result.stderr}", file=sys.stderr)
        return []

    files = [f.strip() for f in result.stdout.splitlines() if f.strip()]
    print(f"Changed files: {len(files)}", file=sys.stderr)
    return files


def build_targets(targets: set[str], build_dir: str, jobs: int = 32) -> bool:
    """Build the specified CMake targets. Returns True on success."""
    if not targets:
        print("No targets to build.", file=sys.stderr)
        return True

    target_list = sorted(targets)
    print(f"\nBuilding {len(target_list)} target(s):", file=sys.stderr)
    for t in target_list:
        print(f"  - {t}", file=sys.stderr)

    # Build all targets in one cmake invocation
    cmd = [
        "cmake", "--build", build_dir,
        "--config", "Release",
        "-j", str(jobs),
        "--target",
    ] + target_list

    result = run(cmd, check=False)
    if result.returncode != 0:
        print(f"Build failed!", file=sys.stderr)
        return False

    print("Build succeeded.", file=sys.stderr)
    return True


def find_changed_outputs(build_dir: str, build_started: float) -> list[Path]:
    """Scan the build directory for files modified after build_started.

    Returns paths relative to the build directory.
    """
    build_path = Path(build_dir)
    changed = []

    for root, dirs, files in os.walk(build_path):
        # Skip hidden dirs and CMakeFiles
        dirs[:] = [d for d in dirs if not d.startswith(".") and d != "CMakeFiles"]

        for fname in files:
            fpath = Path(root) / fname
            # Check if file was modified after build started
            try:
                mtime = fpath.stat().st_mtime
                if mtime >= build_started - 1:  # 1s tolerance for clock skew
                    rel = fpath.relative_to(build_path)
                    changed.append(rel)
            except OSError:
                continue

    return changed


def find_changed_outputs_by_targets(
    build_dir: str,
    targets: set[str],
    build_started: float,
) -> list[Path]:
    """Find outputs that were actually rebuilt, filtered by target list.

    First tries timestamp-based detection, then falls back to known output paths.
    """
    build_path = Path(build_dir)

    # Strategy 1: timestamp-based scan
    all_changed = find_changed_outputs(build_dir, build_started)

    # Filter to only outputs that match our targets
    target_outputs = set(get_output_files_for_targets(targets, ""))

    # Match changed files against known outputs
    matched = []
    for changed_file in all_changed:
        changed_str = str(changed_file).replace("\\", "/")
        for target_output in target_outputs:
            # Check if the changed file matches or is in the same directory
            if changed_str == target_output or changed_str.startswith(target_output.rsplit("/", 1)[0] + "/"):
                matched.append(changed_file)
                break

    if matched:
        print(f"Found {len(matched)} changed output(s) via timestamp scan", file=sys.stderr)
        return matched

    # Strategy 2: fall back to known output paths (check if they exist)
    print("Timestamp scan found no outputs, falling back to known paths", file=sys.stderr)
    known = []
    for output in target_outputs:
        out_path = build_path / output
        if out_path.exists():
            known.append(Path(output))
    return known


def create_patch_zip(
    outputs: list[Path],
    build_dir: str,
    output_path: str,
    commit_sha: str,
    branch: str,
) -> str:
    """Create a patch zip file from the changed outputs.

    The zip preserves directory structure relative to the build directory,
    matching the installed app's layout (bin/, Mod/, lib/).

    Returns the path to the created zip.
    """
    build_path = Path(build_dir)
    zip_path = Path(output_path)

    with zipfile.ZipFile(zip_path, "w", zipfile.ZIP_DEFLATED) as zf:
        for output in outputs:
            source = build_path / output
            if source.exists():
                zf.write(source, output)
                print(f"  Added: {output} ({source.stat().st_size:,} bytes)", file=sys.stderr)

        # Add version.txt
        version_info = {
            "commit_sha": commit_sha,
            "branch": branch,
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "files": [str(o) for o in outputs],
        }
        zf.writestr("version.txt", json.dumps(version_info, indent=2))

    size_mb = zip_path.stat().st_size / (1024 * 1024)
    print(f"\nPatch zip: {zip_path} ({size_mb:.1f} MB, {len(outputs)} files)", file=sys.stderr)
    return str(zip_path)


def save_current_sha(sha: str, output_file: str):
    """Save the current commit SHA for the next run's diff baseline."""
    with open(output_file, "w") as f:
        f.write(sha + "\n")
    print(f"Saved current SHA to {output_file}: {sha}", file=sys.stderr)


def main():
    parser = argparse.ArgumentParser(description="Build a dev patch artifact")
    parser.add_argument("--build-dir", required=True, help="Path to the build directory")
    parser.add_argument("--last-sha-file", help="File containing the last successful build SHA")
    parser.add_argument("--output-dir", required=True, help="Directory to write patch.zip and version.txt")
    parser.add_argument("--sha-output", help="File to write the current SHA for next run")
    parser.add_argument("--jobs", type=int, default=32, help="Parallel build jobs")
    parser.add_argument("--build-gui", action="store_true", default=True,
                        help="Build GUI targets (default: True)")
    parser.add_argument("--no-build-gui", dest="build_gui", action="store_false",
                        help="Skip GUI targets")
    parser.add_argument("--dry-run", action="store_true",
                        help="Only show what would be built, don't build")
    args = parser.parse_args()

    os.makedirs(args.output_dir, exist_ok=True)

    # Get repo root
    result = run(["git", "rev-parse", "--show-toplevel"], check=True)
    repo_root = result.stdout.strip()

    # Get current SHA and branch
    result = run(["git", "rev-parse", "HEAD"], check=True)
    current_sha = result.stdout.strip()

    result = run(["git", "rev-parse", "--abbrev-ref", "HEAD"], check=False)
    branch = result.stdout.strip() if result.returncode == 0 else "unknown"

    print(f"\n{'='*60}", file=sys.stderr)
    print(f"Current SHA:  {current_sha}", file=sys.stderr)
    print(f"Branch:       {branch}", file=sys.stderr)
    print(f"Build dir:    {args.build_dir}", file=sys.stderr)
    print(f"{'='*60}\n", file=sys.stderr)

    # Get last SHA
    last_sha = get_last_sha(args.last_sha_file, None)

    # Get changed files
    changed_files = get_changed_files(last_sha)

    if not changed_files:
        print("No files changed — nothing to build.", file=sys.stderr)
        # Still save the SHA for next time
        save_current_sha(current_sha, args.sha_output)
        # Create an empty patch with just version info
        create_patch_zip([], args.build_dir,
                         os.path.join(args.output_dir, "patch.zip"),
                         current_sha, branch)
        return

    # Map to targets
    targets = map_changed_files(changed_files, build_gui=args.build_gui)

    if not targets:
        print("No build targets identified from changes.", file=sys.stderr)
        save_current_sha(current_sha, args.sha_output)
        create_patch_zip([], args.build_dir,
                         os.path.join(args.output_dir, "patch.zip"),
                         current_sha, branch)
        return

    target_list = sorted(targets)
    print(f"\nTargets to build ({len(target_list)}):", file=sys.stderr)
    for t in target_list:
        print(f"  {t}", file=sys.stderr)

    # Count total targets vs changed targets
    from target_map import MODULE_MAP, THIRDPARTY_MAP
    total_module_targets = sum(1 for a, g, s in MODULE_MAP.values() if a) + \
                           sum(1 for a, g, s in MODULE_MAP.values() if g)
    total_targets = total_module_targets + len(THIRDPARTY_MAP) + 6  # core + execs
    print(f"\nSelective build: {len(target_list)}/{total_targets} targets", file=sys.stderr)

    if args.dry_run:
        print("\n[DRY RUN] Would build:", file=sys.stderr)
        for t in target_list:
            print(f"  {t}", file=sys.stderr)
        return

    # Record build start time
    build_started = time.time()

    # Build
    success = build_targets(targets, args.build_dir, args.jobs)

    if not success:
        print("\nBuild failed — falling back to full build...", file=sys.stderr)
        # Try a full build as fallback
        build_started = time.time()
        cmd = ["cmake", "--build", args.build_dir, "--config", "Release", "-j", str(args.jobs)]
        result = run(cmd, check=False)
        if result.returncode != 0:
            print("Full build also failed!", file=sys.stderr)
            sys.exit(1)
        # After full build, all outputs are "changed"
        all_outputs = find_changed_outputs(args.build_dir, build_started)
        outputs = [o for o in all_outputs if not str(o).startswith("CMakeFiles")]
    else:
        # Find what was actually rebuilt
        outputs = find_changed_outputs_by_targets(args.build_dir, targets, build_started)

    print(f"\nChanged outputs to package: {len(outputs)}", file=sys.stderr)
    for o in outputs:
        print(f"  {o}", file=sys.stderr)

    # Create patch zip
    patch_path = os.path.join(args.output_dir, "patch.zip")
    create_patch_zip(outputs, args.build_dir, patch_path, current_sha, branch)

    # Save current SHA for next run
    if args.sha_output:
        save_current_sha(current_sha, args.sha_output)

    print("\nDone!", file=sys.stderr)


if __name__ == "__main__":
    main()
