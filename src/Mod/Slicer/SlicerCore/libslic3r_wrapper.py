"""
Wrapper around PrusaSlicer CLI for FDM slicing via libslic3r.

DEPRECATED: Tungsten CAD no longer uses PrusaSlicer as a slicing engine. The
built-in FDM engine in :mod:`SlicerCore.fdm_slicer` is used exclusively. This
module is retained so that any third-party code or tests importing it do not
break, but it is no longer invoked by the Slicer workbench and will be removed
in a future release.

License: LGPL-2.1-or-later
"""

from __future__ import annotations

import logging
import os
import platform
import re
import shutil
import subprocess
import tempfile
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any, Callable, Dict, List, Optional, Tuple

logger = logging.getLogger(__name__)

# ---------------------------------------------------------------------------
# Settings mapping: our internal names → PrusaSlicer CLI flags / INI keys
# ---------------------------------------------------------------------------

SETTING_MAP: Dict[str, str] = {
    "layer_height": "layer_height",
    "first_layer_height": "first_layer_height",
    "infill_percentage": "fill_density",
    "infill_pattern": "fill_pattern",
    "perimeter_count": "perimeters",
    "top_solid_layers": "top_solid_layers",
    "bottom_solid_layers": "bottom_solid_layers",
    "print_speed": "perimeter_speed",
    "external_perimeter_speed": "external_perimeter_speed",
    "infill_speed": "infill_speed",
    "first_layer_speed": "first_layer_speed",
    "travel_speed": "travel_speed",
    "temperature": "temperature",
    "first_layer_temperature": "first_layer_temperature",
    "bed_temperature": "bed_temperature",
    "first_layer_bed_temperature": "first_layer_bed_temperature",
    "support": "support_enable",
    "support_threshold": "support_threshold_angle",
    "support_density": "support_material_density",
    "support_pattern": "support_material_pattern",
    "raft_layers": "raft_layers",
    "brim_width": "brim_width",
    "skirt_loops": "skirt_loops",
    "skirt_distance": "skirt_distance",
    "filament_diameter": "filament_diameter",
    "nozzle_diameter": "nozzle_diameter",
    "retraction_length": "retraction_length",
    "retraction_speed": "retraction_speed",
    "max_retraction_count": "retract_length_toolchange",
    "cooling": "enable_fan",
    "fan_speed": "fan_always_on",
    "min_fan_speed": "min_fan_speed",
    "max_fan_speed": "max_fan_speed",
    "bridge_fan_speed": "bridge_fan_speed",
    "overhang_fan_speed": "overhang_fan_speed",
    "slowdown_below_layer_time": "slowdown_below_layer_time",
    "min_print_speed": "min_print_speed",
    "skirts": "skirts",
    "wipe_tower": "wipe_tower",
    "wipe_tower_width": "wipe_tower_width",
    "extrusion_width": "extrusion_width",
    "solid_infill_extrusion_width": "solid_infill_extrusion_width",
    "top_infill_extrusion_width": "top_infill_extrusion_width",
    "perimeter_extrusion_width": "perimeter_extrusion_width",
    "support_material_extrusion_width": "support_material_extrusion_width",
    "ironing": "ironing",
    "ironing_speed": "ironing_speed",
    "ironing_flowrate": "ironing_flowrate",
    "ironing_spacing": "ironing_spacing",
    "fuzzy_skin": "fuzzy_skin",
    "fuzzy_skin_thickness": "fuzzy_skin_thickness",
    "fuzzy_skin_noise": "fuzzy_skin_noise_distance",
    "seam_position": "seam_position",
    "vase_mode": "spiral_vase",
    "thin_walls": "thin_walls",
    "detect_thin_walls": "detect_thin_walls",
    "xy_size_compensation": "xy_size_compensation",
    "elephant_foot_compensation": "elephant_foot_compensation",
    "gcode_comments": "gcode_comments",
    "gcode_flavor": "gcode_flavor",
    "start_gcode": "start_gcode",
    "end_gcode": "end_gcode",
    "before_layer_gcode": "before_layer_gcode",
    "toolchange_gcode": "toolchange_gcode",
    "color": "filament_colour",
}

