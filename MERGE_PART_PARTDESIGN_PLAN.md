# Plan: Merging Part and PartDesign Workbenches

## Executive Summary

This document outlines a detailed plan to **merge the Part workbench INTO the PartDesign workbench** in FreeCAD/TungstenCAD. The Part workbench will be **completely removed** and all functionality moved to PartDesign.

**Current State:**
- Part Workbench: CSG-style direct modeling with primitives and boolean operations
- PartDesign Workbench: Feature-based parametric modeling within Body containers

**Target State:**
- **Part workbench REMOVED** - no longer exists as a separate workbench
- PartDesign workbench becomes the **single unified modeling workbench**
- All Part features integrated into PartDesign

**Feasibility: MEDIUM-HIGH**
- Architectural foundation already exists (PartDesign.Body inherits from Part::BodyBase)
- Significant work but achievable in phased approach

---

## 1. Feature Mapping

### 1.1 Features Already in PartDesign (No Action Needed)

| Category | Features |
|----------|----------|
| **Additive Primitives** | AdditiveBox, AdditiveCylinder, AdditiveSphere, AdditiveCone, AdditiveEllipsoid, AdditiveTorus, AdditivePrism, AdditiveWedge |
| **Subtractive Primitives** | SubtractiveBox, SubtractiveCylinder, SubtractiveSphere, SubtractiveCone, SubtractiveEllipsoid, SubtractiveTorus, SubtractivePrism, SubtractiveWedge |
| **Sketch-Based** | Pad, Pocket, Revolution, Groove |
| **Loft/Pipe** | AdditiveLoft, SubtractiveLoft, AdditivePipe, SubtractivePipe |
| **Helix** | AdditiveHelix, SubtractiveHelix |
| **Dress-up** | Fillet, Chamfer, Draft, Thickness |
| **Patterns** | Mirrored, LinearPattern, PolarPattern, MultiTransform |
| **Datum** | Plane, Line, Point, CoordinateSystem |
| **References** | ShapeBinder, SubShapeBinder, Clone |
| **Body Management** | Body, Migrate, MoveTip, MoveFeature |
| **Wizards** | Shaft Design Wizard |

### 1.2 Features to Move from Part to PartDesign

| Feature | File Location | Complexity | Notes |
|---------|--------------|------------|-------|
| **Primitives** (standalone) | `src/Mod/Part/Gui/CommandCreate.*` | Medium | Add as standalone or body-aware versions |
| **Boolean: Cut** | `src/Mod/Part/Gui/CommandBoolean.*` | Low | Already partially in PartDesign.Boolean |
| **Boolean: Union** | `src/Mod/Part/Gui/CommandBoolean.*` | Low | Already partially in PartDesign.Boolean |
| **Boolean: Intersection** | `src/Mod/Part/Gui/CommandBoolean.*` | Low | Add to PartDesign |
| **Boolean: Section** | `src/Mod/Part/Gui/CommandSection.*` | Medium | Add to PartDesign |
| **Join: Connect** | `src/Mod/Part/JoinFeatures.*` | Medium | Move BOPTools to PartDesign |
| **Join: Embed** | `src/Mod/Part/JoinFeatures.*` | Medium | Move BOPTools to PartDesign |
| **Join: Cutout** | `src/Mod/Part/JoinFeatures.*` | Medium | Move BOPTools to PartDesign |
| **Split: Boolean Fragments** | `src/Mod/Part/SplitFeatures.*` | High | Complex topology handling |
| **Split: Slice** | `src/Mod/Part/SplitFeatures.*` | High | Complex topology handling |
| **Split: Slice Apart** | `src/Mod/Part/SplitFeatures.*` | High | Complex topology handling |
| **Split: XOR** | `src/Mod/Part/SplitFeatures.*` | Medium | Move to PartDesign |
| **Compound** | `src/Mod/Part/CompoundTools/*` | Medium | Move to PartDesign |
| **Compound Filter** | `src/Mod/Part/CompoundTools/CompoundFilter.*` | Medium | Move to PartDesign |
| **Compound Explode** | `src/Mod/Part/CompoundTools/Explode.*` | Low | Move to PartDesign |
| **Offset 3D** | `src/Mod/Part/Gui/CommandOffset.*` | Medium | Add body-aware version |
| **Offset 2D** | `src/Mod/Part/Gui/CommandOffset.*` | Medium | Add body-aware version |
| **Scale** | `src/Mod/Part/Gui/CommandScale.*` | Low | Add to PartDesign |
| **Shape from Mesh** | `src/Mod/Part/Gui/CommandShapeFromMesh.*` | High | Add to PartDesign |
| **Defeaturing** | `src/Mod/Part/Defeaturing.*` | High | Add to PartDesign |
| **Check Geometry** | `src/Mod/Part/Gui/CommandCheckGeometry.*` | Low | Move to PartDesign |
| **Cross-sections** | `src/Mod/Part/Gui/CommandCrossSections.*` | Low | Move to PartDesign |
| **Primitives Dialog** | `src/Mod/Part/Gui/Dialog*` | Medium | Move to PartDesign |
| **Shape Builder** | `src/Mod/Part/Gui/CommandBuilder.*` | High | Move to PartDesign |
| **Face from Wires** | `src/Mod/Part/Gui/CommandLoft.*` | Medium | Move to PartDesign |
| **Ruled Surface** | `src/Mod/Part/Gui/CommandRuledSurface.*` | Low | Move to PartDesign |
| **2D Geometry** (Circle, Arc, Line, Point, etc.) | `src/Mod/Part/Gui/Command*` | Low | Consider moving to Sketcher or keep in Part kernel only |

