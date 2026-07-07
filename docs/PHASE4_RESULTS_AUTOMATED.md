# Phase 4 — Automated GUI Verification Results (Partial)

**Tester**: Z (automated, headless via FreeCADCmd + Xvfb)
**Date**: 2026-07-08
**FCD version**: 1.2.0 (Git shallow, build 20260706)
**Platform**: Debian 13 (trixie) container with KDE Neon libs

## Summary

| Metric | Value |
|---|---|
| Total commands tested | 97 |
| Passed | 14 |
| Failed | 83 |
| Pass rate | 14% |

**Note**: This is an automated functional test only. The "intuitive vs Fusion 360"
comparison still requires manual testing by the user. This document covers what
can be verified headlessly: does each command create its feature object without
crashing, and does the resulting geometry have the expected properties?

## Passing commands (14)

These commands work correctly in CLI mode — the feature object is created,
recompute succeeds, and the Shape is non-null with expected properties.

| # | Command | Duration |
|---|---|---|
| 1 | `PartDesign_Body` | 224ms |
| 2 | `PartDesign_CoordinateSystem` | 220ms |
| 3 | `PartDesign_Line` | 236ms |
| 4 | `PartDesign_Plane` | 221ms |
| 5 | `PartDesign_Point` | 222ms |
| 6 | `PartDesign_Draft` | 227ms |
| 7 | `PartDesign_NewSketch` | 225ms |
| 8 | `PartDesign_PartBox` | 214ms |
| 9 | `PartDesign_PartCompound` | 212ms |
| 10 | `PartDesign_PartCone` | 218ms |
| 11 | `PartDesign_PartCylinder` | 217ms |
| 12 | `PartDesign_PartSimpleCylinder` | 214ms |
| 13 | `PartDesign_PartSphere` | 231ms |
| 14 | `PartDesign_PartTorus` | 218ms |

## Failing commands (83)

### Failure categories