# Boolean flags that map to --flag / --no-flag pairs
BOOLEAN_FLAGS: Dict[str, str] = {
    "support": "support_enable",
    "cooling": "enable_fan",
    "ironing": "ironing",
    "fuzzy_skin": "fuzzy_skin",
    "thin_walls": "thin_walls",
    "detect_thin_walls": "detect_thin_walls",
    "wipe_tower": "wipe_tower",
    "vase_mode": "spiral_vase",
}

# Values that need special translation before being passed to PrusaSlicer
PATTERN_MAP: Dict[str, Dict[str, str]] = {
    "infill_pattern": {
        "grid": "grid",
        "lines": "lines",
        "triangles": "triangles",
        "stars": "stars",
        "cubic": "cubic",
        "line": "line",
        "concentric": "concentric",
        "honeycomb": "honeycomb",
        "gyroid": "gyroid",
        "hilbertcurve": "hilbertcurve",
        "supportcubic": "supportcubic",
        "lightning": "lightning",
        "3dhoneycomb": "3dhoneycomb",
        "adaptivecubic": "adaptivecubic",
        "rectilinear": "rectilinear",
    },
    "seam_position": {
        "nearest": "nearest",
        "random": "random",
        "aligned": "aligned",
        "rear": "rear",
    },
    "support_pattern": {
        "grid": "grid",
        "lines": "lines",
        "concentric": "concentric",
        "honeycomb": "honeycomb",
        "pillars": "pillars",
    },
    "gcode_flavor": {
        "reprap": "RepRap",
        "marlin": "Marlin",
        "sprinter": "Sprinter",
        "repetier": "Repetier",
        "teacup": "Teacup",
        "makerbot": "Makerbot",
        "sailfish": "Sailfish",
        "mach3": "Mach3",
        "linuxcnc": "LinuxCNC",
        "millipede": "Millipede",
        "no_extrusion": "No extrusion",
        "klipper": "Klipper",
    },
}

# ---------------------------------------------------------------------------
# SliceResult
# ---------------------------------------------------------------------------


@dataclass
class SliceResult:
    """Result returned after a successful slice operation."""

    success: bool = False
    gcode: str = ""
    gcode_path: str = ""
    print_time: float = 0.0
    material_used: float = 0.0
    material_weight: float = 0.0
    layer_count: int = 0
    filament_usage: float = 0.0
    total_moves: int = 0
    output_dir: str = ""
    warnings: List[str] = field(default_factory=list)
    errors: List[str] = field(default_factory=list)
    raw_output: str = ""
    raw_errors: str = ""

    # -- convenience helpers -------------------------------------------------

    def get_gcode(self) -> str:
        """Read and return the full G-code string from the output file."""
        if self.gcode:
            return self.gcode
        if self.gcode_path and os.path.isfile(self.gcode_path):
            with open(self.gcode_path, "r", encoding="utf-8", errors="replace") as fh:
                self.gcode = fh.read()
            return self.gcode
        return ""


# ---------------------------------------------------------------------------
# LibSlic3rWrapper
# ---------------------------------------------------------------------------


