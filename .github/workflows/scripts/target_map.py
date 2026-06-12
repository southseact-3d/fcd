"""Source-to-CMake-target mapping for Tungsten CAD dev patch builds.

Given a list of changed source files (from git diff), determines which
CMake targets need to be rebuilt. Handles the dependency cascade so that
building a module's App target also triggers its Gui target.
"""

import re
import sys
from pathlib import PurePosixPath

# ---------------------------------------------------------------------------
# Core library targets — changes here cascade to everything downstream
# ---------------------------------------------------------------------------
CORE_TARGETS = {
    "src/Base":     "FreeCADBase",
    "src/App":      "FreeCADApp",
    "src/Gui":      "FreeCADGui",
    "src/Main":     None,  # handled specially (multiple targets)
}

CORE_DLL_NAMES = {"FreeCADBase", "FreeCADApp", "FreeCADGui"}

# ---------------------------------------------------------------------------
# Module target mapping: source dir prefix → (App target, Gui target, Scripts target)
# The Gui target is only needed when BUILD_GUI=ON.
# ---------------------------------------------------------------------------
MODULE_MAP = {
    "Assembly":             ("Assembly",              "AssemblyGui",              None),
    "BIM":                  (None,                    None,                       "BIM"),
    "CAM":                  ("Path",                  "PathGui",                  None),
    "Cloud":                ("Cloud",                 "CloudGui",                 None),
    "Colour":               (None,                    None,                       None),
    "Draft":                ("DraftUtils",            None,                       "Draft"),
    "Fem":                  ("Fem",                   "FemGui",                   "FemScriptsTarget"),
    "Help":                 (None,                    None,                       None),
    "Idf":                  (None,                    None,                       None),
    "Import":               ("Import",                "ImportGui",                None),
    "Inspection":           ("Inspection",            "InspectionGui",            None),
    "JtReader":             ("JtReader",              None,                       None),
    "Material":             ("Materials",             "MatGui",                   "MaterialScripts"),
    "Measure":              ("Measure",               "MeasureGui",               None),
    "Mesh":                 ("Mesh",                  "MeshGui",                  "MeshScripts"),
    "MeshPart":             ("MeshPart",              "MeshPartGui",              None),
    "OpenSCAD":             (None,                    None,                       None),
    "Part":                 ("Part",                  "PartGui",                  "PartScripts"),
    "PartDesign":           ("PartDesign",            "PartDesignGui",            "PartDesignScripts"),
    "Plot":                 (None,                    None,                       None),
    "Points":               ("Points",                "PointsGui",                None),
    "Render":               (None,                    None,                       None),
    "ReverseEngineering":   ("ReverseEngineering",    "ReverseEngineeringGui",    None),
    "Robot":                ("Robot",                 "RobotGui",                 None),
    "Sandbox":              ("Sandbox",               "SandboxGui",               None),
    "Show":                 (None,                    None,                       None),
    "Sketcher":             ("Sketcher",              "SketcherGui",              None),
    "Spreadsheet":          ("Spreadsheet",           "SpreadsheetGui",           None),
    "Start":                (None,                    "StartGui",                 None),
    "Surface":              ("Surface",               "SurfaceGui",               None),
    "TechDraw":             ("TechDraw",              "TechDrawGui",              "TechDraw_Data"),
    "TemplatePyMod":        (None,                    None,                       None),
    "Test":                 (None,                    "QtUnitGui",                None),
    "Tux":                  (None,                    None,                       None),
    "Web":                  ("Web",                   None,                       None),
}

# Third-party targets
THIRDPARTY_MAP = {
    "src/3rdParty/salomesmesh/SMDS":           "SMDS",
    "src/3rdParty/salomesmesh/SMESHDS":        "SMESHDS",
    "src/3rdParty/salomesmesh/SMESH":          "SMESH",
    "src/3rdParty/salomesmesh/Driver":         "Driver",
    "src/3rdParty/salomesmesh/DriverSTL":      "DriverSTL",
    "src/3rdParty/salomesmesh/DriverDAT":      "DriverDAT",
    "src/3rdParty/salomesmesh/DriverUNV":      "DriverUNV",
    "src/3rdParty/salomesmesh/MEFISTO2":       "MEFISTo2",
    "src/3rdParty/salomesmesh/StdMeshers":     "StdMeshers",
    "src/3rdParty/salomesmesh/NETGENPlugin":   "NETGENPlugin",
    "src/3rdParty/OndselSolver":               "OndselSolver",
    "src/3rdParty/FastSignals":                "libfastsignals",
}

