# Plan: Merging Part and PartDesign Workbenches

## Executive Summary

This document outlines a detailed plan to merge the Part and PartDesign workbenches in FreeCAD/TungstenCAD. The goal is to consolidate functionality while maintaining backward compatibility, ultimately providing a simpler, more cohesive modeling experience.

**Current State:**
- Part Workbench: CSG-style direct modeling with primitives and boolean operations
- PartDesign Workbench: Feature-based parametric modeling within Body containers

**Target State:**
- Unified PartDesign workbench containing all modeling capabilities
- Part workbench deprecated but still functional for legacy files

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
| **Primitives** (standalone) | `src/Mod/Part/Gui/CommandCreate.*` | Medium | Add body-aware versions |
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
| **Check Geometry** | `src/Mod/Part/Gui/CommandCheckGeometry.*` | Low | Analysis tool - move or keep in Part |
| **Cross-sections** | `src/Mod/Part/Gui/CommandCrossSections.*` | Low | View tool - move or keep in Part |

### 1.3 Features to Keep in Part (File I/O & Analysis)

| Feature | Reason |
|---------|--------|
| **Import STEP/IGES/BREP** | File I/O is separate concern |
| **Export STEP/IGES/BREP** | File I/O is separate concern |
| **BasicShapes 2D** (Circle, Arc, Line, Point, etc.) | These are 2D elements - belong in Sketcher conceptually |
| **Primitives Dialog** | Could move to PartDesign |
| **Shape Builder** | Advanced tool - could move |
| **Face from Wires** | Sketcher-related - could move |

---

## 2. Technical Implementation

### 2.1 Architecture Overview

```
Part (Core/Foundation)
├── Part::Feature (base class for all shapes)
├── Part::BodyBase (common base for Part and PartDesign bodies)
├── OpenCASCADE bindings (geometry kernel)
└── File I/O (STEP, IGES, BREP)

PartDesign (Extended Functionality)
└── PartDesign::Body : Part::BodyBase
    ├── Feature history (parametric)
    ├── Tip control
    └── Body-aware operations
```

### 2.2 Key Files to Modify

#### A. Workbench Definition
| File | Change |
|------|--------|
| `src/Mod/PartDesign/InitGui.py` | Add all Part features to PartDesign workbench |
| `src/Mod/PartDesign/Gui/Command*.py` | Add new commands for moved features |

#### B. Core C++ Classes (If Needed)
| File | Change |
|------|--------|
| `src/Mod/PartDesign/App/Body.*` | May need to extend for new operations |
| `src/Mod/PartDesign/App/Feature.*` | May need new feature types |

#### C. Command Files to Create/Modify

**New Commands for PartDesign:**
- `PartDesign/Gui/CommandPrimitive.py` - Body-aware primitives
- `PartDesign/Gui/CommandJoin.py` - Connect, Embed, Cutout
- `PartDesign/Gui/CommandSplit.py` - Boolean Fragments, Slice, XOR
- `PartDesign/Gui/CommandCompound.py` - Compound tools
- `PartDesign/Gui/CommandOffset.py` - 3D/2D offset
- `PartDesign/Gui/CommandScale.py` - Scale feature
- `PartDesign/Gui/CommandMeshToCAD.py` - Shape from mesh
- `PartDesign/Gui/CommandDefeaturing.py` - Defeaturing

#### D. Module Structure

```
src/Mod/PartDesign/
├── App/
│   ├── Body.*              (existing)
│   ├── Feature.*          (existing)
│   ├── Pad.*, Pocket.*    (existing)
│   └── NEW: SplitFeature.*, JoinFeature.*, CompoundFeature.*
├── Gui/
│   ├── InitGui.py         (MODIFY - add new commands)
│   ├── CommandBody.*      (existing)
│   ├── CommandPrimitive.* (NEW - body-aware primitives)
│   ├── CommandBoolean.*   (existing - extend)
│   ├── CommandJoin.*      (NEW)
│   ├── CommandSplit.*     (NEW)
│   ├── CommandCompound.*   (NEW)
│   ├── CommandOffset.*    (NEW)
│   └── CommandMisc.*      (NEW - Scale, MeshToCAD, etc.)
└── Resources/
    └── icons/             (add new icons)
```

### 2.3 Backward Compatibility Strategy

1. **Keep Part Module:** Do NOT delete Part module
2. **Deprecation Warning:** Show warning when Part workbench is selected
3. **Redirect Messages:** In Part workbench, show message pointing to PartDesign
4. **File Loading:** Legacy Part workbench files must continue to load correctly
5. **API Stability:** All Part module Python APIs remain functional

### 2.4 Import Dependencies (Risk Assessment)

**Modules importing Part (328 files):**

| Module | Risk Level | Action |
|--------|------------|--------|
| Draft | High | Update to use PartDesign where applicable |
| FEM | Medium | May need updates for new features |
| OpenSCAD | Medium | May need updates |
| Sketcher | Low | Uses Part geometric types - keep as-is |
| Import/Export | Low | Keep as-is for file I/O |
| Surface | Low | Keep as-is |
| Mesh | Low | Keep as-is |

---

## 3. Implementation Phases

### Phase 1: Foundation (Weeks 1-3)

**Goal:** Set up infrastructure and add basic features

