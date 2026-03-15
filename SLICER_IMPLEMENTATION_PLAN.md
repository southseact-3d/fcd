# FreeCAD Slicer Workbench Implementation Plan

## Executive Summary

This document outlines the implementation of a comprehensive **Slicer Workbench** for FreeCAD that integrates open-source slicing engines directly into the application. The workbench will support both FDM (filament) and Resin/SLA 3D printing workflows, leveraging PrusaSlicer's open-source core (Slic3r) as the primary slicing engine.

---

## 1. Architecture Overview

### 1.1 High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     FreeCAD Slicer Workbench                     │
├─────────────────────────────────────────────────────────────────┤
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────────┐  │
│  │   FDM Mode   │  │ Resin/SLA   │  │   Preview/Viewer    │  │
│  │              │  │    Mode     │  │                      │  │
│  └──────┬───────┘  └──────┬──────┘  └──────────┬───────────┘  │
│         │                 │                    │              │
│         └─────────────────┼────────────────────┘              │
│                           │                                   │
│  ┌────────────────────────┴────────────────────────┐           │
│  │         Slicer Engine Integration Layer        │           │
│  │  ┌────────────┐ ┌────────────┐ ┌────────────┐ │           │
│  │  │ Slic3r Core│ │ Custom     │ │ Post-      │ │           │
│  │  │ (Prusa)    │ │ Algorithms │ │ Processors │ │           │
│  │  └────────────┘ └────────────┘ └────────────┘ │           │
│  └───────────────────────────────────────────────┘           │
│                           │                                   │
│  ┌────────────────────────┴────────────────────────┐           │
│  │           Output Format Generators               │           │
│  │  G-code │ CTB │ Photon │ 3MF │ SL1/SL1S        │           │
│  └────────────────────────────────────────────────┘           │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 Integration Strategy

**Primary Approach**: Embed Slic3r/PrusaSlicer core as a compiled library with FreeCAD-specific bindings.

**Secondary Approach**: Implement custom algorithms using CGAL/Clipper2 for specific operations not covered by the engine.

---

## 2. Open Source Components to Integrate

### 2.1 Core Slicing Engine

| Component | License | Repository | Purpose |
|-----------|---------|------------|---------|
| **Slic3r (Prusa Edition)** | AGPL-3.0 | github.com/prusa3d/Slic3r | Primary slicing engine |
| **libslic3r** | AGPL-3.0 | Part of PrusaSlicer | Core library for slicing operations |
| **Clipper2** | Boost Software License | github.com/AngusJohnson/Clipper2 | Polygon clipping and offsetting |
| **CGAL** | GPL/LGPL | github.com/CGAL/cgal | Computational geometry (alternative) |

### 2.2 File Format Libraries

| Component | License | Repository | Purpose |
|-----------|---------|------------|---------|
| **libnest2d** | LGPL | github.com/tamasmeszaros/libnest2d | Object nesting for resin printing |
| **CTB format** | MIT-compatible | Reverse engineering / community | Chitubox resin format |
| **3MF SDK** | BSD | github.com/3MFConsortium/lib3mf | 3MF file I/O |
| **libpng/zlib** | zlib/libpng | Standard libraries | Image compression for resin |

### 2.3 Mesh Processing

| Component | License | Repository | Purpose |
|-----------|---------|------------|---------|
| **OpenVDB** | MPL-2.0 | github.com/AcademySoftwareFoundation/openvdb | Volumetric data processing |
| **admesh** | GPL-2.0 | github.com/admesh/admesh | STL repair and validation |

---

## 3. Module Structure

### 3.1 Directory Layout

```
src/Mod/Slicer/
├── Init.py                          # Module initialization
├── InitGui.py                       # GUI workbench initialization
├── App/                             # Core application logic
│   ├── CMakeLists.txt
│   ├── Slicer.cpp                   # Main module entry
│   ├── SliceJob.cpp/h               # Slicing job container
│   ├── SliceSettings.cpp/h          # Settings/preferences
│   ├── MeshProcessor.cpp/h          # Input mesh preparation
│   ├── SlicerEngine.cpp/h           # Engine wrapper interface
│   └── PostProcessor.cpp/h          # G-code/Resin post-processing
├── Gui/                             # User interface
│   ├── CMakeLists.txt
│   ├── Workbench.cpp/h              # Workbench implementation
│   ├── TaskPanels/                  # Task panels for operations
│   │   ├── SliceSetupPanel.cpp/h
│   │   ├── SupportSettingsPanel.cpp/h
│   │   ├── InfillSettingsPanel.cpp/h
│   │   └── MaterialSettingsPanel.cpp/h
│   ├── Widgets/                     # Custom UI widgets
│   │   ├── LayerViewWidget.cpp/h    # 2D layer visualization
│   │   ├── PreviewWidget.cpp/h      # 3D toolpath preview
│   │   └── PrintBedWidget.cpp/h     # Build volume visualization
│   └── Resources/
│       ├── icons/
│       ├── ui/
│       └── translations/
├── Lib/                             # Embedded libraries
│   ├── slic3r/                      # PrusaSlicer/Slic3r core
│   ├── clipper2/                    # Polygon operations
│   ├── libnest2d/                   # Nesting for resin
│   └── ctb-writer/                  # CTB/Photon format writer
├── Presets/                         # Default configurations
│   ├── printers/
│   │   ├── fdm/
│   │   └── resin/
│   ├── materials/
│   └── print_settings/
├── PostProcessors/                  # Output generators
│   ├── GCodeWriter.cpp/h
│   ├── CTBWriter.cpp/h
│   ├── PhotonWriter.cpp/h
│   ├── ThreeMFWriter.cpp/h
│   └── SL1Writer.cpp/h
└── Tests/                           # Unit and integration tests
    ├── CMakeLists.txt
    └── test_slicer.py
```

