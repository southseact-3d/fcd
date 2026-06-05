## Summary

Adds sketchless hole placement to the PartDesign Hole tool, similar to Fusion 360's Hole command. Users can now create holes by selecting a face directly, without needing to create a sketch first.

## Changes

### Backend (App layer)
- **FeatureHole.h/cpp**: Added `Placement` (FromSketch/AtPoint), `PlacementFace`, `PlacementPoint`, `PlacementReferences`, and `PlacementOffsets` properties. Added `getAtPointNormal()` and `buildAtPointProfile()` helper methods. Overrode `positionByPrevious()` and `getProfileShape()` for AtPoint mode.
- **FeatureSketchBased.h**: Made `getProfileShape()` and `positionByPrevious()` virtual to enable override in Hole.

### Frontend (Gui layer)
- **Command.cpp**: Rewrote `CmdPartDesignHole::activated()` to detect face selection and create holes in AtPoint mode. Updated tooltip.
- **TaskHoleParameters**: Added Placement combo box (FromSketch / At Point) with visibility toggling for profile-related controls.

### UI
- **TaskHoleParameters.ui**: Added Placement combo as first row in the TopLayout. Existing rows shifted down.

## Behavior

| Selection | Mode | Action |
|-----------|------|--------|
| Face selected | AtPoint | Creates Hole with `Placement=AtPoint`, sets `PlacementFace` |
| Sketch selected | FromSketch | Uses existing sketch-based flow |
| Nothing selected | FromSketch | Falls back to existing `prepareProfileBased()` flow |

## Key Design Decisions

1. **Backward compatible**: `Placement` defaults to `FromSketch` - all existing files load unchanged
2. **No base class breakage**: `getProfileShape()` and `positionByPrevious()` made virtual in `ProfileBased`; no changes to Pad/Pocket/etc.
3. **Face normal as hole direction**: Uses face normal (planar) or radial direction (cylindrical), with Reversed toggle
4. **findHoles() reuse**: AtPoint mode creates a synthetic single-vertex profile that works with the existing `findHoles()` OnPoints code path

## Testing

- Existing Hole tests should pass unchanged (FromSketch mode is default)
- New AtPoint mode: select a face, hole is created at face center, verify boolean subtraction succeeds

## Related

Fixes the "No sketch is available in the document" error when trying to create holes without a pre-existing sketch.