| Task | Files | Effort |
|------|-------|--------|
| Update PartDesign InitGui.py to include existing Part tools | `PartDesign/InitGui.py` | 1 week |
| Create unified toolbar structure | `PartDesign/InitGui.py` | 0.5 week |
| Add body-aware Boolean operations | `PartDesign/Gui/CommandBoolean.py` | 1 week |
| Add Section command | NEW `PartDesign/Gui/CommandSection.py` | 0.5 week |
| Add Scale command | NEW `PartDesign/Gui/CommandScale.py` | 0.5 week |

**Deliverable:** Users can access basic Part operations within PartDesign context

### Phase 2: Move Join/Split Features (Weeks 4-6)

**Goal:** Add BOPTools functionality to PartDesign

| Task | Files | Effort |
|------|-------|--------|
| Move BOPTools JoinFeatures to PartDesign | `Part/JoinFeatures.*` → `PartDesign/` | 1 week |
| Move BOPTools SplitFeatures to PartDesign | `Part/SplitFeatures.*` → `PartDesign/` | 1.5 weeks |
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

### Phase 4: Body-Aware Primitives & Cleanup (Weeks 10-12)

**Goal:** Complete feature parity and deprecate Part

| Task | Files | Effort |
|------|-------|--------|
| Add body-aware primitives | NEW `PartDesign/Gui/CommandPrimitive.py` | 1 week |
| Update documentation | Wiki pages | 0.5 week |
| Update tests | `tests/src/Mod/PartDesign/*` | 1 week |
| Add deprecation warnings | `Part/InitGui.py` | 0.5 week |
| Final integration testing | - | 1 week |

**Deliverable:** Complete feature parity, Part deprecated

### Phase 5: Post-Merge (Ongoing)

- Monitor bug reports
- Update tutorials
- Consider removing Part workbench in version +1 or +2

---

## 4. Testing Strategy

### 4.1 Unit Tests

**Existing Tests to Update:**
- `tests/src/Mod/PartDesign/` - Update for new features
- `tests/src/Mod/Part/` - May need to add compatibility tests

**New Tests to Create:**
- Body-aware primitive creation tests
- Join/Split feature tests within bodies
- Compound operations within bodies
- Offset within bodies
- Mesh-to-CAD within bodies

### 4.2 Integration Tests

1. **File Loading:** Test legacy .fcstd files with Part workbench objects
2. **Copy/Paste:** Test copying objects between Part and PartDesign
3. **Interoperability:** Test Part and PartDesign objects in same document

### 4.3 UI Tests

1. Verify all commands accessible via toolbar, menu, and keyboard shortcuts
2. Test contextual menus in different scenarios
3. Test workbench switching

---

## 5. Risks and Mitigations

| Risk | Severity | Mitigation |
|------|----------|------------|
| **Breaking existing scripts** | High | Keep Part module, add deprecation warnings |
| **Regression in boolean operations** | High | Extensive testing, especially edge cases |
| **Body/non-body confusion** | Medium | Clear UI labels, tooltips, documentation |
| **Performance regression** | Medium | Benchmark before/after, optimize if needed |
| **Dependency cascade** | Medium | Update all dependent modules before merge |
| **Long development time** | Medium | Phased approach allows stopping points |

---

## 6. Success Criteria

1. **Feature Parity:** All Part features available in PartDesign
2. **Backward Compatibility:** Legacy files load correctly, no broken APIs
3. **User Experience:** Clear, intuitive interface with good tooltips
4. **Testing:** >95% of existing tests pass, new tests for new features
5. **Documentation:** Updated wiki and internal documentation

---

## 7. Open Questions

1. **Timeline flexibility:** Is 12 weeks a hard deadline or can it extend?
2. **Breaking changes:** Are we allowed to show deprecation warnings? Remove Part workbench visibility?
3. **Resources:** How many developers will work on this?
4. **Testing priority:** Should we prioritize certain features over others?
5. **Communication:** How to announce changes to community?

---

## Appendix A: File Inventory

### Part Module Files to Reference (for moving to PartDesign)

```
src/Mod/Part/Gui/
├── CommandCreateBox.py
├── CommandCreateCylinder.py
├── CommandCreateSphere.py
├── CommandCreateCone.py
├── CommandCreateTorus.py
├── CommandCreateEllipsoid.py
├── CommandBoolean.py
├── CommandSection.py
├── CommandOffset.py
├── CommandScale.py
├── CommandCheckGeometry.py
├── CommandCrossSections.py
├── CommandShapeFromMesh.py

src/Mod/Part/
├── JoinFeatures.py
├── SplitFeatures.py
├── CompoundTools/
│   ├── CompoundFilter.py
│   ├── Explode.py
│   └── _Command*.py
├── Defeaturing.py

src/Mod/Part/BOPTools/
└── (already imported in Part/InitGui.py)
```

### PartDesign Files to Modify

```
src/Mod/PartDesign/
├── InitGui.py                    (PRIMARY - add all features here)
├── Gui/CommandBoolean.py         (extend for all boolean types)
├── Gui/CommandFillet.py          (ensure works with new features)
├── Gui/CommandChamfer.py        (ensure works with new features)
└── Gui/CommandThickness.py       (ensure works with new features)
```

---

*Document Version: 1.0*
*Created: 2026-03-15*
*For: TungstenCAD / FreeCAD Development*