### 1.3 Features to KEEP (Part Kernel - Not Workbench)

These are in the Part **module** but should NOT be part of a workbench - they're the core geometry kernel:

| Feature | Reason |
|---------|--------|
| **Part::Feature** | Core geometry class - used by all workbenches |
| **Import/Export** (STEP, IGES, BREP) | File I/O is handled by Import/Export workbenches |
| **OpenCASCADE bindings** | Core kernel - not UI |
| **Part::BodyBase** | Base class - used internally |

---

## 2. Technical Implementation

### 2.1 Architecture Overview

```
Before:
┌─────────────────┐     ┌─────────────────────┐
│   Part          │     │   PartDesign       │
│   Workbench     │     │   Workbench        │
├─────────────────┤     ├─────────────────────┤
│ - Primitives    │     │ - Body             │
│ - Boolean       │     │ - Pad/Pocket       │
│ - Join/Split    │     │ - Primitives       │
│ - Compounds     │     │ - Boolean          │
│ - 2D Geometry   │     │ - Patterns         │
│ - Import/Export │     │ - Datum            │
└─────────────────┘     └─────────────────────┘

After:
┌─────────────────────┐
│   PartDesign       │
│   Workbench        │
├─────────────────────┤
│ - Body             │
│ - ALL Part features│
│ - Primitives       │
│ - Boolean          │
│ - Join/Split       │
│ - Compounds        │
│ - Patterns         │
│ - Datum            │
│ - Import/Export    │
└─────────────────────┘

Part Module (kernel only - no workbench):
├─────────────────────┤
│ - Part::Feature    │
│ - Part::BodyBase   │
│ - OpenCASCADE      │
│ - File I/O         │
└─────────────────────┘
```

### 2.2 Files to Remove/Modify

#### A. Remove Part Workbench
| File | Action |
|------|--------|
| `src/Mod/Part/InitGui.py` | DELETE or move kernel code |
| `src/Mod/Part/Gui/` | DELETE entire directory (workbench GUI) |
| `src/Mod/Part/PartGui.*` | DELETE or move to kernel-only |

#### B. Keep Part Kernel
| File | Action |
|------|--------|
| `src/Mod/Part/App/` | KEEP - core geometry classes |
| `src/Mod/Part/JoinFeatures.py` | KEEP - move to PartDesign later |
| `src/Mod/Part/SplitFeatures.py` | KEEP - move to PartDesign later |
| `src/Mod/Part/CompoundTools/` | KEEP - move to PartDesign later |
| `src/Mod/Part/Defeaturing.py` | KEEP - move to PartDesign later |
| `src/Mod/Part/BOPTools/` | KEEP - move to PartDesign later |

#### C. PartDesign Files to Modify
| File | Change |
|------|--------|
| `src/Mod/PartDesign/InitGui.py` | Add ALL Part features |
| `src/Mod/PartDesign/Gui/` | Add new command files |

### 2.3 New Commands to Create for PartDesign

