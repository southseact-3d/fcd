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
| 2 | Build the PartDesign Parity Matrix | ✅ Done | `phase2/partdesign-parity-matrix` |
| 3 | CLI Test Harness (per-command, strict pass criteria) | 🟡 In progress | `phase3/cli-test-harness` |
| 4 | Interactive GUI Verification (vs Fusion 360, side-by-side) | 📋 Planned (not started) | TBD |
| 5 | Bug Triage + Parity Scorecard | ⏳ Pending | TBD |
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

## Phase 2 — Build the PartDesign Parity Matrix ✅

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

## Phase 4 — Interactive GUI Verification Protocol ⏳ (Planned — not yet started)

**Objective**: Verify that every PartDesign command and every UI surface (viewport, menus, toolbars, dock windows, preferences, shortcuts, drag-drop, file ops, themes) is **intuitive** when compared side-by-side with Fusion 360. This is not just functional testing — it's a usability audit.

**Trigger**: Start only after Phase 3 is green.

### Testing approach (locked-in by user answers)

| Dimension | Decision | Notes |
|---|---|---|
| **Tester** | You (manual) | You run the AppImage on your own Linux machine following a written checklist that I prepare. Most realistic for UX judgement — I can't truly judge "intuitive" via headless automation. |
| **Scope** | Everything | Commands + viewport (zoom/pan/rotate/view cube/navigation styles) + menus/toolbar layout + dock windows (tree, task panel, Python console, report view) + preferences dialog + keyboard shortcuts + drag-drop + file open/save flows + themes. ~220 hours total. |
| **Intuitive definition** | vs Fusion 360 | Same task in FCD and Fusion 360 side-by-side. For each task, measure: click count, time-to-complete, errors, and subjective "did this feel right?". Requires a Fusion 360 license. |
| **Record format** | Markdown checklist | `docs/PHASE4_RESULTS.md` with one section per command/UI surface, containing: steps tried, FCD result, Fusion 360 result, click count comparison, screenshots, severity, notes. |
| **Severity** | 5-level (industry standard) | Critical / Major / Minor / Trivial / Enhancement |
| **Platforms** | Linux only | Just the AppImage we built in Phase 0. Windows + macOS deferred. |
| **Edge cases** | Doc states + Screen sizes | Test in: empty doc, single Body, complex doc (20+ features), after undo/redo, after save/reopen. Also at 1920×1080, 1366×768 (small laptop), 2560×1440 (hi-res). |
| **Bugs found** | One GitHub Issue per bug | Each bug filed as a separate issue with repro steps, screenshots, severity label. |

### Detailed method

#### Step 1 — Prepare the test checklist
I will write `docs/PHASE4_CHECKLIST.md` containing one section per UI surface to test, organized as:

1. **Commands** (~97 entries) — one section per command from `docs/PARITY_MATRIX.md`
2. **Viewport interactions** (~15 entries) — zoom (wheel, toolbar, menu), pan, rotate, view cube, navigation styles (CAD/Blender/Maya/Touch), fit-to-view, save-view, screenshot
3. **Menus and toolbars** (~10 entries) — top menu bar, Part Design menu structure, Sketch menu, View menu, context menus (right-click in tree, right-click in viewport, right-click on feature)
4. **Dock windows** (~6 entries) — Combo View (tree + task panel), Python console, Report view, Selection view, DAG view
5. **Preferences dialog** (~8 entries) — PartDesign settings, Sketcher settings, Display settings, Navigation settings, Workbench selector (Fusion 360 style toggle)
6. **Keyboard shortcuts** (~20 entries) — standard shortcuts (S for sketch, P for pad, etc.), navigation shortcuts, undo/redo, save/open
7. **Drag-and-drop** (~5 entries) — drag features in tree, drag files into app, drag from selection view
8. **File operations** (~6 entries) — new document, open (FCStd, STEP, IGES, STL), save, save-as, export, import
9. **Themes** (~3 entries) — light theme, dark theme, classic theme (if present)

Each section will have:
- A **task description** (e.g., "Create a 50×30×10 mm bracket with 4 Ø5 mm holes")
- **Fusion 360 reference steps** (the canonical way to do it in Fusion)
- **FCD steps to try** (the equivalent FCD workflow)
- **Fields to fill in**: FCD click count, Fusion click count, FCD time, Fusion time, errors encountered, screenshots, severity, notes

