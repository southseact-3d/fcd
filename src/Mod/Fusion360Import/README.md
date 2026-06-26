# Fusion 360 Import Workbench

This workbench reconstructs Fusion 360 parametric design history as
native Tungsten CAD features. It connects to a running Fusion 360
instance via the Fusion360MCP add-in, walks the Fusion timeline,
extracts every feature and parameter, then rebuilds the design as a
`PartDesign::Body` filled with `PartDesign::Pad`, `Pocket`, `Fillet`,
`Hole`, `SketchObject`, etc.

## Setup

### 1. Install the Fusion 360 MCP add-in

The workbench talks to Fusion 360 through the
[`fusion360-mcp-server`](https://github.com/faust-machines/fusion360-mcp-server)
add-in, which is a separate project. Install it once:

```powershell
# Clone the add-in repository
git clone https://github.com/faust-machines/fusion360-mcp-server.git

# Copy the add-in into Fusion's AddIns folder
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

The workbench is shipped as part of the Tungsten CAD source tree. To
build it:

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
3. Click **Connect to Fusion 360...** and confirm the host (`127.0.0.1`)
   and port (`9876`). Use **Test connection** to verify the add-in is
   reachable.
4. Click **Import design from Fusion 360**. A progress dialog will
   show the extraction in real time. When it finishes, the **Results**
   dialog summarises what was reconstructed.

## What gets reconstructed

| Fusion 360 feature | Tungsten CAD feature |
|---|---|
| Extrude (add) | `PartDesign::Pad` |
| Extrude (cut) | `PartDesign::Pocket` |
| Revolve (add) | `PartDesign::Revolution` |
| Revolve (cut) | `PartDesign::Groove` |
| Fillet | `PartDesign::Fillet` |
| Chamfer | `PartDesign::Chamfer` |
| Hole (with thread) | `PartDesign::Hole` |
| Shell | `PartDesign::Thickness` |
| Box primitive | `PartDesign::AdditiveBox` |
| Cylinder primitive | `PartDesign::AdditiveCylinder` |
| Sphere primitive | `PartDesign::AdditiveSphere` |
| Torus primitive | `PartDesign::AdditiveTorus` |
| Sketch + constraints | `Sketcher::SketchObject` |
| User / Model parameters | Document properties |
| Reference geometry | Default XY/XZ/YZ (fallback) |

## What is recorded but not yet rebuilt

- Rectangular patterns, circular patterns, mirror features
- Loft, sweep, coil, rib, web features
- Assemblies (occurrences are flattened into a single body)
- Custom fillet/chamfer edge selection (defaults to "use all edges")

The full list of reconstructed features and any warnings are shown in
the Results dialog after each import.

## Files

```
src/Mod/Fusion360Import/
  __init__.py
  Init.py                   # Headless init
  InitGui.py                # GUI init (workbench registration)
  CMakeLists.txt            # Build script
  fusion360_mcp_client.py   # TCP JSON-RPC client for the add-in
  fusion360_extractor.py    # Python script that runs *inside* Fusion 360
  extractor.py              # High-level wrapper that fetches history
  sketch_rebuilder.py       # Reconstructs Sketcher sketches
  feature_rebuilder.py      # Reconstructs PartDesign features
  importer.py               # Top-level orchestrator
  commands.py               # GUI commands
  connection_dialog.py      # Settings dialog (host/port)
  results_dialog.py         # Post-import summary
  progress_dialog.py        # Real-time progress feedback
  help_dialog.py            # In-app user guide
  Resources/icons/Fusion360Import.svg
```

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
constraint and appearance, exports a STEP file for geometry
verification, and emits a single JSON document. The rebuilder then
walks that JSON and creates matching Tungsten CAD objects inside one
`PartDesign::Body`.

## Limitations and roadmap

- The rebuilder currently assumes sketches live on the XY/XZ/YZ
  datum planes. Custom reference planes fall back to XY.
- Edge/face selection for fillets and chamfers is set to "use all
  edges" as a first pass; refine in the GUI.
- Pattern features and assemblies are recorded in the results dialog
  but not yet reconstructed as live features.

## License

LGPL-2.1-or-later, same as the rest of Tungsten CAD.