```
src/Mod/PartDesign/Gui/
├── CommandPrimitives.py      # Box, Cylinder, Sphere, Cone, Torus, etc.
├── CommandBoolean.py         # Cut, Union, Intersection, Section
├── CommandJoin.py           # Connect, Embed, Cutout
├── CommandSplit.py          # Boolean Fragments, Slice, XOR
├── CommandCompound.py       # Compound, Filter, Explode
├── CommandOffset.py         # 3D/2D Offset
├── CommandScale.py          # Scale
├── CommandMeshToCAD.py      # Shape from Mesh
├── CommandDefeaturing.py    # Defeaturing
├── CommandAnalysis.py       # Check Geometry, Cross-sections
├── CommandBuilder.py        # Shape Builder
├── CommandRuledSurface.py   # Ruled Surface
├── Command2DGeometry.py     # Circle, Arc, Line, Point (optional)
└── CommandImportExport.py   # Import/Export (optional)
```

---

## 3. Implementation Phases

### Phase 1: Foundation (Weeks 1-3)

**Goal:** Set up infrastructure, add basic features to PartDesign, start removing Part workbench

| Task | Files | Effort |
|------|-------|--------|
| Update PartDesign InitGui.py | `PartDesign/InitGui.py` | 1 week |
| Create unified toolbar structure | `PartDesign/InitGui.py` | 0.5 week |
| Add Boolean operations (Cut, Union, Intersection) | `PartDesign/Gui/CommandBoolean.py` | 1 week |
| Add Section command | NEW `PartDesign/Gui/CommandSection.py` | 0.5 week |
| Add Scale command | NEW `PartDesign/Gui/CommandScale.py` | 0.5 week |
| REMOVE Part workbench registration | DELETE `Part/InitGui.py` workbench section | 0.5 week |

**Deliverable:** Basic Part operations in PartDesign, Part workbench begins removal

### Phase 2: Move Join/Split Features (Weeks 4-6)

**Goal:** Add BOPTools functionality to PartDesign

| Task | Files | Effort |
|------|-------|--------|
| Move JoinFeatures to PartDesign | `Part/JoinFeatures.*` → `PartDesign/` | 1 week |
| Move SplitFeatures to PartDesign | `Part/SplitFeatures.*` → `PartDesign/` | 1.5 weeks |
| Create body-aware versions | NEW feature classes | 1 week |
| Test and fix edge cases | - | 0.5 week |

**Deliverable:** Advanced boolean operations available in PartDesign

### Phase 3: Move Compound & Advanced Features (Weeks 7-9)

**Goal:** Add compound tools and advanced features

| Task | Files | Effort |
|------|-------|--------|
| Move Compound tools to PartDesign | `Part/CompoundTools/*` → `PartDesign/` | 1 week |
| Add Offset 3D/2D | NEW `PartDesign/Gui/CommandOffset.py` | 1 week |
| Add Shape from Mesh | NEW `PartDesign/Gui/CommandMeshToCAD.py` | 1.5 weeks |
| Add Defeaturing | NEW `PartDesign/Gui/CommandDefeaturing.py` | 1 week |

**Deliverable:** All major Part features available in PartDesign

### Phase 4: Complete Feature Parity & Cleanup (Weeks 10-12)

**Goal:** Complete merge, remove Part workbench completely

| Task | Files | Effort |
|------|-------|--------|
| Add remaining primitives | `PartDesign/Gui/CommandPrimitives.py` | 0.5 week |
| Add Check Geometry | `PartDesign/Gui/CommandAnalysis.py` | 0.5 week |
| Add Cross-sections | `PartDesign/Gui/CommandCrossSections.py` | 0.5 week |
| Add Shape Builder | `PartDesign/Gui/CommandBuilder.py` | 1 week |
| COMPLETELY REMOVE Part workbench files | DELETE `Part/Gui/*`, `Part/InitGui.py` | 1 week |
| Update tests | `tests/src/Mod/PartDesign/*` | 1 week |
| Final integration testing | - | 1 week |

**Deliverable:** Complete merge, Part workbench fully removed

### Phase 5: Post-Merge (Ongoing)

- Monitor bug reports
- Update documentation
- Consider removing remaining Part kernel files (only if truly unused)

---

## 4. Breaking Changes & Migration Strategy

### 4.1 Breaking Changes

