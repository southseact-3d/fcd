# AGENTS.md

## Project Overview

Tungsten CAD is an open-source parametric 3D CAD modeler (FreeCAD fork). It is a large C++/Python application with a CMake build system, using OpenCASCADE as the geometry kernel, Coin3D for 3D rendering, Qt 6 for the GUI, and Python/PySide6 for scripting.

## Build Commands

The project uses **Pixi** (conda-based) as its package manager and **CMake + Ninja** for builds. All tasks are defined in `pixi.toml`.

```bash
# Initialize submodules (required before first build)
pixi run initialize

# Configure (defaults to debug)
pixi run configure
pixi run configure-release

# Build
pixi run build
pixi run build-release

# Install
pixi run install
pixi run install-release

# Run the application
pixi run freecad
pixi run freecad-release
```

CMake presets are platform-specific (e.g. `conda-linux-debug`, `conda-windows-release`). The build output goes to `build/debug/` or `build/release/`.

## Test Commands

```bash
# Run all C++ tests (GoogleTest via CTest)
pixi run test
pixi run test-release

# Or directly
ctest --test-dir build/debug
ctest --test-dir build/release
```

C++ tests live in `tests/src/` with test executables: `App_tests_run`, `Base_tests_run`, `Misc_tests_run`, `Zipios_tests_run`, and module-specific ones (Sketcher, PartDesign, Assembly, etc.).

Python tests are within each module (e.g. `src/Mod/PartDesign/TestPartDesignApp.py`) and run via the `src/Mod/Test` framework.

Tests require `ENABLE_DEVELOPER_TESTS=ON` (enabled by default in conda presets).

## Linting and Formatting

### Pre-commit hooks

```bash
pre-commit run --all-files
```

Hooks: trailing-whitespace, end-of-file-fixer, black (Python), clang-format (C++), codespell, check-yaml, check-added-large-files, mixed-line-ending.

### C++ Linting

```bash
# clang-format (formatting)
clang-format -i <file>

# clang-tidy (static analysis, needs compile_commands.json)
clang-tidy <file>

# cpplint (Google style)
python tools/lint/cpplint.py <file>

# clazy (Qt analysis)
python tools/lint/clazy.py <file>
```

Config: `.clang-format`, `.clang-tidy`, `tools/lint/cpplint.cfg`

### Python Linting

```bash
# black (formatter, line length 100)
black <file>

# pylint (config in .pylintrc)
pylint <file>
```

Config: `.pylintrc`

### All Lint Scripts

Located in `tools/lint/`: `clang_format.py`, `clang_tidy.py`, `cpplint.py`, `clazy.py`, `clazy_qt6.py`, `pylint.py`, `black.py`, `codespell.py`, `generic_checks.py`, `qt_connections.py`, `changed_lines.py`

## Code Style

### C++ (`.clang-format`)

- LLVM-based style, Qt conventions
- 4-space indent, no tabs, 100 column limit
- Allman braces for classes/functions/namespaces/enums/structs; K&R for control statements
- Pointer/reference alignment: left (`int* p`, `int& r`)
- Insert braces on all control statements
- No short functions/blocks on single line
- Template declarations on separate line
- No include sorting (manual order)

### Python (`.pylintrc` + Black)

- **Black**: line length 100
- **pylint**: max line length 120, snake_case for functions/methods/variables, PascalCase for classes, UPPER_CASE for constants
- 4-space indent
- PySide2 is deprecated; use PySide6
- Max 5 arguments per function, 15 attributes per class

### General

- SPDX license identifiers on all files
- Modern C++ (C++17/20)
- Doxygen-style documentation for public APIs

## Architecture

### Core Layers

- `src/Base/` -- Foundation library (platform abstraction, math, utilities)
- `src/App/` -- Application core (document model, property system, transactions)
- `src/Gui/` -- GUI layer (view providers, widgets, 3D viewer, undo/redo UI)
- `src/Main/` -- Entry points (`MainGui.cpp`, `MainCmd.cpp`, `MainPy.cpp`)

### Modules (`src/Mod/`)

39 workbench modules, each self-contained with App/Gui/Test subdirectories:

| Module | Purpose |
|---|---|
| PartDesign | Parametric part design |
| Sketcher | 2D constrained sketching |
| Part | Basic part modeling |
| Assembly | Assembly workbench |
| BIM | Building Information Modeling |
| CAM | Computer Aided Manufacturing |
| Draft | 2D drafting |
| Fem | Finite Element Method |
| TechDraw | Technical drawing |
| Mesh / MeshPart | Mesh operations and conversion |
| Surface | Surface modeling |
| Import | File import/export |

### Key Patterns

- **Document-Observer**: `App::Document` is the central data model; objects are `App::DocumentObject` instances
- **View-Provider**: Each document object has a `Gui::ViewProvider` for 3D rendering
- **Property System**: Objects expose typed properties (float, string, vector, link, etc.)
- **Extension System**: Python extensions via PyCXX/Shiboken6 bindings

### Third-Party (`src/3rdParty/`)

OndselSolver, GoogleTest, Microsoft GSL, libE57Format, PyCXX, FastSignals, json, zipios++, salomesmesh

## Key Files

| File | Purpose |
|---|---|
| `CMakeLists.txt` | Root CMake build definition |
| `CMakePresets.json` | Platform-specific build presets |
| `pixi.toml` | Package manager config and task definitions |
| `.clang-format` | C++ formatting rules |
| `.clang-tidy` | C++ static analysis rules |
| `.pylintrc` | Python linting rules |
| `.pre-commit-config.yaml` | Pre-commit hook definitions |
| `CONTRIBUTING.md` | Contribution process and PR requirements |
| `README.md` | Project overview and links |
