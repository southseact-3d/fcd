# Phase 4 — Interactive GUI Verification Checklist

**Branch**: `phase4/gui-verification`
**Workbench in scope**: PartDesign only
**Tester**: You (manual)
**Platform**: Linux (AppImage from Phase 0/1)
**Reference**: Fusion 360 (side-by-side comparison)

This is the checklist you work through. For each entry, do the task in both FCD and Fusion 360 (side-by-side if you have a wide monitor), then fill in `docs/PHASE4_RESULTS.md` with your findings.

---

## How to use this checklist

1. **Install the latest AppImage** from the Phase 0/1 build:
   ```bash
   chmod +x FreeCAD_1.2.0-Ubuntu-x86_64.AppImage
   ./FreeCAD_1.2.0-Ubuntu-x86_64.AppImage
   ```
2. **Open Fusion 360** in another window.
3. **For each section below**, work through the entries in order.
4. **For each entry**, do the following:
   - Read the task description
   - Do the task in FCD (count clicks, time it, note any errors/confusion)
   - Do the same task in Fusion 360 (count clicks, time it)
   - Fill in `docs/PHASE4_RESULTS.md` for that entry with:
     - FCD click count
     - Fusion 360 click count
     - FCD time-to-complete (seconds)
     - Fusion 360 time-to-complete (seconds)
     - Errors encountered in FCD (if any)
     - Subjective rating: "felt right" / "clunky" / "broken"
     - Screenshots (if anything went wrong)
     - Severity (Critical / Major / Minor / Trivial / Enhancement) if a bug
     - Notes
5. **If you find a bug**, file a GitHub Issue with:
   - Title: `[Phase 4] <short description>`
   - Body: repro steps, expected vs actual, screenshots
   - Label: `phase-4-finding`
   - Severity label: `severity-critical`, `severity-major`, `severity-minor`, `severity-trivial`, or `severity-enhancement`

---

## Severity definitions (reminder)

| Level | Definition | Example |
|---|---|---|
| **Critical** | Crash, data loss, or produces wrong geometry silently | App segfaults when clicking Pad; Pad creates a 0-volume solid without warning |
| **Major** | Feature is broken or unusable; no reasonable workaround | Fillet dialog doesn't accept edge selection; Pad preview shows wrong shape |
| **Minor** | Feature works but is confusing or has a small bug | Tooltip says "Pad" but command is "Pocket"; icon is missing for Chamfer |
| **Trivial** | Cosmetic or nitpick | Wrong spacing in dialog; icon is slightly off-center; typo in tooltip |
| **Enhancement** | Not a bug — a request for improvement vs Fusion 360 | "Fusion 360 shows a live preview while dragging; FCD doesn't" |

---

## Edge case matrix

In addition to the happy-path testing below, test 5 key commands (Pad, Pocket, Fillet, Hole, Linear Pattern) under these conditions:

| # | Document state | Screen size |
|---|---|---|
| 1 | Empty document (no Body yet) | 1920×1080 |
| 2 | Single Body with one Pad | 1920×1080 |
| 3 | Complex doc (20+ features: sketch → pad → pocket → fillet → chamfer → pattern × 3) | 1920×1080 |
| 4 | Single Body, after undo+redo × 3 | 1920×1080 |
| 5 | Single Body, after save → close → reopen | 1920×1080 |
| 6 | Single Body with one Pad | 1366×768 (small laptop) |
| 7 | Single Body with one Pad | 2560×1440 (hi-res) |

For each cell, note: does the UI render correctly? Do dialogs fit on screen? Does the viewport behave the same? File bugs as needed.

---

# Section 1 — Commands (97 entries)

For each command below, the task is:

1. **In FCD**: Switch to PartDesign workbench. Invoke the command via:
   - Menu (Part Design menu → ... → command)
   - Toolbar (click the toolbar icon)
   - Python console (`Gui.runCommand("PartDesign_<Name>")`)
2. **In Fusion 360**: Do the equivalent operation.
3. Compare clicks, time, errors.

Mark each entry with: `✓ pass` / `✗ fail (severity)` / `? n/a` (e.g., if no Fusion equivalent).

## 1.1 — Body & Sketch Management (8 commands)

### 1.1.1 — `PartDesign_Body` (New Body)
- **Fusion 360 equivalent**: Right-click component → New Body, or Assemble → New Component
- **FCD steps**:
  1. File → New
  2. Switch to PartDesign workbench (if not already)
  3. Click "New Body" toolbar icon (or Part Design menu → New Body)
  4. Verify: a Body appears in the tree view
- **Fusion steps**:
  1. New Design
  2. Assemble → New Component (or right-click → New Component)
- **Fields to fill**: FCD clicks ___, Fusion clicks ___, FCD time ___s, Fusion time ___s, errors ___, rating ___, notes ___

### 1.1.2 — `PartDesign_NewSketch` (Create Sketch)
- **Fusion 360 equivalent**: Create → Create Sketch
- **FCD steps**:
  1. With a Body active, click "Create Sketch" toolbar icon
  2. Select a plane (XY / XZ / YZ or a datum plane)
  3. Sketcher opens
- **Fusion steps**:
  1. Create → Create Sketch
  2. Select a plane
- **Fields**: ___

