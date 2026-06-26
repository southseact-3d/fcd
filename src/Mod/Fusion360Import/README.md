# Fusion 360 Import Workbench

This workbench reconstructs Fusion 360 parametric design history as
native Tungsten CAD features. It connects to a running Fusion 360
instance via the Fusion360MCP add-in, walks the Fusion timeline,
extracts every feature and parameter, then rebuilds the design as
Tungsten CAD objects inside one or more `PartDesign::Body` containers.

## Setup

### 1. Install the Fusion 360 MCP add-in

The workbench talks to Fusion 360 through the
[`fusion360-mcp-server`](https://github.com/faust-machines/fusion360-mcp-server)
add-in, which is a separate project. Install it once:

```powershell
git clone https://github.com/faust-machines/fusion360-mcp-server.git
Copy-Item -Recurse fusion360-mcp-server/Fusion360MCP `
    "$env:APPDATA\Autodesk\Autodesk Fusion 360\API\AddIns\"
```

Then, in Fusion 360:

1. Open **Shift+S** (Scripts and Add-Ins).
2. Select **Fusion360MCP** under Add-Ins.
3. Tick **Run on Startup** so the add-in is available every time.
4. Click **Run**. The TEXT COMMANDS window should print
   `[MCP] Server listening on localhost:9876`.

### 2. Build Tungsten CAD with this workbench enabled

```bash
pixi run configure
pixi run build
```

If you want to disable the workbench, pass
`-DBUILD_FUSION360IMPORT=OFF` to CMake.

### 3. Use the workbench

1. Open the design you want to import in Fusion 360 and leave the
   design active.
2. Switch to the **Fusion 360 Import** workbench in Tungsten CAD.
3. Click **Connect to Fusion 360...** and confirm the host
   (`127.0.0.1`) and port (`9876`). Use **Test connection** to verify.
4. Click **Import design from Fusion 360**. A progress dialog will
   show the extraction in real time. When it finishes, the **Results**
   dialog summarises what was reconstructed.

## What gets reconstructed

| Fusion 360 feature | Tungsten CAD feature |
|---|---|
| Extrude (add/cut/intersect) | `PartDesign::Pad` / `Pocket` |
| Revolve (add/cut) | `PartDesign::Revolution` / `Groove` |
| Loft (add/cut) | `PartDesign::AdditiveLoft` / `SubtractiveLoft` |
| Sweep (add/cut) | `PartDesign::AdditivePipe` / `SubtractivePipe` |
| Coil / helix | `PartDesign::AdditiveHelix` / `SubtractiveHelix` |
| Hole (with thread) | `PartDesign::Hole` |
| Fillet | `PartDesign::Fillet` |
| Chamfer | `PartDesign::Chamfer` |
| Draft | `PartDesign::Draft` |
| Shell | `PartDesign::Thickness` |
| Rectangular Pattern | `PartDesign::LinearPattern` |
| Circular Pattern | `PartDesign::PolarPattern` |
| Path Pattern | `PartDesign::PatternOnPath` |
| Mirror | `PartDesign::Mirrored` |
| Scale | `PartDesign::Scaled` |
| Boolean / Combine | `PartDesign::Boolean` |
| Box / Cylinder / Sphere / Torus / Cone | `PartDesign::Additive*` |
| Construction plane / axis / point | `Part::Datum*` |
| Sketch + constraints | `Sketcher::SketchObject` |
| User / Model parameters | `Spreadsheet::Sheet` with aliases |
| Occurrences (assembly instances) | `PartDesign::Body` + transform |

## What is recorded but not rebuilt

- Fusion surface-only operations: ReplaceFace, BoundaryFill, Patch,
  Stitch, Unstitch, Thicken, Form, Rip, SilhouetteSplit,
  RuleFillet, Pipe-wall, Rib, Web.
- Cosmetic threads on existing cylindrical faces (ThreadFeature)
  - add a `PartDesign::Hole` with `Threaded=True` manually.
- Components inside assemblies are flattened to a single Body; the
  parametric children of an occurrence cannot be reconstructed from
  F3D alone.

The full list of reconstructed features and any warnings are shown in
the Results dialog after each import.

## Architecture

```
Tungsten CAD ─► FusionExtractor ─► Fusion360MCP add-in ─► Fusion 360
   ▲                                       │
   │                                       ▼
   └── FeatureRebuilder ◄── JSON history ◄── (timeline + STEP)
```

`FusionExtractor` sends the extraction script (a string of Python
that runs in Fusion's main thread) to the add-in via TCP. The script
walks `design.timeline`, collects every feature, parameter, sketch,
constraint, datum, body, occurrence and appearance, exports a STEP
file for geometry verification, and emits a single JSON document.

The rebuilder then walks that JSON and creates matching Tungsten CAD
objects inside one `PartDesign::Body` (with a separate body per
occurrence). Entity references (face/edge tokens) are resolved via
positional indices captured during extraction, producing stable
`Face1`, `Edge3` style sub-element names on the FreeCAD side.

## Files

```
src/Mod/Fusion360Import/
  __init__.py
  Init.py                       # Headless init
  InitGui.py                    # GUI init (workbench registration)
  CMakeLists.txt                # Build script
  fusion360_mcp_client.py       # TCP JSON-RPC client for the add-in
  fusion360_extractor.py        # Python script that runs *inside* Fusion 360
  extractor.py                  # High-level wrapper that fetches history
  sketch_rebuilder.py           # Reconstructs Sketcher sketches
  feature_rebuilder.py          # Reconstructs PartDesign features
  reference_resolver.py         # Maps Fusion tokens to FreeCAD sub-elements
  importer.py                   # Top-level orchestrator
  commands.py                   # GUI commands
  connection_dialog.py          # Settings dialog (host/port)
  results_dialog.py             # Post-import summary
  progress_dialog.py            # Real-time progress feedback
  help_dialog.py                # In-app user guide
  Fusion360ImportTests/         # Unit tests
  Resources/icons/Fusion360Import.svg
  README.md
```

## Limitations and roadmap

- Fillet/chamfer edge selection is best-effort: we capture every
  Fusion edge token, resolve what we can, and fall back to
  "use all edges" if no edges resolve.
- Patterns may end up without a `Direction` reference when Fusion
  uses a custom construction plane as the direction; set it manually.
- Assemblies are flattened into one body per occurrence; the
  parametric children inside each component are recorded but not
  rebuilt.
- Some surface-only operations (BoundaryFill, Patch, etc.) are
  recorded but skipped.

## License

LGPL-2.1-or-later, same as the rest of Tungsten CAD.