# CAM sub-targets
CAM_SUBTARGETS = {
    "src/Mod/CAM/libarea":       "area",
    "src/Mod/CAM/PathSimulator": "PathSimulator",
    "src/Mod/CAM/AppGL":         "CAMSimulator",
}


def _extract_module_name(path: str) -> str | None:
    """Extract the module directory name from a src/Mod/<Name>/... path."""
    parts = path.split("/")
    if len(parts) >= 3 and parts[0] == "src" and parts[1] == "Mod":
        return parts[2]
    return None


def _is_in_dir(path: str, dir_prefix: str) -> bool:
    """Check if path starts with dir_prefix (treating / as separator)."""
    return path.startswith(dir_prefix + "/") or path == dir_prefix


def map_changed_files(changed_files: list[str], build_gui: bool = True) -> set[str]:
    """Map a list of changed source files to CMake targets that must be built.

    Args:
        changed_files: List of file paths relative to the repo root.
        build_gui: Whether the build includes the GUI (FreeCADGui).

    Returns:
        Set of CMake target names to build.
    """
    targets: set[str] = set()
    needs_full_rebuild = False

    for f in changed_files:
        # Skip non-source files
        if not any(f.endswith(ext) for ext in (
            ".cpp", ".h", ".hpp", ".cxx", ".c", ".py", ".cmake", ".ui",
            ".qrc", ".ts", ".rc", ".def",
        )):
            # But still check CMakeLists.txt changes
            if not f.endswith("CMakeLists.txt"):
                continue

        # --- Core library changes → full rebuild ---
        for dir_prefix, target_name in CORE_TARGETS.items():
            if _is_in_dir(f, dir_prefix):
                if target_name:
                    targets.add(target_name)
                else:
                    # src/Main → rebuild executables
                    targets.update(["FreeCADMain", "FreeCADMainCmd", "FreeCADMainPy", "FreeCADGuiPy"])
                # Core changes cascade — mark for full rebuild
                if f.startswith("src/Base/") or f.startswith("src/App/"):
                    needs_full_rebuild = True
                break

        # --- Module changes ---
        module_name = _extract_module_name(f)
        if module_name and module_name in MODULE_MAP:
            app_target, gui_target, scripts_target = MODULE_MAP[module_name]

            # Determine sub-path within the module
            module_rel = f[len("src/Mod/" + module_name):]

            if module_rel.startswith("/App/"):
                if app_target:
                    targets.add(app_target)
            elif module_rel.startswith("/Gui/"):
                if gui_target and build_gui:
                    targets.add(gui_target)
            elif module_rel.startswith("/Resources/") or module_rel.endswith(".py"):
                if scripts_target:
                    targets.add(scripts_target)
                # Python/resource changes don't need C++ recompilation,
                # but we still want to package them
            else:
                # Generic module change — build everything for this module
                if app_target:
                    targets.add(app_target)
                if gui_target and build_gui:
                    targets.add(gui_target)
                if scripts_target:
                    targets.add(scripts_target)

        # --- Third-party changes ---
        for dir_prefix, target_name in THIRDPARTY_MAP.items():
            if _is_in_dir(f, dir_prefix):
                targets.add(target_name)
                break

        # --- CAM sub-targets ---
        for dir_prefix, target_name in CAM_SUBTARGETS.items():
            if _is_in_dir(f, dir_prefix):
                targets.add(target_name)
                break

        # --- Ext (Python namespace) changes ---
        if f.startswith("src/Ext/"):
            # These are configure-time copies, no build target needed
            pass

    # --- Cascade: if a module's App target changed, rebuild its Gui too ---
    if build_gui:
        for module_name, (app_target, gui_target, _) in MODULE_MAP.items():
            if app_target and app_target in targets and gui_target:
                targets.add(gui_target)

    # --- Full rebuild if core changed ---
    if needs_full_rebuild:
        # Add all module targets
        for module_name, (app_target, gui_target, scripts_target) in MODULE_MAP.items():
            if app_target:
                targets.add(app_target)
            if gui_target and build_gui:
                targets.add(gui_target)
            if scripts_target:
                targets.add(scripts_target)
        # Add all third-party targets
        targets.update(THIRDPARTY_MAP.values())
        # Add executables
        targets.update(["FreeCADMain", "FreeCADMainCmd", "FreeCADMainPy", "FreeCADGuiPy"])

    # Filter out None values
    targets.discard(None)

    return targets