### 1.1.3 — `PartDesign_Migrate` (Migrate legacy doc)
- **Fusion 360 equivalent**: n/a (Fusion doesn't have legacy migration)
- **FCD steps**: Open a legacy FreeCAD file with Part-based features; Part Design menu → Migrate
- **Fields**: ___

### 1.1.4 — `PartDesign_MoveTip` (Set Tip)
- **Fusion 360 equivalent**: n/a (Fusion uses timeline ordering, no "tip" concept)
- **FCD steps**: Right-click a feature in the tree → "Set Tip"
- **Fields**: ___

### 1.1.5 — `PartDesign_DuplicateSelection` (Duplicate)
- **Fusion 360 equivalent**: Right-click → Copy, then Paste
- **FCD steps**: Select a feature; Edit → Duplicate Selection (or right-click → Duplicate)
- **Fields**: ___

### 1.1.6 — `PartDesign_MoveFeature` (Move Object To...)
- **Fusion 360 equivalent**: Drag feature to different component in timeline
- **FCD steps**: Right-click feature → "Move Object To..." → select target Body
- **Fields**: ___

### 1.1.7 — `PartDesign_MoveFeatureInTree` (Move Feature After...)
- **Fusion 360 equivalent**: Drag feature to reorder in timeline
- **FCD steps**: Right-click feature → "Move Feature After..." → select target feature
- **Fields**: ___

### 1.1.8 — `PartDesign_MigratePartMacros` (Migrate Part Macros)
- **Fusion 360 equivalent**: n/a
- **FCD steps**: Part Design menu → Migration → Migrate Part Macros... → select macro file(s)
- **Fields**: ___

## 1.2 — Datums (4 commands)

### 1.2.1 — `PartDesign_Plane` (Datum Plane)
- **Fusion 360 equivalent**: Construct → Plane (offset/parallel/at angle/through edges/etc.)
- **FCD steps**:
  1. Click "Datum Plane" icon
  2. Select attachment reference (face, edge, vertex)
  3. Choose attachment mode
  4. Set offset/angle
- **Fusion steps**:
  1. Construct → Plane → select type
  2. Select references
  3. Set offset/angle
- **Fields**: ___
- **Note**: Compare the number of plane types offered. Fusion has 9+ plane types (offset, parallel, at angle, through two edges, through three points, tangent to surface, normal to edge, at point on face, midpoint between two faces). FCD has fewer — note which ones are missing.

### 1.2.2 — `PartDesign_Line` (Datum Line / Axis)
- **Fusion 360 equivalent**: Construct → Axis
- **Fields**: ___

### 1.2.3 — `PartDesign_Point` (Datum Point)
- **Fusion 360 equivalent**: Construct → Point
- **Fields**: ___

### 1.2.4 — `PartDesign_CoordinateSystem` (Local Coordinate System)
- **Fusion 360 equivalent**: n/a (Fusion uses component origins)
- **Fields**: ___

## 1.3 — Additive Sketch-Based Features (9 commands)

### 1.3.1 — `PartDesign_Pad` (Extrude — Join)
- **Fusion 360 equivalent**: Create → Extrude → Operation: Join
- **FCD steps**:
  1. Sketch a closed profile (e.g., circle radius 10)
  2. Click "Pad" icon
  3. In the Pad task panel:
     - Type: Dimension (or Up To Face, Up To Last)
     - Length: 5mm
     - Symmetric to plane: yes/no
     - Taper: 0° (or set)
     - Reversed: yes/no
  4. Click OK
- **Fusion steps**:
  1. Sketch same profile
  2. Create → Extrude
  3. Set distance, operation (Join), taper, etc.
  4. OK
- **Fields**: FCD clicks ___, Fusion clicks ___, FCD time ___s, Fusion time ___s, errors ___, rating ___, notes ___
- **Compare**:
  - Does FCD show a live preview as you change Length? (Fusion does)
  - Does FCD's "Up To Face" mode work the same as Fusion's "Extent: To Object"?
  - Does FCD's symmetric mode work as expected?

### 1.3.2 — `PartDesign_Revolution` (Revolve — Join)
- **Fusion 360 equivalent**: Create → Revolve → Operation: Join
- **Fields**: ___
- **Compare**: How do you select the axis in each? Fusion has an explicit axis selector in the dialog; FCD uses a ReferenceAxis property — does that work intuitively?

### 1.3.3 — `PartDesign_AdditiveLoft` (Loft — Join)
- **Fusion 360 equivalent**: Create → Loft → Operation: Join
- **Fields**: ___
- **Compare**: How do you add multiple sections in each? Can you set rail curves? Transition types?

### 1.3.4 — `PartDesign_AdditivePipe` (Sweep — Join)
- **Fusion 360 equivalent**: Create → Sweep → Operation: Join
- **Fields**: ___
- **Compare**: Path selection, guide rails, twist control, scale control.

### 1.3.5 — `PartDesign_AdditiveHelix` (Coil — Join)
- **Fusion 360 equivalent**: Create → Coil → Operation: Join
- **Fields**: ___
- **Compare**: Pitch, height, revolutions, taper. Does FCD offer all the same parameters?

### 1.3.6 — `PartDesign_AdditiveCoil` (Coil variant — Join)
- **Fusion 360 equivalent**: Create → Coil (variable pitch)
- **Fields**: ___

### 1.3.7 — `PartDesign_Extrude` (Standalone Extrude, non-Body)
- **Fusion 360 equivalent**: n/a (Fusion always uses components)
- **Fields**: ___

### 1.3.8 — `PartDesign_CompPrimitiveAdditive` (Additive Primitives dropdown)
- **Fusion 360 equivalent**: Create → Primitives dropdown
- **Fields**: ___

### 1.3.9 — `PartDesign_CompPrimitiveSubtractive` (Subtractive Primitives dropdown)
- **Fusion 360 equivalent**: Modify → Primitives dropdown (cut)
- **Fields**: ___

## 1.4 — Subtractive Sketch-Based Features (8 commands)

### 1.4.1 — `PartDesign_Pocket` (Extrude — Cut)
- **Fusion 360 equivalent**: Create → Extrude → Operation: Cut
- **Fields**: ___

### 1.4.2 — `PartDesign_Hole` (Hole)
- **Fusion 360 equivalent**: Create → Hole
- **Fields**: ___
- **Compare**: This is a big one. Compare:
  - Thread types (FCD: ISO, ISO Fine, UNC, UNF, UNEF, NPT, BSP, BSW, BSF. Fusion: similar + clearance holes)
  - Hole types (simple, drilled, counterbore, countersink)
  - Depth options (dimension, through-all, to face)
  - Does FCD have a "clearance" thread type with built-in screw tables? (Fusion does)
  - Does FCD support "Hole Series" across multiple parts? (Fusion does)
  - Does FCD show a live preview of the hole with thread visualization?

### 1.4.3 — `PartDesign_Groove` (Revolve — Cut)
- **Fusion 360 equivalent**: Create → Revolve → Operation: Cut
- **Fields**: ___

### 1.4.4 — `PartDesign_SubtractiveLoft` (Loft — Cut)
- **Fusion 360 equivalent**: Create → Loft → Operation: Cut
- **Fields**: ___

### 1.4.5 — `PartDesign_SubtractivePipe` (Sweep — Cut)
- **Fusion 360 equivalent**: Create → Sweep → Operation: Cut
- **Fields**: ___

### 1.4.6 — `PartDesign_SubtractiveHelix` (Coil — Cut)
- **Fusion 360 equivalent**: Create → Coil → Operation: Cut
- **Fields**: ___

### 1.4.7 — `PartDesign_SubtractiveCoil` (Coil variant — Cut)
- **Fusion 360 equivalent**: Create → Coil (variable)
- **Fields**: ___

## 1.5 — Dress-Up Features (7 commands)

### 1.5.1 — `PartDesign_Fillet` (Fillet)
- **Fusion 360 equivalent**: Modify → Fillet
- **Fields**: ___
- **Compare**: 
  - Single-radius fillet: should be equivalent
  - **Variable-radius fillet**: Fusion supports multiple radii along an edge. Does FCD? (Test: select an edge, try to set different radii at the start vs end.)
  - **G2 continuity**: Fusion has a "Curvature" option. Does FCD?
  - **Tangent propagation**: Fusion's "Tangent chain" option. Does FCD propagate along tangent edges?
  - **Corner mode**: Fusion has sharp/round/preserved corner modes. Does FCD?

### 1.5.2 — `PartDesign_Chamfer` (Chamfer)
- **Fusion 360 equivalent**: Modify → Chamfer
- **Fields**: ___
- **Compare**:
  - Equal-distance, two-distance, distance+angle modes — both should have these
  - **Corner mode**: Fusion has sharp/round/preserved corner modes. Does FCD?

### 1.5.3 — `PartDesign_Draft` (Draft)
- **Fusion 360 equivalent**: Modify → Draft
- **Fields**: ___
- **Compare**:
  - Pull direction selection
  - Neutral plane selection
  - Per-face angle (Fusion: dynamic draft with per-face overrides. FCD?)
  - Parting line selection (Fusion supports this. FCD?)

### 1.5.4 — `PartDesign_Thickness` (Shell)
- **Fusion 360 equivalent**: Modify → Shell
- **Fields**: ___
- **Compare**: Multiple faces to remove? Inward vs outward thickness?

### 1.5.5 — `PartDesign_BrickTexture` (Brick Texture — FCD-specific)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___
- **Note**: This is an FCD-specific procedural texture feature. Just verify it works (creates a non-null shape with brick pattern on selected faces).

### 1.5.6 — `PartDesign_WoodPlankTexture` (Wood Plank Texture — FCD-specific)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

### 1.5.7 — `PartDesign_StoneTexture` (Stone Texture — FCD-specific)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

## 1.6 — Patterns & Transforms (6 commands)

### 1.6.1 — `PartDesign_Mirrored` (Mirror)
- **Fusion 360 equivalent**: Create → Mirror
- **Fields**: ___

### 1.6.2 — `PartDesign_LinearPattern` (Rectangular Pattern)
- **Fusion 360 equivalent**: Create → Rectangular Pattern
- **Fields**: ___
- **Compare**: Fusion's Rectangular Pattern does 2D (X and Y directions) in one feature. FCD's LinearPattern is 1D — to do 2D, you need to chain two LinearPatterns (or use MultiTransform). Is this intuitive?

### 1.6.3 — `PartDesign_PolarPattern` (Circular Pattern)
- **Fusion 360 equivalent**: Create → Circular Pattern
- **Fields**: ___

### 1.6.4 — `PartDesign_PatternOnPath` (Pattern on Path)
- **Fusion 360 equivalent**: Create → Pattern on Path
- **Fields**: ___
- **Compare**: Path alignment options (Tangent, Frenet). Direction control. Spacing vs Extent.

### 1.6.5 — `PartDesign_Scaled` (Scale)
- **Fusion 360 equivalent**: Modify → Scale
- **Fields**: ___

### 1.6.6 — `PartDesign_MultiTransform` (Multi-Transform — FCD-specific)
- **Fusion 360 equivalent**: n/a (Fusion stacks separate pattern features)
- **Fields**: ___
- **Note**: This is FCD-specific. Test if the chained-transform UI is intuitive or clunky vs Fusion's separate pattern features.

## 1.7 — Boolean (1 command)

### 1.7.1 — `PartDesign_Boolean` (Combine)
- **Fusion 360 equivalent**: Modify → Combine (Join/Cut/Intersect)
- **Fields**: ___

## 1.8 — Part Bridge — Primitives (9 commands)

For each, create the primitive, then verify parameters match Fusion's equivalent.

### 1.8.1 — `PartDesign_PartBox` (Box)
- **Fusion 360 equivalent**: Create → Box
- **Fields**: ___
- **Compare**: Length, width, height params. Can you set the origin point?

### 1.8.2 — `PartDesign_PartCylinder` (Cylinder)
- **Fusion 360 equivalent**: Create → Cylinder
- **Fields**: ___

### 1.8.3 — `PartDesign_PartSphere` (Sphere)
- **Fusion 360 equivalent**: Create → Sphere
- **Fields**: ___

### 1.8.4 — `PartDesign_PartCone` (Cone)
- **Fusion 360 equivalent**: Create → Cone
- **Fields**: ___

### 1.8.5 — `PartDesign_PartTorus` (Torus)
- **Fusion 360 equivalent**: Create → Torus
- **Fields**: ___

### 1.8.6 — `PartDesign_PartTube` (Tube)
- **Fusion 360 equivalent**: Create → Cylinder (with hole) — no direct Tube primitive in Fusion
- **Fields**: ___

### 1.8.7 — `PartDesign_PartPrimitives` (Primitives dialog)
- **Fusion 360 equivalent**: Create → Primitives dropdown
- **Fields**: ___

### 1.8.8 — `PartDesign_PartBuilder` (Shape Builder)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

### 1.8.9 — `PartDesign_PartSimpleCylinder` (Simple Cylinder)
- **Fusion 360 equivalent**: Create → Cylinder (non-parametric)
- **Fields**: ___

## 1.9 — Part Bridge — Boolean (5 commands)

### 1.9.1 — `PartDesign_PartBoolean` (Boolean dialog)
- **Fusion 360 equivalent**: Modify → Combine
- **Fields**: ___

### 1.9.2 — `PartDesign_PartCut` (Cut)
- **Fusion 360 equivalent**: Modify → Combine → Cut
- **Fields**: ___

### 1.9.3 — `PartDesign_PartFuse` (Union)
- **Fusion 360 equivalent**: Modify → Combine → Join
- **Fields**: ___

### 1.9.4 — `PartDesign_PartCommon` (Intersection)
- **Fusion 360 equivalent**: Modify → Combine → Intersect
- **Fields**: ___

### 1.9.5 — `PartDesign_PartSection` (Section)
- **Fusion 360 equivalent**: Modify → Split Body → Section
- **Fields**: ___

## 1.10 — Part Bridge — Join (3 commands)

### 1.10.1 — `PartDesign_PartJoinConnect` (Connect)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

### 1.10.2 — `PartDesign_PartJoinEmbed` (Embed)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

### 1.10.3 — `PartDesign_PartJoinCutout` (Cutout)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

## 1.11 — Part Bridge — Split (4 commands)

### 1.11.1 — `PartDesign_PartBooleanFragments` (Boolean Fragments)
- **Fusion 360 equivalent**: n/a (closest: Split Body)
- **Fields**: ___

### 1.11.2 — `PartDesign_PartSlice` (Slice)
- **Fusion 360 equivalent**: Modify → Split Body
- **Fields**: ___

### 1.11.3 — `PartDesign_PartSliceApart` (Slice Apart)
- **Fusion 360 equivalent**: Modify → Split Body (with separate result bodies)
- **Fields**: ___

### 1.11.4 — `PartDesign_PartXor` (XOR)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

## 1.12 — Part Bridge — Compound (4 commands)

### 1.12.1 — `PartDesign_PartCompound` (Compound)
- **Fusion 360 equivalent**: Group (in timeline)
- **Fields**: ___

### 1.12.2 — `PartDesign_PartExplodeCompound` (Explode Compound)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

### 1.12.3 — `PartDesign_PartCompoundFilter` (Compound Filter)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

### 1.12.4 — `PartDesign_PartToleranceSet` (Set Tolerance)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

## 1.13 — Part Bridge — Copy / Refine (4 commands)

### 1.13.1 — `PartDesign_PartSimpleCopy` (Simple Copy)
- **Fusion 360 equivalent**: Copy + Paste (non-parametric)
- **Fields**: ___

### 1.13.2 — `PartDesign_PartTransformedCopy` (Transformed Copy)
- **Fusion 360 equivalent**: Copy + Paste with transform
- **Fields**: ___

### 1.13.3 — `PartDesign_PartElementCopy` (Element Copy)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

### 1.13.4 — `PartDesign_PartRefineShape` (Refine Shape)
- **Fusion 360 equivalent**: n/a (Fusion does this automatically)
- **Fields**: ___

## 1.14 — Part Bridge — Offset / Surface (8 commands)

### 1.14.1 — `PartDesign_PartOffset` (3D Offset)
- **Fusion 360 equivalent**: Modify → Press Pull (on a solid)
- **Fields**: ___

### 1.14.2 — `PartDesign_PartOffset2D` (2D Offset)
- **Fusion 360 equivalent**: Sketch → Offset (within sketch)
- **Fields**: ___

### 1.14.3 — `PartDesign_PartFaceOffset` (Face Offset)
- **Fusion 360 equivalent**: Surface → Offset
- **Fields**: ___

### 1.14.4 — `PartDesign_PartRuledSurface` (Ruled Surface)
- **Fusion 360 equivalent**: Surface → Ruled
- **Fields**: ___

### 1.14.5 — `PartDesign_PartLoft` (Loft)
- **Fusion 360 equivalent**: Surface → Loft
- **Fields**: ___

### 1.14.6 — `PartDesign_PartSweep` (Sweep)
- **Fusion 360 equivalent**: Surface → Sweep
- **Fields**: ___

### 1.14.7 — `PartDesign_PartThickness` (Thickness)
- **Fusion 360 equivalent**: Modify → Shell
- **Fields**: ___

### 1.14.8 — `PartDesign_PartProjectionOnSurface` (Projection on Surface)
- **Fusion 360 equivalent**: Curve → Project to Surface
- **Fields**: ___

## 1.15 — Part Bridge — Other (7 commands)

### 1.15.1 — `PartDesign_PartEditAttachment` (Attachment)
- **Fusion 360 equivalent**: n/a (Fusion uses "Edit Feature" on construction geometry)
- **Fields**: ___

### 1.15.2 — `PartDesign_PartScale` (Scale)
- **Fusion 360 equivalent**: Modify → Scale
- **Fields**: ___

### 1.15.3 — `PartDesign_PartShapeFromMesh` (Shape from Mesh)
- **Fusion 360 equivalent**: Mesh → BRep Convert
- **Fields**: ___

### 1.15.4 — `PartDesign_PartDefeaturing` (Defeaturing)
- **Fusion 360 equivalent**: n/a (Fusion uses Remove Face)
- **Fields**: ___

### 1.15.5 — `PartDesign_PartCrossSections` (Cross-sections)
- **Fusion 360 equivalent**: Inspect → Section Analysis
- **Fields**: ___

### 1.15.6 — `PartDesign_PartCheckGeometry` (Check Geometry)
- **Fusion 360 equivalent**: Inspect → Check
- **Fields**: ___

### 1.15.7 — `PartDesign_PartSectionCut` (Section Cut)
- **Fusion 360 equivalent**: Inspect → Section Analysis (interactive)
- **Fields**: ___

## 1.16 — ShapeBinders / Clone (3 commands)

### 1.16.1 — `PartDesign_ShapeBinder` (Shape Binder)
- **Fusion 360 equivalent**: n/a (Fusion uses Derive)
- **Fields**: ___

### 1.16.2 — `PartDesign_SubShapeBinder` (Sub-Shape Binder)
- **Fusion 360 equivalent**: Derive (sub-element)
- **Fields**: ___

### 1.16.3 — `PartDesign_Clone` (Clone)
- **Fusion 360 equivalent**: Derive
- **Fields**: ___

## 1.17 — Migration / Tree Ops (2 commands — skip the 5 selection-filter aliases)

### 1.17.1 — `PartDesign_PartMakeSolid` (Convert to Solid)
- **Fusion 360 equivalent**: n/a (automatic in Fusion)
- **Fields**: ___

### 1.17.2 — `PartDesign_PartReverseShape` (Reverse Shape)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

## 1.18 — Scripted Features (4 commands)

### 1.18.1 — `PartDesign_InvoluteGear` (Involute Gear)
- **Fusion 360 equivalent**: n/a (third-party add-ins only)
- **Fields**: ___
- **Test**: Create a 20-tooth module-2 gear. Verify the geometry looks correct.

### 1.18.2 — `PartDesign_Sprocket` (Sprocket)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

### 1.18.3 — `PartDesign_Chain` (Chain)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

### 1.18.4 — `PartDesign_WizardShaft` (Shaft Wizard)
- **Fusion 360 equivalent**: n/a
- **Fields**: ___

---

# Section 2 — Viewport Interactions (15 entries)

These are the viewport controls that every CAD user uses constantly. They're not "commands" in the menu sense, but they're critical to usability.

For each, test:
- Does it work as expected?
- Is it discoverable (can a new user figure it out without docs)?
- Does it match Fusion 360's behavior?

## 2.1 — Zoom

### 2.1.1 — Zoom with mouse wheel
- **FCD**: Roll mouse wheel up/down to zoom in/out
- **Fusion 360**: Same
- **Compare**: Zoom direction (does up zoom in or out?), zoom-to-cursor vs zoom-to-center, smoothness
- **Fields**: ___

### 2.1.2 — Zoom with toolbar
- **FCD**: View → Standard Views → Zoom In / Zoom Out (or toolbar icons)
- **Fusion 360**: Navigation → Zoom In / Zoom Out
- **Fields**: ___

### 2.1.3 — Fit to view
- **FCD**: View → Fit to view (or toolbar icon, or press F)
- **Fusion 360**: Navigation → Fit
- **Fields**: ___

### 2.1.4 — Zoom to selection
- **FCD**: Right-click → View Fit, or menu option
- **Fusion 360**: Right-click → Zoom To
- **Fields**: ___

## 2.2 — Pan

### 2.2.1 — Pan with middle mouse button
- **FCD**: Hold middle mouse button + drag
- **Fusion 360**: Same
- **Fields**: ___

### 2.2.2 — Pan with keyboard
- **FCD**: Hold Shift + middle mouse, or arrow keys
- **Fusion 360**: Hold Shift + middle mouse
- **Fields**: ___

## 2.3 — Rotate

### 2.3.1 — Rotate with left mouse button (CAD navigation style)
- **FCD**: Hold left mouse button + drag (in CAD navigation style)
- **Fusion 360**: Same
- **Fields**: ___

### 2.3.2 — Rotate with middle mouse button (Blender navigation style)
- **FCD**: Switch to Blender navigation in Preferences → Display → Navigation; hold middle mouse + drag
- **Fusion 360**: n/a
- **Fields**: ___

## 2.4 — View Cube

### 2.4.1 — View cube — click face
- **FCD**: Click the Front/Top/Right face of the view cube (if present)
- **Fusion 360**: Click face of view cube
- **Compare**: Does FCD have a view cube? If yes, does it work the same?
- **Fields**: ___

### 2.4.2 — View cube — click corner/edge
- **FCD**: Click a corner or edge of the view cube for isometric view
- **Fusion 360**: Same
- **Fields**: ___

## 2.5 — Navigation Styles

### 2.5.1 — Switch navigation style (CAD / Blender / Maya / Touch)
- **FCD**: Edit → Preferences → Display → 3D View → Navigation → choose style
- **Fusion 360**: n/a (Fusion has only one style)
- **Fields**: ___
- **Test**: Switch to each style and verify the mouse behavior changes correctly.

## 2.6 — Standard Views

### 2.6.1 — Standard views (Front/Top/Right/Iso)
- **FCD**: View → Standard Views → Front (or toolbar icons, or numeric keys 1, 2, 3, etc.)
- **Fusion 360**: View → Face → Front (or view cube)
- **Fields**: ___

## 2.7 — View Save / Restore

### 2.7.1 — Save current view
- **FCD**: View → Save View (or similar)
- **Fusion 360**: View → Named Views → Save
- **Fields**: ___

### 2.7.2 — Restore saved view
- **FCD**: View → Restore View
- **Fusion 360**: View → Named Views → click saved view
- **Fields**: ___

## 2.8 — Screenshot

### 2.8.1 — Save viewport screenshot
- **FCD**: View → Save Image (or toolbar icon)
- **Fusion 360**: File → Save As Image
- **Fields**: ___

---

# Section 3 — Menus and Toolbars (10 entries)

## 3.1 — Top Menu Bar

### 3.1.1 — File menu
- **FCD**: Click File menu → verify items: New, Open, Save, Save As, Export, Import, Print, Quit
- **Fusion 360**: File menu → same items
- **Compare**: Are the items in the same order? Are there extra/missing items?
- **Fields**: ___

### 3.1.2 — Edit menu
- **FCD**: Edit → Undo, Redo, Cut, Copy, Paste, Duplicate Selection
- **Fusion 360**: Edit → Undo, Redo, Cut, Copy, Paste
- **Fields**: ___

### 3.1.3 — View menu
- **FCD**: View → Standard Views, Toolbars, Panels, etc.
- **Fusion 360**: View menu
- **Fields**: ___

### 3.1.4 — Part Design menu
- **FCD**: Part Design menu → verify all submenus (Part Primitives, Part Boolean, etc.) and items match what's documented in `docs/PARITY_MATRIX.md`
- **Fusion 360**: Equivalent menus
- **Fields**: ___

### 3.1.5 — Sketch menu
- **FCD**: Sketch menu (provided by Sketcher workbench but visible in PartDesign)
- **Fusion 360**: Sketch menu
- **Fields**: ___

## 3.2 — Toolbars

### 3.2.1 — Part Design toolbar
- **FCD**: Verify the Part Design toolbar has icons for: Body, New Sketch, Pad, Pocket, Revolution, Groove, Fillet, Chamfer, etc.
- **Fusion 360**: Equivalent toolbar
- **Fields**: ___

### 3.2.2 — Part Design Part Tools toolbar (FCD-specific)
- **FCD**: Verify the "Part Design Part Tools" toolbar (the custom widget mentioned in Workbench::activated) is visible and functional
- **Fusion 360**: n/a
- **Fields**: ___

## 3.3 — Context Menus

### 3.3.1 — Right-click in tree view (empty area)
- **FCD**: Right-click in empty tree area → verify context menu
- **Fusion 360**: Right-click in browser empty area
- **Fields**: ___

### 3.3.2 — Right-click on a feature in tree
- **FCD**: Right-click a Body/Pad/Sketch → verify context menu has Edit, Toggle Visibility, Delete, etc.
- **Fusion 360**: Right-click feature in timeline
- **Fields**: ___

### 3.3.3 — Right-click in viewport (3D view)
- **FCD**: Right-click in the 3D viewport → verify context menu
- **Fusion 360**: Right-click in viewport
- **Fields**: ___

---

# Section 4 — Dock Windows (6 entries)

## 4.1 — Combo View (Tree + Task Panel)

### 4.1.1 — Tree view (model structure)
- **FCD**: View → Panels → Tree view (if not visible). Verify it shows the model hierarchy.
- **Fusion 360**: Browser panel
- **Compare**: Does the tree show the same info? Body → Sketch → Pad → etc.?
- **Fields**: ___

### 4.1.2 — Task panel (command dialogs)
- **FCD**: When a command is active (e.g., Pad), verify the task panel shows on the side with parameters.
- **Fusion 360**: Edit Feature dialog appears on the right
- **Fields**: ___

## 4.2 — Python Console

### 4.2.1 — Open Python console
- **FCD**: View → Panels → Python console
- **Fusion 360**: Tools → Scripts and Add-Ins (different but similar)
- **Test**: Run `print("hello")` in the console. Verify output appears.
- **Fields**: ___

## 4.3 — Report View

### 4.3.1 — Open Report view
- **FCD**: View → Panels → Report view
- **Fusion 360**: n/a (Fusion uses toast notifications)
- **Test**: Trigger an error (e.g., invalid Pad profile) and verify the error appears in Report view.
- **Fields**: ___

## 4.4 — Selection View

### 4.4.1 — Open Selection view
- **FCD**: View → Panels → Selection view
- **Test**: Select several objects; verify they appear in Selection view.
- **Fields**: ___

## 4.5 — DAG View

### 4.5.1 — Open DAG view (if available)
- **FCD**: View → Panels → DAG view
- **Test**: Verify it shows the dependency graph of features.
- **Fields**: ___

## 4.6 — Dock Window Layout

### 4.6.1 — Drag dock windows to different positions
- **FCD**: Drag the Python console from the bottom to the right side. Verify it docks there.
- **Fusion 360**: n/a (Fusion's panels are fixed)
- **Fields**: ___

---

# Section 5 — Preferences Dialog (8 entries)

## 5.1 — PartDesign settings

### 5.1.1 — Edit → Preferences → Part Design
- **FCD**: Edit → Preferences → Part Design → verify settings (e.g., "Use Part Design Part Tools", "Auto-recompute")
- **Fusion 360**: Preferences → Design
- **Fields**: ___

## 5.2 — Sketcher settings

### 5.2.1 — Edit → Preferences → Sketcher
- **FCD**: Edit → Preferences → Sketcher → verify settings (grid, snap, constraints)
- **Fusion 360**: Preferences → Design → Sketch
- **Fields**: ___

## 5.3 — Display settings

### 5.3.1 — Edit → Preferences → Display
- **FCD**: Edit → Preferences → Display → verify 3D View, Colors, Navigation tabs
- **Fusion 360**: Preferences → General → Display
- **Fields**: ___

### 5.3.2 — Change background color
- **FCD**: Preferences → Display → Colors → Background → set to a custom color
- **Fusion 360**: Preferences → Display → Background Color
- **Fields**: ___

## 5.4 — Navigation settings

### 5.4.1 — Edit → Preferences → Display → Navigation
- **FCD**: Preferences → Display → 3D View → Navigation → change navigation style
- **Fusion 360**: n/a
- **Fields**: ___

## 5.5 — Workbench selector

### 5.5.1 — Verify Fusion 360 style workbench selector is enabled
- **FCD**: Preferences → Workbenches → "Uses the Fusion 360 style top tab selector" — verify this is enabled (this is the FCD fork's signature UI feature)
- **Fusion 360**: n/a
- **Fields**: ___
- **Test**: Restart FCD after enabling. Verify the workbench selector appears as a top tab bar (like Fusion) instead of a dropdown.

## 5.6 — Keyboard shortcuts

### 5.6.1 — Edit → Preferences → Keyboard
- **FCD**: Preferences → Keyboard → verify you can remap shortcuts
- **Fusion 360**: Preferences → Keyboard Shortcuts
- **Fields**: ___

## 5.7 — Save/restore preferences

### 5.7.1 — Save preferences, restart, verify they persist
- **FCD**: Change several preferences; quit FCD; reopen; verify changes persisted
- **Fusion 360**: Same
- **Fields**: ___

---

# Section 6 — Keyboard Shortcuts (20 entries)

For each, test the shortcut and compare to Fusion 360.

| # | Action | FCD default | Fusion 360 default | FCD works? | Notes |
|---|---|---|---|---|---|
| 6.1 | New document | Ctrl+N | Ctrl+N | ___ | ___ |
| 6.2 | Open file | Ctrl+O | Ctrl+O | ___ | ___ |
| 6.3 | Save | Ctrl+S | Ctrl+S | ___ | ___ |
| 6.4 | Undo | Ctrl+Z | Ctrl+Z | ___ | ___ |
| 6.5 | Redo | Ctrl+Y (or Ctrl+Shift+Z) | Ctrl+Y | ___ | ___ |
| 6.6 | Copy | Ctrl+C | Ctrl+C | ___ | ___ |
| 6.7 | Paste | Ctrl+V | Ctrl+V | ___ | ___ |
| 6.8 | Delete | Del | Del | ___ | ___ |
| 6.9 | Create sketch | S (or B for Box?) | S | ___ | ___ |
| 6.10 | Extrude/Pad | P | E | ___ | ___ |
| 6.11 | Fit to view | F | F | ___ | ___ |
| 6.12 | Standard view: Front | 1 | n/a (view cube) | ___ | ___ |
| 6.13 | Standard view: Top | 2 | n/a | ___ | ___ |
| 6.14 | Standard view: Right | 3 | n/a | ___ | ___ |
| 6.15 | Standard view: Isometric | 0 | n/a | ___ | ___ |
| 6.16 | Toggle visibility | Space | V | ___ | ___ |
| 6.17 | Edit selected feature | Enter (or double-click) | double-click | ___ | ___ |
| 6.18 | Cancel current operation | Esc | Esc | ___ | ___ |
| 6.19 | Toggle fullscreen | F11 (or Alt+F11) | F11 | ___ | ___ |
| 6.20 | Open Python console | Ctrl+Shift+P (?) | n/a | ___ | ___ |

Fill in the "FCD works?" column with `✓` / `✗` / `?` and add notes for any that don't work or differ from Fusion in a confusing way.

---

# Section 7 — Drag-and-Drop (5 entries)

## 7.1 — Drag feature in tree to reorder

### 7.1.1 — Drag a feature to a new position
- **FCD**: Click and drag a feature in the tree view to a new position within the same Body
- **Fusion 360**: Drag a feature in the timeline to a new position
- **Compare**: Does FCD allow free reordering like Fusion? Or does it only allow "Move Feature After..."?
- **Fields**: ___

## 7.2 — Drag feature to different Body

### 7.2.1 — Drag a feature from one Body to another
- **FCD**: Drag a feature from Body1 to Body2 in the tree
- **Fusion 360**: Drag a feature from one component to another in the timeline
- **Fields**: ___

## 7.3 — Drag a file from file manager into FCD

### 7.3.1 — Drag a .FCStd file from Nautilus into FCD
- **FCD**: Drag a .FCStd file from the file manager onto the FCD window
- **Fusion 360**: Drag a .f3d file onto Fusion
- **Fields**: ___

### 7.3.2 — Drag a STEP file from Nautilus into FCD
- **FCD**: Drag a .step file onto FCD
- **Fusion 360**: Drag a .step file onto Fusion
- **Fields**: ___

## 7.4 — Drag from Selection view

### 7.4.1 — Drag a selection from Selection view into the tree
- **FCD**: Select several objects; drag them from Selection view into the tree
- **Fusion 360**: n/a
- **Fields**: ___

---

# Section 8 — File Operations (6 entries)

## 8.1 — New document

### 8.1.1 — File → New
- **FCD**: File → New → verify a new untitled document opens
- **Fusion 360**: File → New Design
- **Fields**: ___

## 8.2 — Open file

### 8.2.1 — Open a .FCStd file
- **FCD**: File → Open → select a .FCStd file
- **Fusion 360**: File → Open → select a .f3d file
- **Fields**: ___

### 8.2.2 — Open a STEP file
- **FCD**: File → Open → select a .step file (or Import → STEP)
- **Fusion 360**: File → Open → select a .step file
- **Fields**: ___

### 8.2.3 — Open an IGES file
- **FCD**: File → Open → select a .iges file
- **Fusion 360**: Same
- **Fields**: ___

### 8.2.4 — Open an STL file
- **FCD**: File → Open → select a .stl file
- **Fusion 360**: Same
- **Fields**: ___

## 8.3 — Save file

### 8.3.1 — File → Save (save a new doc)
- **FCD**: File → Save → choose location → save as .FCStd
- **Fusion 360**: File → Save → save as .f3d
- **Fields**: ___

## 8.4 — Save As

### 8.4.1 — File → Save As
- **FCD**: File → Save As → save with a new name
- **Fusion 360**: Same
- **Fields**: ___

## 8.5 — Export

### 8.5.1 — Export to STEP
- **FCD**: File → Export → choose STEP format → save
- **Fusion 360**: File → Export → STEP
- **Fields**: ___

### 8.5.2 — Export to STL
- **FCD**: File → Export → choose STL format → save
- **Fusion 360**: Same
- **Fields**: ___

## 8.6 — Import

### 8.6.1 — Import a STEP file into existing doc
- **FCD**: File → Import → select STEP
- **Fusion 360**: Insert → Insert STEP
- **Fields**: ___

---

# Section 9 — Themes (3 entries)

## 9.1 — Light theme

### 9.1.1 — Switch to light theme
- **FCD**: Edit → Preferences → Display → Stylesheet → choose light theme (e.g., "FreeCAD light")
- **Fusion 360**: Preferences → Appearance → Light
- **Test**: Verify all UI elements (toolbar icons, dialog text, viewport background, tree view) are readable.
- **Fields**: ___

## 9.2 — Dark theme

### 9.2.1 — Switch to dark theme
- **FCD**: Edit → Preferences → Display → Stylesheet → choose dark theme (e.g., "FreeCAD dark")
- **Fusion 360**: Preferences → Appearance → Dark
- **Test**: Verify all UI elements are readable in dark mode. Look for: low-contrast text, white-on-white icons, dialog backgrounds that don't match.
- **Fields**: ___

## 9.3 — Classic theme (if present)

### 9.3.1 — Switch to classic theme
- **FCD**: Edit → Preferences → Display → Stylesheet → choose "FreeCAD classic" (if present)
- **Fusion 360**: n/a
- **Fields**: ___

---

# Section 10 — Edge Case Matrix

Test 5 key commands (Pad, Pocket, Fillet, Hole, Linear Pattern) under each of these conditions:

| # | Document state | Screen size | Pad | Pocket | Fillet | Hole | Linear Pattern |
|---|---|---|---|---|---|---|---|
| 10.1 | Empty document (no Body yet) | 1920×1080 | ___ | ___ | ___ | ___ | ___ |
| 10.2 | Single Body with one Pad | 1920×1080 | ___ | ___ | ___ | ___ | ___ |
| 10.3 | Complex doc (20+ features) | 1920×1080 | ___ | ___ | ___ | ___ | ___ |
| 10.4 | After undo+redo × 3 | 1920×1080 | ___ | ___ | ___ | ___ | ___ |
| 10.5 | After save → close → reopen | 1920×1080 | ___ | ___ | ___ | ___ | ___ |
| 10.6 | Single Body with one Pad | 1366×768 | ___ | ___ | ___ | ___ | ___ |
| 10.7 | Single Body with one Pad | 2560×1440 | ___ | ___ | ___ | ___ | ___ |

For each cell, mark `✓ works` / `✗ issue (severity)` / `? n/a`. For issues, file a GitHub issue and link to it from the cell.

Common things to check:
- Does the dialog render correctly at this size?
- Are all buttons accessible (not cut off)?
- Does the viewport behave the same?
- Does the tree view show the full feature list without scrolling issues?
- Are tooltips readable?

---

# Final Rollup

After completing all sections, fill in `docs/PHASE4_SUMMARY.md` with:

1. Total entries tested: ___ / 185
2. Total bugs found by severity:
   - Critical: ___
   - Major: ___
   - Minor: ___
   - Trivial: ___
   - Enhancement: ___
3. Average click-count delta (FCD clicks minus Fusion clicks): ___
4. Average time delta (FCD time minus Fusion time): ___ seconds
5. Top 10 most problematic commands (by severity × frequency):
   1. ___
   2. ___
   3. ___
   4. ___
   5. ___
   6. ___
   7. ___
   8. ___
   9. ___
   10. ___
6. Top 10 best-matching commands (lowest click/time delta, no bugs):
   1. ___
   2. ___
   3. ___
   4. ___
   5. ___
   6. ___
   7. ___
   8. ___
   9. ___
   10. ___
7. Overall PartDesign parity rating (1–5 stars): ___

---

# Appendix — Useful links

- Phase 4 plan: `docs/PHASES.md` (Phase 4 section)
- Parity matrix: `docs/PARITY_MATRIX.md`
- File bugs: https://github.com/southseact-3d/fcd/issues/new (label `phase-4-finding`)
- Latest AppImage: see Phase 0/1 workflow run artifacts