#### Step 2 — You execute the checklist
You install the latest AppImage on your Linux machine and work through `docs/PHASE4_CHECKLIST.md` section by section. For each entry:
1. Open the same task in both FCD and Fusion 360 (side-by-side windows if you have a wide monitor)
2. Time yourself and count clicks in each
3. Note any errors, confusion, missing features, or weird behavior
4. Take screenshots of failures
5. Fill in the section in `docs/PHASE4_RESULTS.md`
6. If you find a bug, file a GitHub Issue with: title, severity, repro steps, expected vs actual, screenshot, label `phase-4-finding`

#### Step 3 — Bug triage
After each workbench section is complete, I will:
- Review all GitHub issues filed from that section
- Tag each with the appropriate workbench label
- Cross-reference with `docs/PARITY_MATRIX.md` to update parity status if a `full` is found to actually be `partial`
- Surface any patterns (e.g., "every dress-up feature has the same UX bug")

#### Step 4 — Final report
When all sections are done, I will:
- Roll up results into `docs/PHASE4_SUMMARY.md`
- Calculate: total commands tested, total bugs found by severity, average click-count delta vs Fusion 360, average time delta
- List the top 10 most problematic commands (by severity × frequency)
- List the top 10 best-matching commands (lowest click/time delta, no bugs)

### Bug severity definitions

| Level | Definition | Example |
|---|---|---|
| **Critical** | Crash, data loss, or produces wrong geometry silently | App segfaults when clicking Pad; Pad creates a 0-volume solid without warning |
| **Major** | Feature is broken or unusable; no reasonable workaround | Fillet dialog doesn't accept edge selection; Pad preview shows wrong shape |
| **Minor** | Feature works but is confusing or has a small bug | Tooltip says "Pad" but command is "Pocket"; icon is missing for Chamfer |
| **Trivial** | Cosmetic or nitpick | Wrong spacing in dialog; icon is slightly off-center; typo in tooltip |
| **Enhancement** | Not a bug — a request for improvement vs Fusion 360 | "Fusion 360 shows a live preview while dragging; FCD doesn't" |

### Acceptance criteria

- Every command in `docs/PARITY_MATRIX.md` has a manual verification entry in `docs/PHASE4_RESULTS.md`
- All viewport / menu / toolbar / dock window / preferences / shortcuts / drag-drop / file ops / themes sections are complete
- All Critical and Major bugs are filed as GitHub Issues with full repro
- All Minor / Trivial / Enhancement findings are filed or explicitly deferred
- `docs/PHASE4_SUMMARY.md` is published with the rollup

### Estimated effort

| Section | Entries | Hours |
|---|---:|---:|
| Commands (per the parity matrix) | 97 | 130 |
| Viewport interactions | 15 | 12 |
| Menus and toolbars | 10 | 8 |
| Dock windows | 6 | 5 |
| Preferences dialog | 8 | 6 |
| Keyboard shortcuts | 20 | 8 |
| Drag-and-drop | 5 | 4 |
| File operations | 6 | 6 |
| Themes | 3 | 3 |
| Edge cases (doc states × screen sizes for 5 key commands) | 15 | 15 |
| Bug triage + summary report | — | 15 |
| **Total** | **185** | **~212** |

This is a heavy phase. Suggest splitting across 2–3 weeks of focused work, or scope it down to commands + viewport only (~150 hours) for a first pass and defer the rest.

### What I will NOT do in Phase 4

- **Drive the GUI myself via headless automation** — I can't judge "intuitive" through xdotool. The user must do the side-by-side with Fusion 360.
- **Test on Windows or macOS** — out of scope; the AppImage is Linux-only.
- **Test multiple UI languages** — English only for now (translation issues are a separate concern).
- **Test multiple themes** — light theme only for the first pass; dark theme as a "nice-to-have" if time permits.
- **Write automated regression tests for UX** — Phase 4 produces a markdown report and bug issues, not code. Phase 6 (continuous verification) may automate specific UX checks later.

### Prerequisites

Before starting Phase 4, the following must be true:
1. Phase 3 is green — every CLI test passes (or has a known-bug ticket).
2. The latest AppImage is downloaded from the Phase 0/1 workflow.
3. You have a working Fusion 360 installation for side-by-side comparison.
4. You have ~3 weeks of focused time available (or we scope down).

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
- **`phase2/partdesign-parity-matrix`** — Phase 2 work (`docs/PHASES.md` + `docs/PARITY_MATRIX.md`). Created from `main`.
- **`phase3/cli-test-harness`** — Phase 3 work (CLI test files + runner + workflow). Branched from `fix/ubuntu-build-appimage` so the tests can actually run in CI (carries the Ubuntu build fix), with `phase2/partdesign-parity-matrix` merged in to also carry the docs.
- **Future Phase 4+ branches** — TBD when each phase starts.

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