| Category | Count | Root cause |
|---|---|---|
| ["unexpected error: module 'FreeCADGui' has no attribute 'commandManager'"] | 35 | See below |
| ["Shape is null after recompute"] | 20 | See below |
| (no error message — script ran but did not print TEST_PASS) | 17 | See below |
| ["Shape is null"] | 4 | See below |
| ["AdditiveHelix.Shape is null after recompute"] | 1 | See below |
| ["Groove.Shape is null after recompute"] | 1 | See below |
| ["unexpected volume: 1570.80 (expected ~1507.96)"] | 1 | See below |
| ["Pad.Shape is null after recompute"] | 1 | See below |
| ["addObject raised: Document::addObject: 'Part::Tube' is not a document object t | 1 | See below |
| ["Pocket.Shape is null after recompute"] | 1 | See below |
| FreeCADCmd exited with code 1 | 1 | See below |

### Details

#### `["unexpected error: module 'FreeCADGui' has no attribute 'commandManager'"]`

**35 commands affected:**

- `PartDesign_Chain` (123ms)
- `PartDesign_Clone` (122ms)
- `PartDesign_CompPrimitiveAdditive` (152ms)
- `PartDesign_CompPrimitiveSubtractive` (123ms)
- `PartDesign_DuplicateSelection` (119ms)
- `PartDesign_InvoluteGear` (132ms)
- `PartDesign_Migrate` (122ms)
- `PartDesign_MigratePartMacros` (121ms)
- `PartDesign_MoveFeature` (125ms)
- `PartDesign_MoveFeatureInTree` (123ms)
- `PartDesign_MoveTip` (123ms)
- `PartDesign_PartBoolean` (128ms)
- `PartDesign_PartBooleanFragments` (130ms)
- `PartDesign_PartBuilder` (125ms)
- `PartDesign_PartCheckGeometry` (124ms)
- `PartDesign_PartCompoundFilter` (129ms)
- `PartDesign_PartCrossSections` (124ms)
- `PartDesign_PartEditAttachment` (125ms)
- `PartDesign_PartExplodeCompound` (121ms)
- `PartDesign_PartJoinConnect` (121ms)
- `PartDesign_PartJoinCutout` (122ms)
- `PartDesign_PartJoinEmbed` (119ms)
- `PartDesign_PartPrimitives` (121ms)
- `PartDesign_PartProjectionOnSurface` (124ms)
- `PartDesign_PartSectionCut` (122ms)
- `PartDesign_PartShapeFromMesh` (130ms)
- `PartDesign_PartSlice` (129ms)
- `PartDesign_PartSliceApart` (124ms)
- `PartDesign_PartToleranceSet` (124ms)
- `PartDesign_PartXor` (121ms)
- `PartDesign_ShapeBinder` (122ms)
- `PartDesign_Sprocket` (122ms)
- `PartDesign_SubShapeBinder` (121ms)
- `PartDesign_WizardShaft` (126ms)
- `PartDesign_WizardShaftCallBack` (128ms)

#### `["Shape is null after recompute"]`

**20 commands affected:**

- `PartDesign_PartCommon` (219ms)
- `PartDesign_PartCut` (221ms)
- `PartDesign_PartDefeaturing` (215ms)
- `PartDesign_PartElementCopy` (213ms)
- `PartDesign_PartFaceOffset` (210ms)
- `PartDesign_PartFuse` (210ms)
- `PartDesign_PartLoft` (208ms)
- `PartDesign_PartMakeFace` (211ms)
- `PartDesign_PartMakeSolid` (211ms)
- `PartDesign_PartOffset` (217ms)
- `PartDesign_PartOffset2D` (212ms)
- `PartDesign_PartRefineShape` (213ms)
- `PartDesign_PartReverseShape` (213ms)
- `PartDesign_PartRuledSurface` (213ms)
- `PartDesign_PartScale` (212ms)
- `PartDesign_PartSection` (214ms)
- `PartDesign_PartSimpleCopy` (215ms)
- `PartDesign_PartSweep` (215ms)
- `PartDesign_PartThickness` (212ms)
- `PartDesign_PartTransformedCopy` (213ms)

#### `(no error message — script ran but did not print TEST_PASS)`

**17 commands affected:**

- `PartDesign_AdditiveCoil` (254ms)
- `PartDesign_AdditiveLoft` (244ms)
- `PartDesign_AdditivePipe` (244ms)
- `PartDesign_Boolean` (263ms)
- `PartDesign_Chamfer` (254ms)
- `PartDesign_Extrude` (157ms)
- `PartDesign_Fillet` (260ms)
- `PartDesign_LinearPattern` (246ms)
- `PartDesign_Mirrored` (266ms)
- `PartDesign_MultiTransform` (243ms)
- `PartDesign_PatternOnPath` (250ms)
- `PartDesign_PolarPattern` (244ms)
- `PartDesign_Scaled` (248ms)
- `PartDesign_SubtractiveCoil` (260ms)
- `PartDesign_SubtractiveLoft` (265ms)
- `PartDesign_SubtractivePipe` (257ms)
- `PartDesign_Thickness` (267ms)

#### `["Shape is null"]`

**4 commands affected:**

- `PartDesign_BrickTexture` (230ms)
- `PartDesign_StoneTexture` (228ms)
- `PartDesign_SubtractiveHelix` (228ms)
- `PartDesign_WoodPlankTexture` (235ms)

#### `["AdditiveHelix.Shape is null after recompute"]`

**1 commands affected:**

- `PartDesign_AdditiveHelix` (228ms)

#### `["Groove.Shape is null after recompute"]`

**1 commands affected:**

- `PartDesign_Groove` (232ms)

#### `["unexpected volume: 1570.80 (expected ~1507.96)"]`

**1 commands affected:**

- `PartDesign_Hole` (241ms)

#### `["Pad.Shape is null after recompute"]`

**1 commands affected:**

- `PartDesign_Pad` (225ms)

#### `["addObject raised: Document::addObject: 'Part::Tube' is not a document object type"]`

**1 commands affected:**

- `PartDesign_PartTube` (135ms)

#### `["Pocket.Shape is null after recompute"]`

**1 commands affected:**

- `PartDesign_Pocket` (226ms)

#### `FreeCADCmd exited with code 1`

**1 commands affected:**

- `PartDesign_Revolution` (219ms)