This merge WILL break existing:
1. **Custom workbenches** that reference Part workbench
2. **Macros** that use Part workbench commands
3. **Scripts** that switch to Part workbench
4. **Legacy files** with Part workbench objects (may need migration)

### 4.2 Migration Path for Users

1. **Auto-migration:** When opening old files, automatically convert Part objects to equivalent PartDesign where possible
2. **Warning messages:** Inform users Part workbench is removed
3. **Documentation:** Clearly document new location of all features
4. **Migration tool:** Provide one-click migration for user scripts/macros

### 4.3 API Changes

| Old Command | New Command |
|------------|-------------|
| `Part.Box` | `PartDesign.Box` |
| `Part.Cut` | `PartDesign.Boolean` (with cut mode) |
| `Part.Fuse` | `PartDesign.Boolean` (with fuse mode) |
| etc. | (similar pattern) |

---

## 5. Dependencies at Risk

**328 modules** import Part module. Key ones that will need updates:

| Module | Risk Level | Action |
|--------|------------|--------|
| Draft | High | Update to use PartDesign or keep Part kernel imports only |
| FEM | Medium | May need updates for new feature locations |
| OpenSCAD | Medium | May need updates |
| Sketcher | Low | Uses Part geometric types - keep as-is |
| Import/Export | Low | Keep as-is for file I/O |
| Surface | Low | Keep as-is |
| Mesh | Low | Keep as-is |

**Note:** The Part **kernel** (App-level classes) must remain. Only the Part **workbench** (GUI) is being removed.

---

## 6. Testing Strategy

### 6.1 Unit Tests

- Update existing PartDesign tests
- Create tests for new features
- Test legacy file loading (ensure compatibility even after workbench removal)

### 6.2 Integration Tests

1. File loading with old Part workbench objects
2. Copy/paste between documents
3. Interoperability with other workbenches

### 6.3 Migration Testing

1. Test auto-migration of old files
2. Test macro/script migration
3. Test custom workbench compatibility

---

## 7. Risks and Mitigations

| Risk | Severity | Mitigation |
|------|----------|------------|
| **Breaking existing scripts** | High | Provide migration tool, update documentation |
| **Breaking other workbenches** | High | Update Draft, FEM, OpenSCAD before merge |
| **Regression in boolean operations** | High | Extensive testing |
| **Missing features in PartDesign** | Medium | Complete feature parity before removing Part |
| **Performance regression** | Medium | Benchmark before/after |
| **Long development time** | Medium | Phased approach |

---

## 8. Success Criteria

1. **Complete Removal:** Part workbench no longer exists
2. **Feature Parity:** All Part features available in PartDesign
3. **Backward Compatibility:** Legacy files load correctly (objects remain, workbench reference removed)
4. **Testing:** All existing tests pass
5. **Documentation:** Updated docs reflect new structure

---

## 9. Open Questions

1. **2D Geometry:** Should 2D primitives (Circle, Arc, Line) move to Sketcher instead?
2. **Import/Export:** Should these move to their own workbench or stay in PartDesign?
3. **Timeline flexibility:** Is 12 weeks a hard deadline?
4. **Breaking changes:** How much backward compatibility is required?

---

## Appendix A: File Inventory

### Part Workbench Files to DELETE

```
src/Mod/Part/InitGui.py              # Workbench registration - DELETE
src/Mod/Part/Gui/                    # Entire directory - DELETE
src/Mod/Part/PartGui.*               # GUI module - DELETE or move to kernel
src/Mod/Part/TestPartGui.py          # GUI tests - DELETE or move
```

### Part Module Files to KEEP (Kernel Only)

```
src/Mod/Part/App/                    # Core geometry classes - KEEP
src/Mod/Part/Init.py                 # Kernel init - KEEP
src/Mod/Part/PartEnums.py            # Enums - KEEP
src/Mod/Part/JoinFeatures.py          # Move to PartDesign later
src/Mod/Part/SplitFeatures.py         # Move to PartDesign later
src/Mod/Part/CompoundTools/          # Move to PartDesign later
src/Mod/Part/Defeaturing.py          # Move to PartDesign later
src/Mod/Part/BOPTools/                # Move to PartDesign later
src/Mod/Part/TestPartApp.py           # Kernel tests - KEEP
```

---

*Document Version: 2.0*
*Created: 2026-03-15*
*Updated: 2026-03-15*
*For: TungstenCAD / FreeCAD Development*
