# FCD → Fusion 360 Parity Project — Phase Plan

**Repository**: [`southseact-3d/fcd`](https://github.com/southseact-3d/fcd) — a fork of `FreeCAD/FreeCAD` rebranded as **Tungsten CAD**, with a Fusion-360-style workbench selector and an in-progress Part→PartDesign workbench merge.
**Goal**: Verify that every command in the PartDesign workbench behaves identically to its Fusion 360 counterpart, with full feature parity. Find weird bugs. Prove the UI is completely usable.
**Scope of this phase plan**: PartDesign workbench only. Other workbenches (Sketcher, Draft, TechDraw, Fem, CAM, etc.) are out of scope for now.

---

## Phase Overview

| Phase | Name | Status | Branch |
|---|---|---|---|
| 0 | Build the App via GitHub Actions | ✅ Done | `fix/ubuntu-build-appimage` |
| 1 | _(merged into Phase 0 — AppImage packaging)_ | ✅ Done | `fix/ubuntu-build-appimage` |
| 2 | Build the PartDesign Parity Matrix | 🟡 In progress | `phase2/partdesign-parity-matrix` |
| 3 | CLI Test Harness (per-command, strict pass criteria) | ⏳ Pending | TBD |
| 4 | Manual GUI Verification Protocol | ⏳ Deferred until Phase 3 is green | TBD |
| 5 | Bug Triage + Parity Scorecard | ⏳ Deferred | TBD |
| 6 | Continuous Verification | ⏳ Ongoing | `main` |

---

## Phase 0 + 1 — Build the App via GitHub Actions ✅

**Objective**: Take the working Windows GitHub Actions workflow and port it to Ubuntu, run it until it produces a successful AppImage, then download the artifact.

**What was done**:
- Forked the existing `sub_buildWindows.yml` workflow into a new `sub_buildUbuntu.yml`.
- Aligned ccache settings with the Windows workflow (MAXSIZE=5G, COMPILERCHECK=content, SLOPPINESS without pch_defines, NODIRECT=true).
- Added a Ninja build cache (FC-NINJA-*) mirroring Windows so subsequent runs are true incremental builds.
- Added AppImage packaging using the existing `package/rattler-build/linux/AppDir/AppRun` launcher + `org.freecad.FreeCAD.desktop` + `freecad.svg` icon.
- Uploaded AppImage + logs + report as workflow artifacts (30-day retention).

**Bugs fixed during iteration** (8 workflow runs, 7 failures, 1 success):
1. Broken fallback URL for `appimagetool` (the `releases/download/13/...` asset 404s).
2. MSVC-vs-GCC friend-name lookup: `friend class ToolSettingsPanel;` resolves differently between compilers — qualified to `friend class ::Gui::ToolSettingsPanel;`.
3. Windows-vs-Linux case sensitivity: `#include "PropertyEditor/PropertyEditor.h"` vs on-disk `propertyeditor/`.
4. Missing apt packages (9 packages from upstream FreeCAD's install script were absent).
5. Missing `sbkversion.h` — KDE Neon's apt layout puts it in a nested `shiboken6/` subdir; CMake wasn't searching there. Patched `cMake/FindShiboken6.cmake` with `find_path()`.
6. SIGPIPE under `set -o pipefail` from `find / | head -5` — replaced with scoped `find /usr/lib/python3`.

**Deliverables**:
- Branch `fix/ubuntu-build-appimage` (8 commits, ready to merge to main when user approves).
- Successful workflow run: https://github.com/southseact-3d/fcd/actions/runs/28755790954
- AppImage artifact: `FreeCAD_1.2.0-Ubuntu-x86_64.AppImage` (145 MB).

**Acceptance criteria**: ✅ All met.
- Ubuntu workflow passes on `main` and on PRs.
- AppImage artifact appears in the workflow run.
- AppImage is a valid ELF executable.

---

## Phase 2 — Build the PartDesign Parity Matrix 🟡

**Objective**: Produce a single source-of-truth document listing every command in the PartDesign workbench, mapped to its Fusion 360 equivalent, with parity assessment.

**Method**: Direct code exploration — NO automated scripts. Read the source files:
- `src/Mod/PartDesign/Gui/Command.cpp` — 81 C++ command macros
- `src/Mod/PartDesign/Gui/CommandBody.cpp` — 6 C++ command macros (Body, Migrate, MoveTip, etc.)
- `src/Mod/PartDesign/Gui/CommandPrimitive.cpp` — 2 C++ command macros (additive/subtractive primitive dropdowns)
- `src/Mod/PartDesign/Gui/Workbench.cpp` — menu/toolbar structure
- `src/Mod/PartDesign/InitGui.py` — workbench registration, Python command imports
- `src/Mod/PartDesign/PartCommandBridge.py` — alias table mapping `PartDesign_Part*` → `Part_*` commands
- `src/Mod/PartDesign/BOPTools/{JoinFeatures,SplitFeatures,ToleranceFeatures}.py` — Boolean/Join/Split/Tolerance Python commands
- `src/Mod/PartDesign/CompoundTools/{_CommandCompoundFilter,_CommandExplodeCompound}.py`
- `src/Mod/PartDesign/PartWorkbenchMigrationGui.py` — `PartDesign_MigratePartMacros` command
- `src/Mod/PartDesign/{InvoluteGearFeature,SprocketFeature,ChainFeature}.py` — gear/sprocket/chain generators
- `src/Mod/PartDesign/WizardShaft/WizardShaft.py` — shaft wizard + callback
- `src/Mod/PartDesign/App/Feature*.cpp` — feature classes (one per PartDesign feature type)

**Deliverable**: `docs/PARITY_MATRIX.md` committed to the repo, with one row per command containing:
- Command ID (e.g., `PartDesign_Pad`)
- Source (C++ in `Command.cpp` line N, or Python in `PartCommandBridge.py`)
- Fusion 360 equivalent (e.g., "Extrude → Join")
- Parity assessment (`full` / `partial` / `gap` / `n/a`)
- Notes (what's missing, what differs, known bugs)

**Acceptance criteria**:
- Every PartDesign command (C++ + Python + bridged) appears as a row.
- Each row has a Fusion 360 equivalent identified.
- Parity status is marked.
- Total expected: ~95 commands after dedup.

---

## Phase 3 — CLI Test Harness ⏳

**Objective**: Build a per-command CLI test suite that exercises every command in the parity matrix and verifies it works correctly without launching the GUI.

**Architecture**:
```
tests/cli/
├── README.md                          # how to run
├── conftest.py                        # shared helpers (FreeCAD path, assertion utils)
├── runner.py                          # runs all tests, aggregates results
├── results/                           # CSV + JSON output per run
└── partdesign/
    ├── test_body.py
    ├── test_pad.py
    ├── test_pocket.py
    ├── test_revolution.py
    ├── test_groove.py
    ├── test_loft.py
    ├── test_pipe.py
    ├── test_helix.py
    ├── test_fillet.py
    ├── test_chamfer.py
    ├── test_draft.py
    ├── test_thickness.py
    ├── test_hole.py
    ├── test_pattern_linear.py
    ├── test_pattern_polar.py
    ├── test_mirrored.py
    ├── test_multitransform.py
    ├── test_datum_plane.py
    ├── test_datum_line.py
    ├── test_datum_point.py
    ├── test_coordinate_system.py
    ├── test_shapebinder.py
    ├── test_clone.py
    ├── test_subshapebinder.py
    ├── test_bridge_cut.py
    ├── test_bridge_union.py
    ├── test_bridge_common.py
    └── ... (one per matrix row)
```

**Per-test pattern** (example `test_pad.py`):
```python
"""CLI test for PartDesign_Pad command."""
import sys, os, subprocess, json, tempfile

FREECADCMD = os.environ.get("FREECADCMD", "FreeCADCmd")

SCRIPT = """
import FreeCAD as App
import PartDesign
import Sketcher

doc = App.newDocument("test_pad")
body = doc.addObject("PartDesign::Body","Body")
sketch = body.newObject("Sketcher::SketchObject","Sketch")
sketch.AttachmentSupport = doc.getObject('XY_Plane')
sketch.addGeometry(Part.Circle(App.Vector(0,0,0), App.Vector(0,0,1), 10))
sketch.addConstraint(Sketcher.Constraint('Radius',0,10.0))
doc.recompute()

pad = body.newObject("PartDesign::Pad","Pad")
pad.Profile = sketch
pad.Length = 5.0
doc.recompute()

# Assertions
errors = []
if pad.Shape.isNull():
    errors.append("Pad shape is null after recompute")
if pad.Shape.Volume < 1500 or pad.Shape.Volume > 1600:
    errors.append(f"Unexpected volume: {pad.Shape.Volume} (expected ~1570.8)")
if not pad.TypeId.startswith("PartDesign::Pad"):
    errors.append(f"Wrong TypeId: {pad.TypeId}")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
else:
    print("TEST_PASS")
    sys.exit(0)
"""

def run():
    with tempfile.NamedTemporaryFile(suffix=".py", mode="w", delete=False) as f:
        f.write(SCRIPT)
        script_path = f.name
    result = subprocess.run([FREECADCMD, script_path], capture_output=True, text=True, timeout=60)
    passed = result.returncode == 0 and "TEST_PASS" in result.stdout
    return {
        "command": "PartDesign_Pad",
        "passed": passed,
        "stdout": result.stdout[-2000:],
        "stderr": result.stderr[-2000:],
        "returncode": result.returncode,
    }

if __name__ == "__main__":
    r = run()
    print(json.dumps(r, indent=2))
    sys.exit(0 if r["passed"] else 1)
```

**Strict pass criteria** (per user spec):
1. `FreeCADCmd` exit code = 0
2. No `Error` / `Exception` in stdout/stderr
3. Expected geometry object exists in the document tree
4. (Implicit) Object has non-null `Shape` and recompute succeeded

**Runner** (`runner.py`):
- Walks `tests/cli/partdesign/` for `test_*.py` files
- Runs each as a subprocess with timeout (60s default)
- Captures stdout, stderr, returncode, duration
- Writes `results/<timestamp>.csv` and `results/<timestamp>.json`
- Prints a summary table at the end: `PASS 45 / FAIL 3 / SKIP 0`
- Exits non-zero if any test failed

**GitHub Actions integration**:
- Add `sub_cli_tests.yml` workflow triggered on PR + manual dispatch
- Runs after `sub_buildUbuntu.yml` succeeds (uses `needs:`)
- Downloads the build artifact, extracts FreeCADCmd path
- Runs `python tests/cli/runner.py`
- Uploads `results/` as artifact for review

**Acceptance criteria**:
- One `test_*.py` file per row in `docs/PARITY_MATRIX.md`
- Each test runs in <60s standalone
- `runner.py` produces a CSV + JSON report
- `sub_cli_tests.yml` runs in CI, fails the PR on any test failure
- At least 90% of tests pass on first green run (the rest become bug tickets)

**Estimated effort**: 30-50 hours (~95 test files × ~30 min each).

---

## Phase 4 — Manual GUI Verification Protocol ⏳ (Deferred)

**Objective**: Verify that every command is *usable* in the GUI, not just non-crashing from the CLI. CLI tests can't catch UX issues like "button is off-screen on 1366×768", "cursor disappears during sketch edit", "tooltip is wrong", "icon is missing", etc.

**Trigger**: Start only after Phase 3 is green.

**Method**:
1. Install the latest AppImage (from Phase 0) on a physical Linux machine.
2. For each command in the parity matrix, follow a written checklist:
   - Open workbench → confirm toolbar/icon loads
   - Invoke the command via menu, toolbar, and Python console (3 entry points must all work)
   - Verify undo/redo after the operation
   - Verify save/reopen preserves the result
   - Note any "weird bugs" (ghost selections, broken constraint solvers, missing tooltips, wrong translations, etc.)
3. Record screen with OBS per workbench section.
4. File bugs to GitHub Issues with repro steps + recording snippet + screenshot.

**Bug triage buckets**:
- `crash` (P0) — blocks parity claim
- `wrong-result` (P1) — blocks parity claim
- `ux-broken` (P2) — doesn't block but should be fixed
- `polish` (P3) — nice-to-have

**Acceptance criteria**:
- Every command in the parity matrix has a manual verification entry
- All P0/P1 bugs filed as issues
- All P2/P3 bugs filed as issues or explicitly deferred

**Estimated effort**: ~120 hours (~95 commands × ~75 min per manual check).

---

## Phase 5 — Bug Triage + Parity Scorecard ⏳

**Objective**: Produce a defensible "FCD has X% of Fusion 360 PartDesign feature parity" statement.

**Method**:
1. Decompose Fusion 360's Design workspace into atomic features (e.g., "Extrude → Symmetric distance", "Extrude → Operation: Intersect", "Revolve → Around an axis not in sketch plane"). Use the official Fusion 360 learning docs as the spec.
2. For each Fusion feature, find the FCD equivalent in the parity matrix, run its test, mark `working` / `partial` / `missing`.
3. For `partial` and `missing` features, file enhancement issues with a clear spec + Fusion reference.
4. Publish `docs/PARITY_SCORECARD.md` with a rollup, e.g.:
   - "Sketch-based features: 92%"
   - "Dress-up features: 78%"
   - "Patterns: 85%"
   - "Datums: 100%"
   - "Overall PartDesign parity: 87%"

**Acceptance criteria**:
- `docs/PARITY_SCORECARD.md` published
- Every `partial`/`missing` feature has a GitHub issue with spec + Fusion reference
- Rollup percentages defensible against the Fusion 360 docs

**Estimated effort**: 40 hours.

---

## Phase 6 — Continuous Verification ⏳

**Objective**: Keep parity from regressing on future PRs.

**Method**:
1. Run the full CLI test suite on every PR (Phase 3 harness) — split across 4 parallel jobs by category for ~15min total.
2. Weekly "full parity sweep" on `main` — runs Phase 4 checklist automatically where possible, posts a summary issue if any test regresses.
3. Monthly parity scorecard refresh — regenerate `docs/PARITY_SCORECARD.md`, post diffs to the team.

**Acceptance criteria**:
- PRs blocked if any CLI test fails
- Weekly sweep runs without manual intervention
- Monthly scorecard diffs are reviewed

---

## Branch Strategy

- **`main`** — stable, production-ready. Only merge feature branches here after Phase 3+ validation.
- **`fix/ubuntu-build-appimage`** — Phase 0+1 work (8 commits). Ready to merge when user approves.
- **`phase2/partdesign-parity-matrix`** — Phase 2 work (this document + `docs/PARITY_MATRIX.md`). Created from `main` (not from Phase 0's branch) so the two are independent.
- **Future Phase 3+ branches** — TBD when each phase starts.

The user has explicitly chosen to keep work on separate branches for now (not merge to main). Each phase's branch lives until the user approves a merge.

---

## Out of Scope (for now)

The following FCD workbenches are explicitly out of scope for this phase plan:
- **Sketcher** — needed for PartDesign, but treated as a dependency (already tested by FreeCAD upstream)
- **Draft** — 2D drafting
- **BIM/Arch** — architecture
- **TechDraw** — drawing
- **Fem** — simulation
- **CAM** (formerly Path) — manufacturing
- **Mesh / Points** — mesh modeling
- **Assembly** — assembly
- **Render, Animation, Sheet Metal, Generative Design** — known gaps in FCD vs Fusion 360

These will be addressed in a future phase plan once PartDesign parity is verified.