def get_output_files_for_targets(targets: set[str], build_dir: str = "build/release") -> list[str]:
    """Given a set of CMake targets, return the expected output file paths.

    This uses the known output naming conventions:
    - Module targets produce <Name>.pyd in Mod/<Name>/
    - Core targets produce <Name>.dll in bin/
    - Executables produce .exe in bin/

    Args:
        targets: Set of CMake target names.
        build_dir: Path to the build directory.

    Returns:
        List of output file paths relative to the build directory.
    """
    outputs = []

    for target in sorted(targets):
        # Check if it's a known module target
        for module_name, (app_target, gui_target, _) in MODULE_MAP.items():
            if target == app_target and app_target:
                outputs.append(f"Mod/{module_name}/{target}.pyd")
                break
            elif target == gui_target and gui_target:
                outputs.append(f"Mod/{module_name}/{target}.pyd")
                break
        else:
            # Core libraries and executables
            if target in ("FreeCADBase", "FreeCADApp", "FreeCADGui"):
                outputs.append(f"bin/{target}.dll")
            elif target in ("FreeCADMain",):
                outputs.append("bin/FreeCAD.exe")
            elif target in ("FreeCADMainCmd",):
                outputs.append("bin/FreeCADCmd.exe")
            elif target in ("FreeCADMainPy",):
                outputs.append("bin/FreeCAD.pyd")
            elif target in ("FreeCADGuiPy",):
                outputs.append("bin/FreeCADGui.pyd")
            elif target in THIRDPARTY_MAP.values():
                outputs.append(f"lib/{target}.dll")
            elif target in CAM_SUBTARGETS.values():
                if target == "area":
                    outputs.append(f"Mod/CAM/{target}.pyd")
                elif target == "PathSimulator":
                    outputs.append(f"Mod/CAM/{target}.pyd")
                elif target == "CAMSimulator":
                    outputs.append(f"Mod/CAM/{target}.pyd")

    return outputs


def main():
    """CLI entry point: read changed files from stdin (one per line), print targets."""
    import argparse

    parser = argparse.ArgumentParser(description="Map changed files to CMake targets")
    parser.add_argument("--build-gui", action="store_true", default=True,
                        help="Build GUI targets (default: True)")
    parser.add_argument("--no-build-gui", dest="build_gui", action="store_false",
                        help="Skip GUI targets")
    parser.add_argument("--output-files", action="store_true",
                        help="Also print expected output file paths")
    parser.add_argument("--build-dir", default="build/release",
                        help="Build directory path")
    args = parser.parse_args()

    changed_files = [line.strip() for line in sys.stdin if line.strip()]

    if not changed_files:
        print("No changed files provided.", file=sys.stderr)
        sys.exit(0)

    print(f"Changed files: {len(changed_files)}", file=sys.stderr)

    targets = map_changed_files(changed_files, build_gui=args.build_gui)

    if not targets:
        print("No build targets needed.", file=sys.stderr)
        sys.exit(0)

    # Print targets (one per line)
    for t in sorted(targets):
        print(t)

    if args.output_files:
        print("---", file=sys.stderr)
        outputs = get_output_files_for_targets(targets, args.build_dir)
        for o in outputs:
            print(f"OUTPUT:{o}")


if __name__ == "__main__":
    main()