---

## 4. Implementation Phases

### Phase 1: Foundation (Weeks 1-4)

**Goals**: Basic module structure and library integration

- [ ] Create module boilerplate (Init.py, InitGui.py)
- [ ] Set up CMake build configuration
- [ ] Integrate Clipper2 library
- [ ] Create basic Workbench skeleton
- [ ] Implement settings/preferences framework
- [ ] **Deliverable**: Slicer workbench appears in FreeCAD with basic UI

**Files to Create**:
- `src/Mod/Slicer/Init.py`
- `src/Mod/Slicer/InitGui.py`
- `src/Mod/Slicer/App/CMakeLists.txt`
- `src/Mod/Slicer/Gui/CMakeLists.txt`
- `src/Mod/Slicer/Gui/Workbench.cpp/h`

### Phase 2: Slicing Engine Integration (Weeks 5-10)

**Goals**: Core slicing functionality via Slic3r

- [ ] Integrate libslic3r (PrusaSlicer's core library)
- [ ] Create C++ wrapper classes for FreeCAD integration
- [ ] Implement mesh import (from FreeCAD Part/PartDesign objects)
- [ ] Basic slicing algorithm integration
- [ ] Layer generation and toolpath computation
- [ ] **Deliverable**: Can slice simple models and view layer data

**Files to Create**:
- `src/Mod/Slicer/App/SlicerEngine.cpp/h`
- `src/Mod/Slicer/App/MeshProcessor.cpp/h`
- `src/Mod/Slicer/App/SliceJob.cpp/h`

**Key Classes**:
```cpp
class SlicerEngine {
    // Wraps libslic3r functionality
public:
    bool initialize(const SliceSettings& settings);
    SliceResult slice(const Mesh& mesh);
    std::vector<Layer> getLayers() const;
};
```

### Phase 3: FDM Features (Weeks 11-16)

**Goals**: Complete FDM printing support

- [ ] Support generation algorithms
- [ ] Infill patterns (grid, honeycomb, gyroid, etc.)
- [ ] Perimeters and shell configuration
- [ ] Bridging detection and settings
- [ ] Temperature and speed controls
- [ ] Multi-material support (tool changes)
- [ ] G-code generation with custom post-processors
- [ ] **Deliverable**: Production-ready FDM slicing

**Files to Create**:
- `src/Mod/Slicer/App/SupportGenerator.cpp/h`
- `src/Mod/Slicer/App/InfillGenerator.cpp/h`
- `src/Mod/Slicer/PostProcessors/GCodeWriter.cpp/h`

### Phase 4: Resin/SLA Features (Weeks 17-22)

**Goals**: Complete resin printing support

- [ ] Integrate libnest2d for automatic part nesting
- [ ] Anti-aliasing and exposure settings
- [ ] Support structures for resin (different from FDM)
- [ ] Layer image generation (PNG/PWS)
- [ ] CTB format writer implementation
- [ ] Photon format writer implementation
- [ ] SL1/SL1S format support
- [ ] **Deliverable**: Production-ready resin slicing

**Files to Create**:
- `src/Mod/Slicer/App/ResinSupportGenerator.cpp/h`
- `src/Mod/Slicer/App/NestingEngine.cpp/h`
- `src/Mod/Slicer/PostProcessors/CTBWriter.cpp/h`
- `src/Mod/Slicer/PostProcessors/PhotonWriter.cpp/h`
- `src/Mod/Slicer/PostProcessors/SL1Writer.cpp/h`

### Phase 5: UI and Visualization (Weeks 23-28)

**Goals**: Complete user interface

- [ ] Build volume visualization in 3D view
- [ ] 2D layer preview with slider
-  - [ ] G-code simulation and preview
- [ ] Material/filament management panel
- [ ] Printer preset management
- [ ] Print time and material estimation display
- [ ] **Deliverable**: Polished, user-friendly interface

**Files to Create**:
- `src/Mod/Slicer/Gui/Widgets/PreviewWidget.cpp/h`
- `src/Mod/Slicer/Gui/Widgets/LayerViewWidget.cpp/h`
- `src/Mod/Slicer/Gui/Widgets/PrintBedWidget.cpp/h`
- `src/Mod/Slicer/Gui/TaskPanels/*.cpp/h`

### Phase 6: Polish and Integration (Weeks 29-32)

**Goals**: Production readiness

- [ ] Integration with FreeCAD's Material system
- [ ] Mesh repair and validation tools
- [ ] Export to SD card / OctoPrint integration
- [ ] Comprehensive test suite
- [ ] Documentation and tutorials
- [ ] Translation support
- [ ] **Deliverable**: Release-ready workbench

---

## 5. Technical Specifications

### 5.1 Settings Schema

Settings will be organized hierarchically:

```yaml
Slicer/General:
  - DefaultPrinter: "Prusa i3 MK3S"
  - DefaultMaterial: "PLA"

Slicer/FDM:
  LayerHeight: 0.2
  FirstLayerHeight: 0.24
  Perimeters: 2
  TopSolidLayers: 4
  BottomSolidLayers: 3
  Infill:
    Pattern: "grid"
    Density: 20
  Supports:
    Enabled: true
    Pattern: "zigzag"
    OverhangThreshold: 45
  Speed:
    Perimeters: 40
    Infill: 80
    Travel: 150
  Temperature:
    Nozzle: 200
    Bed: 60

Slicer/Resin:
  LayerHeight: 0.05
  ExposureTime: 6.0
  BottomLayers: 8
  BottomExposure: 60.0
  LightOffDelay: 1.0
  Supports:
    Enabled: true
    Density: 80
    TipDiameter: 0.5
  AntiAliasing: 4
  ScreenResolution: [3840, 2160]
```

### 5.2 Python API

The workbench will expose a Python API for automation:

```python
import FreeCAD
import Slicer

# Create a slice job
job = Slicer.createJob("MyPrintJob")
job.addObject(FreeCAD.ActiveDocument.getObject("Cube"))

# Configure settings
settings = Slicer.FDMSettings()
settings.layer_height = 0.2
settings.infill_density = 20
settings.support_enabled = True

# Slice and export
result = job.slice(settings)
result.exportGCode("/path/to/output.gcode")

# Or for resin
resin_settings = Slicer.ResinSettings()
resin_settings.layer_height = 0.05
resin_settings.exposure_time = 6.0

result = job.slice(resin_settings)
result.exportCTB("/path/to/output.ctb")
```

### 5.3 FreeCAD FeaturePython Objects

The workbench will create FeaturePython objects that integrate with FreeCAD's parametric workflow:

```python
class SlicerJob:
    """FeaturePython object representing a slicing job"""
    def __init__(self, obj):
        obj.addProperty("App::PropertyLinkList", "Objects", "Base", "Objects to slice")
        obj.addProperty("App::PropertyEnumeration", "Mode", "Base", "Slicing mode")
        obj.Mode = ["FDM", "Resin"]
        obj.addProperty("App::PropertyFile", "OutputPath", "Output", "Output file path")
        obj.addProperty("App::PropertyBool", "AutoSlice", "Output", "Auto-slice on parameter change")
        self.Type = "Slicer::Job"

class SliceResult:
    """FeaturePython object holding slice results"""
    def __init__(self, obj):
        obj.addProperty("App::PropertyInteger", "LayerCount", "Info", "Total layers")
        obj.addProperty("App::PropertyFloat", "PrintTime", "Info", "Estimated print time")
        obj.addProperty("App::PropertyFloat", "MaterialVolume", "Info", "Material volume in mm³")
```

---

## 6. External Dependencies

### 6.1 Build Dependencies

```cmake
# CMakeLists.txt additions
find_package(clipper2 REQUIRED)
find_package(libnest2d REQUIRED)
find_package(lib3mf REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

# Optional
find_package(opencamlib)  # Already used by CAM
find_package(OpenVDB)
```

### 6.2 Git Submodules or External Projects

Add to `src/3rdParty/` or fetch via CMake's FetchContent:

```cmake
# PrusaSlicer/libslic3r
FetchContent_Declare(
    slic3r_core
    GIT_REPOSITORY https://github.com/prusa3d/PrusaSlicer.git
    GIT_TAG        version_2.7.0
    SOURCE_SUBDIR  src
)

# Clipper2
FetchContent_Declare(
    clipper2
    GIT_REPOSITORY https://github.com/AngusJohnson/Clipper2.git
    GIT_TAG        Clipper2_1.3.0
)

# libnest2d
FetchContent_Declare(
    libnest2d
    GIT_REPOSITORY https://github.com/tamasmeszaros/libnest2d.git
    GIT_TAG        master
)
```

---

## 7. License Considerations

### 7.1 License Compatibility Matrix

| Component | License | FreeCAD Compatibility |
|-----------|---------|----------------------|
| PrusaSlicer/Slic3r | AGPL-3.0 | ⚠️ **Requires special handling** - must be optional module or linked dynamically |
| Clipper2 | Boost 1.0 | ✅ Compatible with LGPL |
| libnest2d | LGPL-3.0 | ✅ Compatible |
| lib3mf | BSD | ✅ Compatible |
| OpenVDB | MPL-2.0 | ✅ Compatible |

### 7.2 Integration Strategy for AGPL Components

**Option A: Dynamic Linking (Recommended)**
- Build PrusaSlicer's libslic3r as a separate shared library
- Load at runtime if available
- Keeps FreeCAD's core LGPL while allowing AGPL features

**Option B: Alternative Implementation**
- Use CuraEngine (LGPL) instead of PrusaSlicer
- Less feature-rich but fully compatible
- Or implement core slicing using CGAL + Clipper2

**Option C: Separate Process**
- Communicate with PrusaSlicer via command line or IPC
- No direct linking, avoids license contamination

**Recommendation**: Option A with graceful fallback to Option C

---

## 8. UI Mockups

### 8.1 Slicer Workbench Layout

```
┌──────────────────────────────────────────────────────────────┐
│  FreeCAD - Slicer Workbench                                  │
├──────────────────────────────────────────────────────────────┤
│ File Edit View Slicer Tools Help                             │
├──────────────────────────────────────────────────────────────┤
│ [Toolbar] [New Job] [Slice] [Preview] [Export]              │
├──────────────────┬───────────────────────────────────────────┤
│                  │                                           │
│  Combo View      │         3D View / Preview               │
│  ├─ Model        │                                           │
│  │  ├─ Object 1  │    [Tab: 3D View | Layers | Toolpath]   │
│  │  └─ Object 2  │                                           │
│  ├─ Print        │    ┌─────────────────────────────┐       │
│  │  Settings     │    │                             │       │
│  ├─ Supports      │    │    [3D Preview of slices]   │       │
│  ├─ Infill        │    │                             │       │
│  └─ Material      │    └─────────────────────────────┘       │
│                  │                                           │
│  Task Panel      │    [Layer Slider: |=========| ] 1/240    │
│  ├─ Presets      │                                           │
│  ├─ Settings     │    Info: Time 2h 15m | Filament 12.5g     │
│  └─ Export       │                                           │
│                  │                                           │
└──────────────────┴───────────────────────────────────────────┘
```

### 8.2 Slice Settings Panel

```
┌────────────────────────────────────┐
│ Slice Settings                     │
├────────────────────────────────────┤
│ Printer: [Prusa i3 MK3S ▼]        │
│ Material: [PLA ▼]                 │
│ Print Settings: [0.20mm ▼]        │
├────────────────────────────────────┤
│ Layers and Perimeters              │
│ ├─ Layer height: [0.20  ] mm     │
│ ├─ First layer:   [0.24  ] mm     │
│ └─ Perimeters:   [2     ]        │
├────────────────────────────────────┤
│ Infill                             │
│ ├─ Pattern: [Grid ▼]              │
│ └─ Density: [20%    ]              │
├────────────────────────────────────┤
│ Support                            │
│ ├─ [✓] Generate support           │
│ ├─ Pattern: [Zigzag ▼]            │
│ └─ Overhang: [45°   ]             │
├────────────────────────────────────┤
│ [ Slice Now ] [ Export G-code ]   │
└────────────────────────────────────┘
```

---

## 9. Testing Strategy

### 9.1 Unit Tests

- Mesh processing and validation
- Settings serialization/deserialization
- Layer generation algorithms
- Toolpath computation
- G-code/CTB output validation

### 9.2 Integration Tests

- End-to-end slicing workflows
- Import/export roundtrips
- Memory usage and performance
- Multi-object slicing

### 9.3 Test Models

Standard test suite should include:
- Calibration cubes
- Overhang test towers
- Bridge tests
- Retraction tests
- Benchy (the 3D printing benchmark)
- Various FreeCAD-generated geometries

---

## 10. Migration from Existing Workflows

### 10.1 CAM Workbench Integration

The existing CAM workbench has some 3D printing related features. We should:
- Deprecate overlapping functionality
- Provide migration path for existing Path/CAM users
- Reuse applicable post-processors where possible

### 10.2 Mesh Workbench Integration

- Leverage existing mesh import/export
- Integrate with mesh repair tools
- Share STL/OBJ handling code

---

## 11. Development Resources

### 11.1 Key References

1. **PrusaSlicer Documentation**: https://github.com/prusa3d/PrusaSlicer/wiki
2. **Slic3r Manual**: https://manual.slic3r.org/
3. **G-code Reference**: https://reprap.org/wiki/G-code
4. **CTB Format Spec**: Community reverse-engineered specs
5. **3MF Core Spec**: https://3mf.io/specification/

### 11.2 Similar Projects for Reference

- **Cura Engine**: https://github.com/Ultimaker/CuraEngine
- **Slic3r Perl (legacy)**: https://github.com/slic3r/Slic3r
- **IceSL**: https://github.com/Funatra/IceSL-github
- **Pathio**: (discontinued but good reference)

---

## 12. Risk Assessment

| Risk | Impact | Mitigation |
|------|--------|------------|
| AGPL license conflicts | High | Dynamic linking or alternative engine |
| Build complexity | Medium | Clear build instructions, CI/CD integration |
| Performance issues | Medium | Benchmark suite, optimization passes |
| Memory usage | Medium | Streaming processing for large models |
| Maintenance burden | Medium | Clear module boundaries, good documentation |

---

## 13. Success Criteria

The implementation will be considered successful when:

1. ✅ Users can slice models for both FDM and resin printers entirely within FreeCAD
2. ✅ Output works correctly on at least 5 major printer brands (Prusa, Creality, Elegoo, Anycubic, Phrozen)
3. ✅ Slicing results are comparable to PrusaSlicer/Cura
4. ✅ UI is intuitive for users familiar with FreeCAD
5. ✅ Performance is within 20% of standalone slicers
6. ✅ All code is properly licensed and documented

---

## 14. Next Steps

### Immediate Actions

1. **License Review**: Confirm FreeCAD Foundation's stance on AGPL integration
2. **Prototype**: Build minimal proof-of-concept with dynamic PrusaSlicer loading
3. **Community Feedback**: Post plan to FreeCAD forum for community input
4. **Resource Planning**: Estimate build server and development time requirements

### Week 1 Tasks

- [ ] Create feature branch: `feature/slicer-workbench`
- [ ] Set up basic module structure
- [ ] Create CMake configuration
- [ ] Write initial workbench skeleton
- [ ] Test build integration

---

## Appendix A: Sample Code Snippets

### A.1 Minimal Workbench Implementation

```cpp
// src/Mod/Slicer/Gui/Workbench.h
#ifndef SLICER_WORKBENCH_H
#define SLICER_WORKBENCH_H

#include <Gui/Workbench.h>

namespace SlicerGui {

class Workbench : public Gui::Workbench {
    TYPESYSTEM_HEADER_WITH_OVERRIDE();
public:
    Workbench();
    ~Workbench() override;
    
    void activated() override;
    void deactivated() override;
    void setupContextMenu(const char* recipient, Gui::MenuItem* item) override;
    Gui::MenuItem* setupMenuBar() override;
    Gui::ToolBarItem* setupToolBars() override;
    Gui::DockWindowItems* setupDockWindows() const override;
};

} // namespace SlicerGui

#endif // SLICER_WORKBENCH_H
```

### A.2 Slicing Command Example

```cpp
// Command to slice selected objects
class CmdSlicerSlice : public Gui::Command {
public:
    CmdSlicerSlice() : Command("Slicer_Slice") {
        sAppModule = "Slicer";
        sGroup = QT_TR_NOOP("Slicer");
        sMenuText = QT_TR_NOOP("Slice Objects");
        sToolTipText = QT_TR_NOOP("Slice selected objects for 3D printing");
        sWhatsThis = "Slicer_Slice";
        sStatusTip = sToolTipText;
        sPixmap = "Slicer_Slice";
        sAccel = "S, S";
    }

    void activated(int) override {
        auto selection = getSelection().getObjectsOfType<App::Part>();
        if (selection.empty()) {
            QMessageBox::warning(Gui::getMainWindow(),
                QObject::tr("No selection"),
                QObject::tr("Please select at least one object to slice."));
            return;
        }
        
        // Create slice job
        auto job = createSliceJob(selection);
        
        // Open settings dialog
        SliceDialog dlg(job, Gui::getMainWindow());
        if (dlg.exec() == QDialog::Accepted) {
            job->execute();
        }
    }
};
```

---

## Appendix B: Step-by-Step Implementation Plan

### Phase 1: Foundation (Weeks 1-4)

#### Week 1: Project Setup
1. Create feature branch: `feature/slicer-workbench` in the FreeCAD repository
2. Create directory structure: `src/Mod/Slicer/`
3. Create `src/Mod/Slicer/Init.py` - Module initialization file with version info and module metadata
4. Create `src/Mod/Slicer/InitGui.py` - GUI workbench initialization with workbench registration
5. Create `src/Mod/Slicer/__init__.py` - Python package init file

#### Week 2: CMake Build Configuration
6. Create `src/Mod/Slicer/CMakeLists.txt` - Main build configuration
7. Create `src/Mod/Slicer/App/CMakeLists.txt` - Application layer build config
8. Create `src/Mod/Slicer/Gui/CMakeLists.txt` - GUI layer build config
9. Add Slicer module to `src/Mod/CMakeLists.txt` 
10. Configure compiler flags and include directories
11. Add dependency detection for required libraries

#### Week 3: Clipper2 Integration
12. Add Clipper2 as git submodule in `src/3rdParty/clipper2`
13. Create `FindClipper2.cmake` or use CMake's FetchContent
14. Integrate Clipper2 into App/CMakeLists.txt
15. Create test program to verify Clipper2 linkage
16. Add basic polygon operations wrapper class

#### Week 4: Workbench Skeleton
17. Create `src/Mod/Slicer/Gui/Workbench.cpp` - Workbench class implementation
18. Create `src/Mod/Slicer/Gui/Workbench.h` - Workbench header
19. Implement `activated()` and `deactivated()` methods
20. Create basic toolbar with placeholder buttons
21. Create basic menu bar structure
22. Register workbench in FreeCAD's workbench manager
23. Build and verify workbench appears in FreeCAD
24. Create placeholder icons for toolbar buttons

### Phase 2: Slicing Engine Integration (Weeks 5-10)

#### Week 5: PrusaSlicer Core Preparation
25. Clone or add PrusaSlicer as external dependency
26. Build libslic3r as standalone library
27. Create `FindSlic3r.cmake` module
28. Configure library include paths
29. Verify static library compilation

#### Week 6: C++ Wrapper Classes
30. Create `src/Mod/Slicer/App/SlicerEngine.h` - Engine interface header
31. Create `src/Mod/Slicer/App/SlicerEngine.cpp` - Engine wrapper implementation
32. Implement `initialize()` method for engine setup
33. Implement `configure()` method for settings
34. Implement `validateMesh()` method
35. Create `SliceJob` class structure

#### Week 7: Mesh Processing
36. Create `src/Mod/Slicer/App/MeshProcessor.h` - Mesh processor header
37. Create `src/Mod/Slicer/App/MeshProcessor.cpp` - Mesh processor implementation
38. Implement mesh conversion from FreeCAD geometry
39. Implement mesh validation and repair integration
40. Implement mesh export to formats Slic3r accepts
41. Create tessellation bridge between FreeCAD and Slic3r

#### Week 8: Basic Slicing Integration
42. Create `src/Mod/Slicer/App/SliceJob.h` - Slice job header
43. Create `src/Mod/Slicer/App/SliceJob.cpp` - Slice job implementation
44. Implement job creation from selected objects
45. Implement settings application to engine
46. Implement slice execution
47. Implement layer data retrieval

#### Week 9: Slice Result Handling
48. Create `src/Mod/Slicer/App/SliceResult.h` - Result data structure
49. Create `src/Mod/Slicer/App/SliceResult.cpp` - Result implementation
50. Implement layer data storage
51. Implement print time estimation retrieval
52. Implement material volume calculation
53. Implement result serialization

#### Week 10: Basic UI Integration
54. Create simple slice command
55. Create basic settings dialog
56. Integrate slicing with toolbar buttons
57. Display basic slice results in console
58. Test end-to-end basic slicing workflow

### Phase 3: FDM Features (Weeks 11-16)

#### Week 11: Support Generation
59. Create `src/Mod/Slicer/App/SupportGenerator.h` - Support generator header
60. Create `src/Mod/Slicer/App/SupportGenerator.cpp` - Support generator implementation
61. Implement overhang detection algorithm
62. Implement support point placement
63. Implement support structure generation
64. Integrate support generation with Slic3r

#### Week 12: Infill System
65. Create `src/Mod/Slicer/App/InfillGenerator.h` - Infill generator header
66. Create `src/Mod/Slicer/App/InfillGenerator.cpp` - Infill generator implementation
67. Implement grid infill pattern
68. Implement honeycomb infill pattern
69. Implement gyroid infill pattern
70. Implement infill density control

#### Week 13: Shell and Perimeters
71. Implement perimeter generation settings
72. Implement top/bottom solid layer settings
73. Implement bridging detection
74. Implement bridge angle optimization
75. Implement spiral vase mode

#### Week 14: Print Settings Integration
76. Create complete settings panel UI
77. Implement layer height configuration
78. Implement first layer height settings
79. Implement perimeter count settings
80. Implement speed settings UI
81. Implement temperature settings UI

#### Week 15: Multi-Material and Advanced
82. Implement multi-material support
83. Implement color change commands
84. Implement tool change handling
85. Implement wipe tower generation
86. Implement advanced speed settings (acceleration, jerk)

#### Week 16: G-Code Post-Processing
87. Create `src/Mod/Slicer/PostProcessors/GCodeWriter.h` - G-code writer header
88. Create `src/Mod/Slicer/PostProcessors/GCodeWriter.cpp` - G-code writer implementation
89. Implement G-code header generation
90. Implement layer change handling
91. Implement G-code footer generation
92. Implement custom start/end scripts
93. Test G-code output on real printer

### Phase 4: Resin/SLA Features (Weeks 17-22)

#### Week 17: Resin Engine Preparation
94. Research resin slicing algorithms
95. Create resin-specific settings structure
96. Configure slice settings for SLA mode

#### Week 18: libnest2d Integration
97. Add libnest2d as dependency
98. Create `src/Mod/Slicer/App/NestingEngine.h` - Nesting engine header
99. Create `src/Mod/Slicer/App/NestingEngine.cpp` - Nesting engine implementation
100. Implement automatic part placement
101. Implement nesting optimization
102. Test multi-part nesting

#### Week 19: Resin Support Structures
103. Create `src/Mod/Slicer/App/ResinSupportGenerator.h` - Resin support header
104. Create `src/Mod/Slicer/App/ResinSupportGenerator.cpp` - Resin support implementation
105. Implement resin support point calculation
106. Implement support density control
107. Implement support tip diameter settings
108. Generate support mesh geometry

#### Week 20: Layer Image Generation
109. Create layer image renderer
110. Implement anti-aliasing (2x, 4x settings)
111. Implement image resolution settings
112. Implement exposure time calculation
113. Implement bottom layer exposure settings

#### Week 21: CTB Format Implementation
114. Research Chitubox format specification
115. Create `src/Mod/Slicer/PostProcessors/CTBWriter.h` - CTB writer header
116. Create `src/Mod/Slicer/PostProcessors/CTBWriter.cpp` - CTB writer implementation
117. Implement CTB file header writing
118. Implement layer data encoding
119. Implement preview image embedding
120. Test CTB output with printer

#### Week 22: Additional Resin Formats
121. Create `src/Mod/Slicer/PostProcessors/PhotonWriter.h` - Photon writer header
122. Create `src/Mod/Slicer/PostProcessors/PhotonWriter.cpp` - Photon writer implementation
123. Create `src/Mod/Slicer/PostProcessors/SL1Writer.h` - SL1 writer header
124. Create `src/Mod/Slicer/PostProcessors/SL1Writer.cpp` - SL1 writer implementation
125. Implement SL1 format support
126. Test all resin formats

### Phase 5: UI and Visualization (Weeks 23-28)

#### Week 23: Build Volume Visualization
127. Create `src/Mod/Slicer/Gui/Widgets/PrintBedWidget.h` - Print bed widget header
128. Create `src/Mod/Slicer/Gui/Widgets/PrintBedWidget.cpp` - Print bed widget implementation
129. Implement 3D build volume display
130. Implement bed size visualization
131. Implement object placement on bed

#### Week 24: Layer Preview
132. Create `src/Mod/Slicer/Gui/Widgets/LayerViewWidget.h` - Layer view header
133. Create `src/Mod/Slicer/Gui/Widgets/LayerViewWidget.cpp` - Layer view implementation
134. Implement 2D layer visualization
135. Implement layer slider control
136. Implement layer navigation

#### Week 25: 3D Toolpath Preview
137. Create `src/Mod/Slicer/Gui/Widgets/PreviewWidget.h` - Preview widget header
138. Create `src/Mod/Slicer/Gui/Widgets/PreviewWidget.cpp` - Preview widget implementation
139. Implement 3D toolpath rendering
140. Implement color-coded speed display
141. Implement travel move visualization

#### Week 26: Task Panels
142. Create `src/Mod/Slicer/Gui/TaskPanels/SliceSetupPanel.h` - Setup panel header
143. Create `src/Mod/Slicer/Gui/TaskPanels/SliceSetupPanel.cpp` - Setup panel implementation
144. Create `src/Mod/Slicer/Gui/TaskPanels/SupportSettingsPanel.h`
145. Create `src/Mod/Slicer/Gui/TaskPanels/SupportSettingsPanel.cpp`
146. Create `src/Mod/Slicer/Gui/TaskPanels/InfillSettingsPanel.h`
147. Create `src/Mod/Slicer/Gui/TaskPanels/InfillSettingsPanel.cpp`
148. Create `src/Mod/Slicer/Gui/TaskPanels/MaterialSettingsPanel.h`
149. Create `src/Mod/Slicer/Gui/TaskPanels/MaterialSettingsPanel.cpp`

#### Week 27: Preset Management
150. Create printer preset system
151. Create material/filament preset system
152. Create print settings preset system
153. Implement preset import/export
154. Create default preset files

#### Week 28: Statistics and Export
155. Implement print time estimation display
156. Implement material volume display
157. Implement cost estimation
158. Create export dialog with format selection
159. Implement file save functionality

### Phase 6: Polish and Integration (Weeks 29-32)

#### Week 29: Material System Integration
160. Integrate with FreeCAD Material system
161. Create material database
162. Implement material properties
163. Support custom material profiles

#### Week 30: Mesh Repair Tools
164. Create mesh validation utilities
165. Integrate admesh for repairs
166. Implement hole filling
167. Implement normal correction
168. Implement manifold fixing

#### Week 31: External Integration
169. Implement OctoPrint API integration
170. Implement file transfer to SD card
171. Create printer connection system
172. Implement print monitoring basics

#### Week 32: Testing and Documentation
173. Create comprehensive unit test suite
174. Create integration test cases
175. Perform memory leak testing
176. Perform performance benchmarking
177. Write user documentation
178. Write developer documentation
179. Prepare release notes
180. Final code review and cleanup

---

## Appendix C: File Creation Checklist

### Core Module Files (Weeks 1-4)
- [ ] `src/Mod/Slicer/Init.py`
- [ ] `src/Mod/Slicer/InitGui.py`
- [ ] `src/Mod/Slicer/__init__.py`
- [ ] `src/Mod/Slicer/CMakeLists.txt`
- [ ] `src/Mod/Slicer/App/CMakeLists.txt`
- [ ] `src/Mod/Slicer/Gui/CMakeLists.txt`
- [ ] `src/Mod/Slicer/Gui/Workbench.cpp`
- [ ] `src/Mod/Slicer/Gui/Workbench.h`

### Engine Integration Files (Weeks 5-10)
- [ ] `src/Mod/Slicer/App/SlicerEngine.h`
- [ ] `src/Mod/Slicer/App/SlicerEngine.cpp`
- [ ] `src/Mod/Slicer/App/MeshProcessor.h`
- [ ] `src/Mod/Slicer/App/MeshProcessor.cpp`
- [ ] `src/Mod/Slicer/App/SliceJob.h`
- [ ] `src/Mod/Slicer/App/SliceJob.cpp`
- [ ] `src/Mod/Slicer/App/SliceResult.h`
- [ ] `src/Mod/Slicer/App/SliceResult.cpp`

### FDM Feature Files (Weeks 11-16)
- [ ] `src/Mod/Slicer/App/SupportGenerator.h`
- [ ] `src/Mod/Slicer/App/SupportGenerator.cpp`
- [ ] `src/Mod/Slicer/App/InfillGenerator.h`
- [ ] `src/Mod/Slicer/App/InfillGenerator.cpp`
- [ ] `src/Mod/Slicer/PostProcessors/GCodeWriter.h`
- [ ] `src/Mod/Slicer/PostProcessors/GCodeWriter.cpp`

### Resin Feature Files (Weeks 17-22)
- [ ] `src/Mod/Slicer/App/NestingEngine.h`
- [ ] `src/Mod/Slicer/App/NestingEngine.cpp`
- [ ] `src/Mod/Slicer/App/ResinSupportGenerator.h`
- [ ] `src/Mod/Slicer/App/ResinSupportGenerator.cpp`
- [ ] `src/Mod/Slicer/PostProcessors/CTBWriter.h`
- [ ] `src/Mod/Slicer/PostProcessors/CTBWriter.cpp`
- [ ] `src/Mod/Slicer/PostProcessors/PhotonWriter.h`
- [ ] `src/Mod/Slicer/PostProcessors/PhotonWriter.cpp`
- [ ] `src/Mod/Slicer/PostProcessors/SL1Writer.h`
- [ ] `src/Mod/Slicer/PostProcessors/SL1Writer.cpp`

### UI Files (Weeks 23-28)
- [ ] `src/Mod/Slicer/Gui/Widgets/PrintBedWidget.h`
- [ ] `src/Mod/Slicer/Gui/Widgets/PrintBedWidget.cpp`
- [ ] `src/Mod/Slicer/Gui/Widgets/LayerViewWidget.h`
- [ ] `src/Mod/Slicer/Gui/Widgets/LayerViewWidget.cpp`
- [ ] `src/Mod/Slicer/Gui/Widgets/PreviewWidget.h`
- [ ] `src/Mod/Slicer/Gui/Widgets/PreviewWidget.cpp`
- [ ] `src/Mod/Slicer/Gui/TaskPanels/SliceSetupPanel.h`
- [ ] `src/Mod/Slicer/Gui/TaskPanels/SliceSetupPanel.cpp`
- [ ] `src/Mod/Slicer/Gui/TaskPanels/SupportSettingsPanel.h`
- [ ] `src/Mod/Slicer/Gui/TaskPanels/SupportSettingsPanel.cpp`
- [ ] `src/Mod/Slicer/Gui/TaskPanels/InfillSettingsPanel.h`
- [ ] `src/Mod/Slicer/Gui/TaskPanels/InfillSettingsPanel.cpp`
- [ ] `src/Mod/Slicer/Gui/TaskPanels/MaterialSettingsPanel.h`
- [ ] `src/Mod/Slicer/Gui/TaskPanels/MaterialSettingsPanel.cpp`

### Preset Files
- [ ] `src/Mod/Slicer/Presets/printers/fdm/*.def`
- [ ] `src/Mod/Slicer/Presets/printers/resin/*.def`
- [ ] `src/Mod/Slicer/Presets/materials/*.def`
- [ ] `src/Mod/Slicer/Presets/print_settings/*.def`

### Icon Resources
- [ ] `src/Mod/Slicer/Gui/Resources/icons/Slicer_NewJob.svg`
- [ ] `src/Mod/Slicer/Gui/Resources/icons/Slicer_Slice.svg`
- [ ] `src/Mod/Slicer/Gui/Resources/icons/Slicer_Preview.svg`
- [ ] `src/Mod/Slicer/Gui/Resources/icons/Slicer_Export.svg`
- [ ] `src/Mod/Slicer/Gui/Resources/icons/Slicer_Settings.svg`

---

## Appendix D: Dependency Build Order

1. **Level 0 - No Dependencies**
   - Clipper2
   - zlib
   - libpng

2. **Level 1 - Level 0 Dependencies**
   - libnest2d (uses Clipper2)
   - lib3mf

3. **Level 2 - Level 1 Dependencies**
   - PrusaSlicer/libslic3r (uses Clipper2, libpng, zlib)
   - admesh

4. **Level 3 - FreeCAD Dependencies**
   - OpenVDB (optional)

5. **Slicer Module**
   - Links against all above

---

**Document Version**: 1.1  
**Last Updated**: 2025-01-XX  
**Author**: Implementation Team  
**Reviewers**: TBD