class LibSlic3rWrapper:
    """Wraps PrusaSlicer's CLI as a subprocess to provide FDM slicing.

    Detects a PrusaSlicer installation, translates application settings
    into PrusaSlicer-compatible parameters, invokes the slicer, and
    parses the result.
    """

    # Common installation directories per platform
    _WINDOWS_SEARCH_DIRS: List[str] = [
        os.path.expandvars(r"%PROGRAMFILES%\PrusaSlicer"),
        os.path.expandvars(r"%PROGRAMFILES%\PrusaSlicer\bin"),
        os.path.expandvars(r"%LOCALAPPDATA%\PrusaSlicer"),
        os.path.expandvars(r"%LOCALAPPDATA%\PrusaSlicer\bin"),
        os.path.expandvars(r"%PROGRAMFILES%\Prusa Research"),
        os.path.expandvars(r"%PROGRAMFILES%\Prusa Research\PrusaSlicer"),
        os.path.expandvars(r"%PROGRAMFILES%\Prusa Research\PrusaSlicer\bin"),
        r"C:\PrusaSlicer",
        r"C:\PrusaSlicer\bin",
    ]

    _LINUX_SEARCH_DIRS: List[str] = [
        "/usr/bin",
        "/usr/local/bin",
        "/usr/sbin",
        "/usr/local/sbin",
        "/opt/prusaslicer/bin",
        "/snap/prusaslicer/current/usr/bin",
        "/var/lib/flatpak/app/com.prusa3d.PrusaSlicer/current/active/files/bin",
        os.path.expanduser("~/.local/bin"),
        os.path.expanduser("~/AppImage"),
    ]

    _MACOS_SEARCH_DIRS: List[str] = [
        "/Applications/PrusaSlicer.app/Contents/MacOS",
        "/Applications/PrusaSlicer/PrusaSlicer.app/Contents/MacOS",
        os.path.expanduser("~/Applications/PrusaSlicer.app/Contents/MacOS"),
    ]

    _LINUX_BINARY_NAMES: List[str] = [
        "prusaslicer",
        "PrusaSlicer",
        "prusa-slicer",
        "PrusaSlicer-nightly",
        "prusaslicer-cli",
    ]

    _MACOS_BINARY_NAMES: List[str] = [
        "PrusaSlicer",
        "prusaslicer",
    ]

    # ------------------------------------------------------------------ init

    def __init__(self, prusa_slicer_path: Optional[str] = None) -> None:
        self._prusa_slicer_path: Optional[str] = None
        self._version: Optional[str] = None

        if prusa_slicer_path:
            if os.path.isfile(prusa_slicer_path):
                self._prusa_slicer_path = prusa_slicer_path
                logger.info(
                    "Using user-provided PrusaSlicer path: %s", prusa_slicer_path
                )
            else:
                logger.warning(
                    "User-provided path does not exist: %s – will attempt auto-detect",
                    prusa_slicer_path,
                )

        if not self._prusa_slicer_path:
            self._prusa_slicer_path = self._detect_binary()

        if self._prusa_slicer_path:
            logger.info("PrusaSlicer detected at: %s", self._prusa_slicer_path)
        else:
            logger.warning("PrusaSlicer binary not found on this system")

    # --------------------------------------------------------------- public

    def is_available(self) -> bool:
        """Return True if PrusaSlicer binary is accessible."""
        if not self._prusa_slicer_path:
            return False
        return os.path.isfile(self._prusa_slicer_path) and os.access(
            self._prusa_slicer_path, os.X_OK
        )

    def get_version(self) -> str:
        """Return the PrusaSlicer version string (cached after first call)."""
        if self._version:
            return self._version
        if not self.is_available():
            return "unknown"
        try:
            proc = subprocess.run(
                [self._prusa_slicer_path, "--version"],
                capture_output=True,
                text=True,
                timeout=15,
            )
            version_line = proc.stdout.strip() or proc.stderr.strip()
            self._version = version_line.splitlines()[0] if version_line else "unknown"
        except Exception as exc:
            logger.debug("Failed to query PrusaSlicer version: %s", exc)
            self._version = "unknown"
        return self._version

    def slice(
        self,
        mesh_path: str,
        settings: Dict[str, Any],
        output_dir: Optional[str] = None,
        progress_callback: Optional[Callable[[float], None]] = None,
    ) -> SliceResult:
        """Slice *mesh_path* using the supplied *settings* dictionary.

        Parameters
        ----------
        mesh_path:
            Path to an STL/3MF/OBJ file.
        settings:
            Application-level settings dict (keys from ``SETTING_MAP``).
        output_dir:
            Directory where G-code and temporary config files are written.
            If ``None`` a temporary directory is created.
        progress_callback:
            Optional callback invoked with a float 0.0-1.0 as progress
            is parsed from the slicer's stderr.

        Returns
        -------
        SliceResult
        """
        if not self.is_available():
            return SliceResult(
                success=False,
                errors=["PrusaSlicer is not installed or not accessible"],
            )

        mesh_path = os.path.abspath(mesh_path)
        if not os.path.isfile(mesh_path):
            return SliceResult(
                success=False,
                errors=[f"Mesh file not found: {mesh_path}"],
            )

        if output_dir is None:
            output_dir = tempfile.mkdtemp(prefix="tungsten_slice_")
        os.makedirs(output_dir, exist_ok=True)

        # Build a PrusaSlicer-compatible INI config
        config_content = self.export_config_from_settings(settings)
        config_path = os.path.join(output_dir, "slicer_config.ini")
        with open(config_path, "w", encoding="utf-8") as fh:
            fh.write(config_content)

        return self.slice_with_config(
            mesh_path,
            config_path,
            output_dir,
            progress_callback=progress_callback,
        )

    def slice_with_config(
        self,
        mesh_path: str,
        config_path: str,
        output_dir: Optional[str] = None,
        progress_callback: Optional[Callable[[float], None]] = None,
    ) -> SliceResult:
        """Slice *mesh_path* using an existing PrusaSlicer INI *config_path*.

        Parameters
        ----------
        mesh_path:
            Path to mesh file (STL / 3MF / OBJ).
        config_path:
            Path to a PrusaSlicer-format INI configuration file.
        output_dir:
            Output directory.  A temp dir is created if ``None``.
        progress_callback:
            Optional progress callback (0.0 → 1.0).

        Returns
        -------
        SliceResult
        """
        if not self.is_available():
            return SliceResult(
                success=False,
                errors=["PrusaSlicer is not installed or not accessible"],
            )

        mesh_path = os.path.abspath(mesh_path)
        config_path = os.path.abspath(config_path)

        if not os.path.isfile(mesh_path):
            return SliceResult(
                success=False,
                errors=[f"Mesh file not found: {mesh_path}"],
            )
        if not os.path.isfile(config_path):
            return SliceResult(
                success=False,
                errors=[f"Config file not found: {config_path}"],
            )

        if output_dir is None:
            output_dir = tempfile.mkdtemp(prefix="tungsten_slice_")
        os.makedirs(output_dir, exist_ok=True)

        gcode_output = os.path.join(output_dir, "output.gcode")

        cmd: List[str] = [
            self._prusa_slicer_path,
            "--export-gcode",
            "--load",
            config_path,
            "--output",
            gcode_output,
            mesh_path,
        ]

        logger.info("Running: %s", " ".join(cmd))

        result = self._run_command(cmd, progress_callback=progress_callback)

        # Locate the generated G-code file – PrusaSlicer may append
        # extra characters to the filename depending on the version.
        gcode_path = gcode_output
        if not os.path.isfile(gcode_path):
            candidates = [f for f in os.listdir(output_dir) if f.endswith(".gcode")]
            if candidates:
                gcode_path = os.path.join(output_dir, candidates[0])
            else:
                gcode_path = ""

        slice_result = SliceResult(
            success=result["success"],
            gcode_path=gcode_path,
            output_dir=output_dir,
            raw_output=result["stdout"],
            raw_errors=result["stderr"],
        )

        if gcode_path and os.path.isfile(gcode_path):
            try:
                with open(gcode_path, "r", encoding="utf-8", errors="replace") as fh:
                    slice_result.gcode = fh.read()
                self._parse_gcode_stats(slice_result)
            except Exception as exc:
                logger.warning("Failed to read G-code output: %s", exc)
                slice_result.warnings.append(f"Could not read G-code: {exc}")

        self._parse_output_messages(result["stderr"], slice_result)

        return slice_result

    def get_profiles(self) -> Dict[str, List[str]]:
        """Return available printer, material, and print quality profiles.

        Returns
        -------
        dict
            ``{"printers": [...], "materials": [...], "print_profiles": [...]}``
        """
        profiles: Dict[str, List[str]] = {
            "printers": [],
            "materials": [],
            "print_profiles": [],
        }
        if not self.is_available():
            return profiles

        for category, flag in [
            ("printers", "--list-printers"),
            ("materials", "--list-filaments"),
            ("print_profiles", "--list-print-profiles"),
        ]:
            try:
                proc = subprocess.run(
                    [self._prusa_slicer_path, flag],
                    capture_output=True,
                    text=True,
                    timeout=30,
                )
                output = proc.stdout.strip() or proc.stderr.strip()
                profiles[category] = [
                    line.strip()
                    for line in output.splitlines()
                    if line.strip() and not line.startswith("=")
                ]
            except Exception as exc:
                logger.debug("Failed to list %s profiles: %s", category, exc)

        return profiles

    # -------------------------------------------------- config generation

    def export_config_from_settings(self, settings: Dict[str, Any]) -> str:
        """Convert an application settings dict into a PrusaSlicer INI string."""
        ini_lines: List[str] = ["# Auto-generated by Tungsten CAD", ""]

        # Always include a known section header so the INI is valid
        ini_lines.append("[print:default]")
        ini_lines.append("[filament:default]")
        ini_lines.append("[printer:default]")
        ini_lines.append("")

        ini_lines.append("[print:default]")
        for our_key, value in settings.items():
            if our_key not in SETTING_MAP:
                continue
            ini_key = SETTING_MAP[our_key]
            translated = self._translate_value(our_key, value)
            if translated is not None:
                ini_lines.append(f"{ini_key} = {translated}")

        ini_lines.append("")
        ini_lines.append("[filament:default]")
        filament_keys = {
            "temperature",
            "first_layer_temperature",
            "bed_temperature",
            "first_layer_bed_temperature",
            "filament_diameter",
            "retraction_length",
            "retraction_speed",
            "cooling",
            "fan_speed",
            "min_fan_speed",
            "max_fan_speed",
            "bridge_fan_speed",
            "overhang_fan_speed",
            "slowdown_below_layer_time",
            "min_print_speed",
            "color",
        }
        for our_key in filament_keys:
            if our_key in settings:
                ini_key = SETTING_MAP.get(our_key, our_key)
                translated = self._translate_value(our_key, settings[our_key])
                if translated is not None:
                    ini_lines.append(f"{ini_key} = {translated}")

        ini_lines.append("")
        ini_lines.append("[printer:default]")
        printer_keys = {
            "nozzle_diameter",
            "extrusion_width",
            "retraction_length",
            "retraction_speed",
            "max_retraction_count",
            "support",
            "support_threshold",
            "support_density",
            "support_pattern",
            "raft_layers",
            "brim_width",
            "skirt_loops",
            "skirt_distance",
            "gcode_flavor",
            "start_gcode",
            "end_gcode",
            "before_layer_gcode",
            "toolchange_gcode",
        }
        for our_key in printer_keys:
            if our_key in settings:
                ini_key = SETTING_MAP.get(our_key, our_key)
                translated = self._translate_value(our_key, settings[our_key])
                if translated is not None:
                    ini_lines.append(f"{ini_key} = {translated}")

        return "\n".join(ini_lines) + "\n"

    def export_config(
        self,
        printer: str = "default",
        material: str = "default",
        settings: Optional[Dict[str, Any]] = None,
    ) -> str:
        """Generate a PrusaSlicer INI config string with named profiles.

        Parameters
        ----------
        printer:
            Printer profile name (or ``"default"``).
        material:
            Material / filament profile name (or ``"default"``).
        settings:
            Additional overrides merged on top of the profile defaults.

        Returns
        -------
        str
            INI-format configuration content.
        """
        lines: List[str] = [
            "# Auto-generated by Tungsten CAD",
            f"# Printer: {printer}  Material: {material}",
            "",
        ]

        lines.append(f"[print:{printer}]")
        if settings:
            for our_key, value in settings.items():
                if our_key in SETTING_MAP:
                    ini_key = SETTING_MAP[our_key]
                    translated = self._translate_value(our_key, value)
                    if translated is not None:
                        lines.append(f"{ini_key} = {translated}")
        lines.append("")

        lines.append(f"[filament:{material}]")
        lines.append("")

        lines.append(f"[printer:{printer}]")
        lines.append("")

        return "\n".join(lines) + "\n"

    # -------------------------------------------------- binary detection

    def _detect_binary(self) -> Optional[str]:
        """Attempt to locate the PrusaSlicer binary on this system."""
        system = platform.system()

        # 1. Check PATH first
        for name in self._get_binary_names():
            found = shutil.which(name)
            if found:
                logger.debug("Found PrusaSlicer on PATH: %s", found)
                return found

        # 2. Search well-known directories
        if system == "Windows":
            search_dirs = self._WINDOWS_SEARCH_DIRS
        elif system == "Darwin":
            search_dirs = self._MACOS_SEARCH_DIRS
        else:
            search_dirs = self._LINUX_SEARCH_DIRS

        for search_dir in search_dirs:
            result = self._search_directory(search_dir)
            if result:
                return result

        # 3. On Linux also check snap / flatpak data dirs via runtimes
        if system == "Linux":
            result = self._check_flatpak()
            if result:
                return result

        return None

    def _get_binary_names(self) -> List[str]:
        system = platform.system()
        if system == "Windows":
            return ["PrusaSlicer.exe", "PrusaSlicer-console.exe", "prusa-slicer.exe"]
        if system == "Darwin":
            return self._MACOS_BINARY_NAMES
        return self._LINUX_BINARY_NAMES

    def _search_directory(self, directory: str) -> Optional[str]:
        """Recursively search *directory* for a PrusaSlicer binary (depth ≤ 3)."""
        if not os.path.isdir(directory):
            return None
        binary_names = self._get_binary_names()
        max_depth = 3

        def _walk(path: str, depth: int) -> Optional[str]:
            if depth > max_depth:
                return None
            try:
                entries = os.scandir(path)
            except PermissionError:
                return None
            for entry in entries:
                if entry.is_file(follow_symlinks=True):
                    if entry.name in binary_names:
                        candidate = entry.path
                        if os.access(candidate, os.X_OK):
                            return candidate
                elif entry.is_dir(follow_symlinks=True):
                    found = _walk(entry.path, depth + 1)
                    if found:
                        return found
            return None

        return _walk(directory, 0)

    def _check_flatpak(self) -> Optional[str]:
        """Attempt to invoke PrusaSlicer through the flatpak runtime."""
        try:
            proc = subprocess.run(
                ["flatpak", "run", "com.prusa3d.PrusaSlicer", "--version"],
                capture_output=True,
                text=True,
                timeout=15,
            )
            if proc.returncode == 0:
                return "flatpak:com.prusa3d.PrusaSlicer"
        except Exception:
            pass
        return None

    # -------------------------------------------------- subprocess handling

    def _run_command(
        self,
        cmd: List[str],
        timeout: int = 600,
        progress_callback: Optional[Callable[[float], None]] = None,
    ) -> Dict[str, Any]:
        """Run *cmd* as a subprocess, streaming stderr for progress info.

        Returns
        -------
        dict
            ``{"success": bool, "stdout": str, "stderr": str, "returncode": int}``
        """
        stdout_chunks: List[str] = []
        stderr_chunks: List[str] = []
        last_progress: float = 0.0

        try:
            # Ensure we don't inherit handles on Windows
            startupinfo = None
            if platform.system() == "Windows":
                startupinfo = subprocess.STARTUPINFO()
                startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW
                startupinfo.wShowWindow = 0  # SW_HIDE

            proc = subprocess.Popen(
                cmd,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                startupinfo=startupinfo,
            )

            deadline = time.monotonic() + timeout
            # Read stderr in a non-blocking fashion for progress updates
            # while waiting for the process to complete.
            import select as _select

            while proc.poll() is None:
                if time.monotonic() > deadline:
                    proc.kill()
                    return {
                        "success": False,
                        "stdout": "".join(stdout_chunks),
                        "stderr": "".join(stderr_chunks) + "\nTimeout exceeded",
                        "returncode": -1,
                    }

                # On Windows select.select does not work on pipes, so we
                # fall back to a short blocking read on stderr.
                if platform.system() == "Windows" and proc.stderr is not None:
                    import msvcrt  # type: ignore[import-untyped]
                    import ctypes

                    kernel32 = ctypes.windll.kernel32  # type: ignore[attr-defined]
                    # Peek to avoid blocking forever
                    available = kernel32.PeekNamedPipe(
                        proc.stderr.handle, None, None, None, None
                    )
                    if available:
                        chunk = proc.stderr.read1(4096)  # type: ignore[union-attr]
                        text = chunk.decode("utf-8", errors="replace")
                        stderr_chunks.append(text)
                        progress = self._parse_progress(text)
                        if progress is not None and progress > last_progress:
                            last_progress = progress
                            if progress_callback:
                                progress_callback(progress)
                elif proc.stderr is not None:
                    import selectors

                    sel = selectors.DefaultSelector()
                    sel.register(proc.stderr, selectors.EVENT_READ)
                    events = sel.select(timeout=0.2)
                    for key, _ in events:
                        chunk = key.fileobj.read1(4096)  # type: ignore[union-attr]
                        if chunk:
                            text = chunk.decode("utf-8", errors="replace")
                            stderr_chunks.append(text)
                            progress = self._parse_progress(text)
                            if progress is not None and progress > last_progress:
                                last_progress = progress
                                if progress_callback:
                                    progress_callback(progress)
                    sel.close()

            # Process has exited – drain remaining output
            remaining_out, remaining_err = proc.communicate(timeout=30)
            if remaining_out:
                stdout_chunks.append(remaining_out.decode("utf-8", errors="replace"))
            if remaining_err:
                stderr_chunks.append(remaining_err.decode("utf-8", errors="replace"))
                progress = self._parse_progress(
                    remaining_err.decode("utf-8", errors="replace")
                )
                if progress is not None and progress > last_progress:
                    last_progress = progress
                    if progress_callback:
                        progress_callback(progress)

            return {
                "success": proc.returncode == 0,
                "stdout": "".join(stdout_chunks),
                "stderr": "".join(stderr_chunks),
                "returncode": proc.returncode,
            }

        except FileNotFoundError:
            return {
                "success": False,
                "stdout": "",
                "stderr": f"PrusaSlicer executable not found: {cmd[0]}",
                "returncode": -1,
            }
        except subprocess.TimeoutExpired:
            return {
                "success": False,
                "stdout": "".join(stdout_chunks),
                "stderr": "".join(stderr_chunks) + "\nSubprocess timed out",
                "returncode": -1,
            }
        except Exception as exc:
            return {
                "success": False,
                "stdout": "".join(stdout_chunks),
                "stderr": str(exc),
                "returncode": -1,
            }

    # -------------------------------------------------- progress parsing

    _PROGRESS_RE = re.compile(r"(?:^|\s)(\d{1,3})%", re.MULTILINE)

    def _parse_progress(self, text: str) -> Optional[float]:
        """Extract the latest progress percentage from stderr text.

        PrusaSlicer emits lines like ``"  42%"`` or ``"Processing..."``.
        We grab the last numeric percentage found.
        """
        matches = self._PROGRESS_RE.findall(text)
        if matches:
            last = float(matches[-1])
            return min(last / 100.0, 1.0)
        return None

    # -------------------------------------------------- gcode parsing

    def _parse_gcode_stats(self, result: SliceResult) -> None:
        """Populate *result* fields from the G-code content."""
        gcode = result.gcode
        if not gcode:
            return

        # Layer count: lines starting with ``;LAYER_CHANGE`` or ``;LAYER:``
        layer_re = re.compile(r"^;LAYER:?\s*(\d+)", re.MULTILINE | re.IGNORECASE)
        layer_matches = layer_re.findall(gcode)
        if layer_matches:
            result.layer_count = max(int(m) for m in layer_matches) + 1
        else:
            # Count ``;LAYER_CHANGE`` comments as an alternative
            result.layer_count = gcode.count(";LAYER_CHANGE")

        # Estimated print time from PrusaSlicer comments:
        #   ``; estimated printing time (normal mode) = 1h 23m 45s``
        time_re = re.compile(
            r";\s*estimated\s+printing\s+time.*?=\s*(.+)",
            re.IGNORECASE,
        )
        time_match = time_re.search(gcode)
        if time_match:
            result.print_time = self._parse_duration(time_match.group(1).strip())

        # Filament usage (mm and grams):
        #   ``;Filament used: 1.23m, 4.56g``
        #   ``;Filament length: 1234.5 mm``
        filament_re = re.compile(
            r";\s*[Ff]ilament\s+(?:used|length)\s*:\s*([\d.]+)\s*(mm|m|g)",
        )
        for fm in filament_re.finditer(gcode):
            value = float(fm.group(1))
            unit = fm.group(2).lower()
            if unit == "m":
                value *= 1000.0
            if "length" in gcode[fm.start() - 5 : fm.start()].lower() or unit in (
                "mm",
                "m",
            ):
                result.filament_usage += value
                result.material_used += value
            if unit == "g":
                result.material_weight += value

        # Fallback: ``;MATERIAL:`` comments
        material_re = re.compile(r";\s*MATERIAL:\s*([\d.]+)\s*g", re.IGNORECASE)
        for mm in material_re.finditer(gcode):
            result.material_weight = max(result.material_weight, float(mm.group(1)))

        # Total move count
        result.total_moves = len(re.findall(r"^[GM]\d+", gcode, re.MULTILINE))

    def _parse_duration(self, duration_str: str) -> float:
        """Convert a human-readable duration like ``1h 23m 45s`` to seconds."""
        total = 0.0
        hours = re.search(r"(\d+)\s*h", duration_str)
        minutes = re.search(r"(\d+)\s*m", duration_str)
        seconds = re.search(r"(\d+)\s*s", duration_str)
        if hours:
            total += float(hours.group(1)) * 3600.0
        if minutes:
            total += float(minutes.group(1)) * 60.0
        if seconds:
            total += float(seconds.group(1))
        if total == 0.0:
            # Plain seconds?  Try a plain number
            plain = re.search(r"([\d.]+)", duration_str)
            if plain:
                total = float(plain.group(1))
        return total

    # -------------------------------------------------- value translation

    def _translate_value(self, our_key: str, value: Any) -> Optional[str]:
        """Translate an application setting value to a PrusaSlicer string.

        Returns ``None`` if the value should be omitted.
        """
        # Boolean flags → support_enable / enable_fan etc.
        if our_key in BOOLEAN_FLAGS:
            ini_key = BOOLEAN_FLAGS[our_key]
            if value in (True, "true", "True", "1", "yes"):
                return f"{ini_key} = 1"
            if value in (False, "false", "False", "0", "no"):
                return f"{ini_key} = 0"
            # Already a pair like "support_enable = 1"
            return f"{ini_key} = {int(bool(value))}"

        # Pattern / enum translation
        if our_key in PATTERN_MAP:
            mapped = PATTERN_MAP[our_key].get(str(value).lower())
            if mapped is not None:
                return mapped

        # Multi-line G-code values need quoting or special handling
        if our_key in (
            "start_gcode",
            "end_gcode",
            "before_layer_gcode",
            "toolchange_gcode",
        ):
            if isinstance(value, str) and "\n" in value:
                # PrusaSlicer expects the literal G-code; the INI format
                # stores it on a single line with escaped newlines.
                escaped = value.replace("\n", "\\n")
                return escaped
            return str(value)

        # Numeric types pass through
        if isinstance(value, (int, float)):
            if our_key == "infill_percentage":
                return f"{int(value)}%"
            return str(value)

        return str(value)

    # -------------------------------------------------- value translation (INI)

    def _translate_ini_value(self, ini_key: str, value: Any) -> Optional[str]:
        """Thin wrapper used when writing raw INI keys (already mapped)."""
        return str(value) if value is not None else None


# ---------------------------------------------------------------------------
# Module-level convenience
# ---------------------------------------------------------------------------

_default_wrapper: Optional[LibSlic3rWrapper] = None


def get_wrapper() -> LibSlic3rWrapper:
    """Return (and cache) a module-level ``LibSlic3rWrapper`` instance."""
    global _default_wrapper
    if _default_wrapper is None:
        _default_wrapper = LibSlic3rWrapper()
    return _default_wrapper


def quick_slice(
    mesh_path: str,
    settings: Dict[str, Any],
    output_dir: Optional[str] = None,
    progress_callback: Optional[Callable[[float], None]] = None,
) -> SliceResult:
    """Convenience function: create a wrapper and slice in one call."""
    return get_wrapper().slice(
        mesh_path, settings, output_dir, progress_callback=progress_callback
    )
