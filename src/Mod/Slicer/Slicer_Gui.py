"""
SlicerWorkbench - GUI Commands Module
Main commands file registering all slicer workbench commands with FreeCAD.

License: LGPL-2.1-or-later
"""

import os
import sys
import traceback

import FreeCAD
import FreeCADGui

from PySide6 import QtCore, QtGui, QtWidgets

_SlicerCore = None
_SlicerUI = None

try:
    from SlicerCore.fdm_slicer import FDMSlicer, StandaloneSupportGenerator, GcodeGenerator
    from SlicerCore.resin_slicer import ResinSlicer
    from SlicerCore.hollowing import MeshHollower, CavityDetector
    from SlicerCore.gcode_parser import GcodeParser
    from SlicerCore.mesh_io import (
        get_selected_meshes,
        get_document_meshes,
        load_mesh_file,
        get_mesh_from_object,
    )
    _SlicerCore = True
except ImportError as exc:
    FreeCAD.Console.PrintWarning(
        f"[Slicer] SlicerCore imports unavailable: {exc}\n"
    )

try:
    from SlicerUI import (
        SupportPaintWidget,
        SlicerPreferencesDialog,
    )
    from SlicerUI.slicer_task_panel import SlicerTaskPanel
    from SlicerUI.slicer_preview_widget import SlicerPreviewWidget
    from SlicerUI.gcode_preview_widget import GcodePreviewWidget
    _SlicerUI = True
except ImportError as exc:
    FreeCAD.Console.PrintWarning(
        f"[Slicer] SlicerUI imports unavailable: {exc}\n"
    )

try:
    import Mesh
except ImportError:
    Mesh = None


def _import_mesh_file(path, doc=None):
    """Import a mesh file into the active document. Returns the created object."""
    if doc is None:
        doc = FreeCAD.ActiveDocument
    if doc is None:
        return None
    try:
        import Mesh
        Mesh.insert(path, doc.Name)
        doc.recompute()
        # Mesh.insert returns None; find the created object by filename stem
        obj_name = os.path.splitext(os.path.basename(path))[0]
        return doc.getObject(obj_name)
    except Exception:
        FreeCAD.Console.PrintError(
            f"[Slicer] Failed to import mesh: {traceback.format_exc()}\n"
        )
        return None


def _apply_mesh_to_object(obj, mesh_data):
    """Apply MeshData result to an existing FreeCAD mesh object."""
    if obj is None or mesh_data is None:
        return
    try:
        import Mesh as MeshModule
        if hasattr(mesh_data, 'vertices') and hasattr(mesh_data, 'triangles'):
            mesh = MeshModule.Mesh(mesh_data.vertices, mesh_data.triangles)
            obj.Mesh = mesh
        elif hasattr(mesh_data, 'Mesh'):
            obj.Mesh = mesh_data.Mesh
        FreeCAD.ActiveDocument.recompute()
    except Exception:
        FreeCAD.Console.PrintError(
            f"[Slicer] Failed to apply mesh: {traceback.format_exc()}\n"
        )


# Use local helpers in place of missing SlicerCore.MeshUtils functions
import_mesh_file = _import_mesh_file
apply_mesh_to_object = _apply_mesh_to_object
selection_to_meshes = lambda: []


# ---------------------------------------------------------------------------
#  Worker threads for non-blocking slicing / processing
# ---------------------------------------------------------------------------

class _SliceWorker(QtCore.QThread):
    """Background thread for FDM slicing."""
    progress = QtCore.Signal(int, str)
    finished = QtCore.Signal(dict)
    error = QtCore.Signal(str)

    def __init__(self, slicer, mesh_data, settings=None, parent=None):
        super().__init__(parent)
        self.slicer = slicer
        self.mesh_data = mesh_data
        self.settings = settings

    def run(self):
        try:
            self.progress.emit(0, "Slicing...")
            result = self.slicer.slice(self.mesh_data, self.settings)
            self.finished.emit({
                "estimated_time": getattr(result, "estimated_time", "N/A"),
                "material_used": getattr(result, "material_used", "N/A"),
                "layer_count": getattr(result, "layer_count", 0),
                "gcode": getattr(result, "gcode", ""),
            })
        except Exception as e:
            self.error.emit(str(e))


class _ResinSliceWorker(QtCore.QThread):
    """Background thread for resin slicing."""
    progress = QtCore.Signal(int, str)
    finished = QtCore.Signal(dict)
    error = QtCore.Signal(str)

    def __init__(self, slicer, triangles, settings=None, parent=None):
        super().__init__(parent)
        self.slicer = slicer
        self.triangles = triangles
        self.settings = settings

    def run(self):
        try:
            self.progress.emit(0, "Resin slicing...")
            result = self.slicer.slice(self.triangles, self.settings)
            self.finished.emit({
                "estimated_time": getattr(result, "estimated_time", "N/A"),
                "material_used": getattr(result, "material_used", "N/A"),
                "layer_count": getattr(result, "layer_count", 0),
                "layers": getattr(result, "layers", []),
                "islands": getattr(result, "islands", []),
            })
        except Exception as e:
            self.error.emit(str(e))


class _SupportWorker(QtCore.QThread):
    """Background thread for auto support generation."""
    progress = QtCore.Signal(int, str)
    finished = QtCore.Signal(object)
    error = QtCore.Signal(str)

    def __init__(self, generator, mesh_data, settings=None, parent=None):
        super().__init__(parent)
        self.generator = generator
        self.mesh_data = mesh_data
        self.settings = settings

    def run(self):
        try:
            self.progress.emit(0, "Generating supports...")
            result = self.generator.generate_support(self.mesh_data, self.settings)
            self.finished.emit(result)
        except Exception as e:
            self.error.emit(str(e))


class _HollowWorker(QtCore.QThread):
    """Background thread for mesh hollowing."""
    progress = QtCore.Signal(int, str)
    finished = QtCore.Signal(object)
    error = QtCore.Signal(str)

    def __init__(self, hollower, mesh_data, settings=None, parent=None):
        super().__init__(parent)
        self.hollower = hollower
        self.mesh_data = mesh_data
        self.settings = settings

    def run(self):
        try:
            self.progress.emit(0, "Hollowing mesh...")
            result = self.hollower.hollow(self.mesh_data, self.settings)
            self.finished.emit(result)
        except Exception as e:
            self.error.emit(str(e))


class _CavityWorker(QtCore.QThread):
    """Background thread for cavity detection."""
    progress = QtCore.Signal(int, str)
    finished = QtCore.Signal(object)
    error = QtCore.Signal(str)

    def __init__(self, detector, mesh_data, settings=None, parent=None):
        super().__init__(parent)
        self.detector = detector
        self.mesh_data = mesh_data
        self.settings = settings or {}

    def run(self):
        try:
            self.progress.emit(0, "Detecting cavities...")
            resolution = self.settings.get("detection_resolution", 0.5)
            cavities = self.detector.detect_cavities(self.mesh_data, resolution)
            self.finished.emit({
                "cavities": [{"volume": c.volume, "center": (c.center_point.x, c.center_point.y, c.center_point.z)} for c in cavities],
                "suggested_drain_holes": [],
            })
        except Exception as e:
            self.error.emit(str(e))


# ---------------------------------------------------------------------------
#  Helper utilities
# ---------------------------------------------------------------------------

def _get_selected_mesh_objects():
    """Return list of selected document objects that carry mesh data."""
    sel = FreeCADGui.Selection.getSelectionEx()
    objs = []
    for sel_obj in sel:
        obj = sel_obj.Object
        if hasattr(obj, "Mesh") or obj.isDerivedFrom("Mesh::Feature"):
            objs.append(obj)
    return objs


def _get_first_selected_mesh_object():
    """Return the first selected mesh object or None."""
    objs = _get_selected_mesh_objects()
    return objs[0] if objs else None


def _document_has_meshes():
    """Return True if the active document has at least one mesh object."""
    if not FreeCAD.ActiveDocument:
        return False
    for obj in FreeCAD.ActiveDocument.Objects:
        if obj.isDerivedFrom("Mesh::Feature"):
            return True
    return False


def _ensure_mesh_data(obj):
    """Ensure an object has a valid mesh; convert shape if needed.

    Returns a FreeCAD :class:`Mesh.Mesh` object, **not** :class:`SlicerCore.mesh_io.MeshData`.
    For code paths that need ``MeshData``, use :func:`_ensure_meshdata` instead.
    """
    if obj.isDerivedFrom("Mesh::Feature"):
        return obj.Mesh
    if hasattr(obj, "Shape"):
        if Mesh is not None:
            mesh = Mesh.Mesh(obj.Shape.tessellate(0.1))
            return mesh
        else:
            FreeCAD.Console.PrintError(
                "[Slicer] Mesh module unavailable, cannot convert shape to mesh.\n"
            )
    return None


def _ensure_meshdata(obj):
    """Ensure an object has a valid mesh and return it as :class:`SlicerCore.mesh_io.MeshData`.

    This is the preferred helper when the result must be passed to
    ``SlicerCore`` functions (hollowing, cavity detection, support
    generation, etc.).
    """
    raw = _ensure_mesh_data(obj)
    if raw is None:
        return None
    try:
        from SlicerCore.mesh_io import MeshData
        return MeshData.from_freecad_mesh(raw, name=getattr(obj, "Label", "Mesh"))
    except Exception:
        FreeCAD.Console.PrintError(
            f"[Slicer] Failed to convert mesh to MeshData: {traceback.format_exc()}\n"
        )
        return None


def _safe_call(fn, *args, **kwargs):
    """Call fn, catching and printing exceptions."""
    try:
        return fn(*args, **kwargs)
    except Exception:
        FreeCAD.Console.PrintError(
            f"[Slicer] {traceback.format_exc()}\n"
        )
        return None


# ===================================================================
#  Command 1: Slicer_SliceCommand
# ===================================================================

class Slicer_SliceCommand:
    """Slice selected objects using FDM slicer."""

    _worker = None
    _panel = None

    def GetResources(self):
        return {
            "MenuText": "Slice (FDM)",
            "ToolTip": "Slice selected objects with FDM settings and generate G-code.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_Slice.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_Slice.svg"
            )) else "part-export",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        sel_objs = _get_selected_mesh_objects()
        if not sel_objs:
            FreeCAD.Console.PrintError(
                "[Slicer] Select one or more mesh objects to slice.\n"
            )
            return

        try:
            panel = SlicerTaskPanel()
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Cannot create task panel: {traceback.format_exc()}\n"
            )
            return

        Slicer_SliceCommand._panel = panel

        def _on_slice_requested(settings):
            meshes = []
            for obj in sel_objs:
                m = _ensure_mesh_data(obj)
                if m is not None:
                    meshes.append(m)

            if not meshes:
                FreeCAD.Console.PrintError("[Slicer] No valid mesh data.\n")
                return

            panel.show_progress(0)
            panel.set_slice_enabled(False)

            try:
                slicer = FDMSlicer(settings)
            except Exception:
                FreeCAD.Console.PrintError(
                    f"[Slicer] FDMSlicer init failed: {traceback.format_exc()}\n"
                )
                panel.set_slice_enabled(True)
                return

            for mesh_data in meshes:
                worker = _SliceWorker(slicer, mesh_data, settings)
                Slicer_SliceCommand._worker = worker

                def _on_finished(result):
                    panel.set_slice_enabled(True)
                    panel.show_progress(100)
                    if result:
                        FreeCAD.Console.PrintMessage(
                            f"[Slicer] Slice complete. "
                            f"Time: {result.get('estimated_time', 'N/A')}, "
                            f"Material: {result.get('material_used', 'N/A')}\n"
                        )
                        doc.SliceData = result

                def _on_error(msg):
                    panel.set_slice_enabled(True)
                    panel.show_progress(0)
                    FreeCAD.Console.PrintError(f"[Slicer] Slicing failed: {msg}\n")

                worker.finished.connect(_on_finished)
                worker.error.connect(_on_error)
                worker.start()

        panel.slice_requested.connect(_on_slice_requested)

        try:
            FreeCADGui.Control.showDialog(panel)
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Could not show dialog: {traceback.format_exc()}\n"
            )

    def IsActive(self):
        return FreeCAD.ActiveDocument is not None and _document_has_meshes()


# ===================================================================
#  Command 2: Slicer_ResinSliceCommand
# ===================================================================

class Slicer_ResinSliceCommand:
    """Slice selected objects for resin printing."""

    _worker = None
    _panel = None

    def GetResources(self):
        return {
            "MenuText": "Slice (Resin)",
            "ToolTip": "Slice selected objects with resin/SLA settings.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_ResinSlice.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_ResinSlice.svg"
            )) else "part-export",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        sel_objs = _get_selected_mesh_objects()
        if not sel_objs:
            FreeCAD.Console.PrintError(
                "[Slicer] Select one or more mesh objects to slice.\n"
            )
            return

        try:
            panel = SlicerTaskPanel()
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Cannot create task panel: {traceback.format_exc()}\n"
            )
            return

        Slicer_ResinSliceCommand._panel = panel

        def _on_slice_requested(settings):
            all_triangles = []
            for obj in sel_objs:
                m = _ensure_mesh_data(obj)
                if m is not None:
                    try:
                        verts = m.Points if hasattr(m, "Points") else []
                        facs = m.Facets if hasattr(m, "Facets") else []
                        for f in facs:
                            idx = f.Index if hasattr(f, "Index") else f
                            if len(idx) == 3:
                                tri = tuple(
                                    tuple(verts[i]) if hasattr(verts[i], "__iter__")
                                    else (verts[i].x, verts[i].y, verts[i].z)
                                    for i in idx
                                )
                                all_triangles.append(tri)
                    except Exception:
                        FreeCAD.Console.PrintError(
                            f"[Slicer] Cannot extract triangles: {traceback.format_exc()}\n"
                        )

            if not all_triangles:
                FreeCAD.Console.PrintError("[Slicer] No valid mesh data.\n")
                return

            panel.show_progress(0)
            panel.set_slice_enabled(False)

            try:
                slicer = ResinSlicer()
            except Exception:
                FreeCAD.Console.PrintError(
                    f"[Slicer] ResinSlicer init failed: {traceback.format_exc()}\n"
                )
                panel.set_slice_enabled(True)
                return

            worker = _ResinSliceWorker(slicer, all_triangles, settings)
            Slicer_ResinSliceCommand._worker = worker

            def _on_finished(result):
                panel.set_slice_enabled(True)
                panel.show_progress(100)
                if result:
                    layer_count = result.get("layer_count", "N/A")
                    est_time = result.get("estimated_time", "N/A")
                    FreeCAD.Console.PrintMessage(
                        f"[Slicer] Resin slice complete. "
                        f"Layers: {layer_count}, "
                        f"Time: {est_time}\n"
                    )
                    doc.SliceData = result

            def _on_error(msg):
                panel.set_slice_enabled(True)
                panel.show_progress(0)
                FreeCAD.Console.PrintError(
                    f"[Slicer] Resin slicing failed: {msg}\n"
                )

            worker.finished.connect(_on_finished)
            worker.error.connect(_on_error)
            worker.start()

        panel.slice_requested.connect(_on_slice_requested)

        try:
            FreeCADGui.Control.showDialog(panel)
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Could not show dialog: {traceback.format_exc()}\n"
            )

    def IsActive(self):
        return FreeCAD.ActiveDocument is not None and _document_has_meshes()


# ===================================================================
#  Command 3: Slicer_PreviewLayersCommand
# ===================================================================

class Slicer_PreviewLayersCommand:
    """Preview sliced layers in a 2D cross-section view."""

    _panel = None

    def GetResources(self):
        return {
            "MenuText": "Preview Layers",
            "ToolTip": "Scrub through sliced layers and view cross-sections.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_PreviewLayers.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_PreviewLayers.svg"
            )) else "view-refresh",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        slice_data = getattr(doc, "SliceData", None)
        if not slice_data:
            FreeCAD.Console.PrintError(
                "[Slicer] No slice data available. Run Slicer_Slice first.\n"
            )
            return

        try:
            panel = SlicerPreviewWidget()
            layers = slice_data.get("layers", [])
            if layers:
                panel.set_layer_data(layers)
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Cannot create preview widget: {traceback.format_exc()}\n"
            )
            return

        Slicer_PreviewLayersCommand._panel = panel

        try:
            FreeCADGui.Control.showDialog(panel)
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Could not show dialog: {traceback.format_exc()}\n"
            )

    def IsActive(self):
        if not FreeCAD.ActiveDocument:
            return False
        return getattr(FreeCAD.ActiveDocument, "SliceData", None) is not None


# ===================================================================
#  Command 4: Slicer_PreviewGcodeCommand
# ===================================================================

class Slicer_PreviewGcodeCommand:
    """Preview generated G-code toolpaths in 3D."""

    _panel = None

    def GetResources(self):
        return {
            "MenuText": "Preview G-code",
            "ToolTip": "Visualize generated G-code toolpaths with animation.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_PreviewGcode.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_PreviewGcode.svg"
            )) else "view-refresh",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        slice_data = getattr(doc, "SliceData", None)
        if not slice_data or "gcode" not in slice_data:
            FreeCAD.Console.PrintError(
                "[Slicer] No G-code available. Run Slicer_Slice first.\n"
            )
            return

        try:
            panel = GcodePreviewWidget()
            panel.set_gcode(slice_data["gcode"])
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Cannot create G-code preview: {traceback.format_exc()}\n"
            )
            return

        Slicer_PreviewGcodeCommand._panel = panel

        try:
            FreeCADGui.Control.showDialog(panel)
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Could not show dialog: {traceback.format_exc()}\n"
            )

    def IsActive(self):
        if not FreeCAD.ActiveDocument:
            return False
        sd = getattr(FreeCAD.ActiveDocument, "SliceData", None)
        return sd is not None and "gcode" in sd


# ===================================================================
#  Command 5: Slicer_GenerateSupportsCommand
# ===================================================================

class Slicer_GenerateSupportsCommand:
    """Auto-generate supports for selected mesh objects."""

    _worker = None
    _panel = None

    def GetResources(self):
        return {
            "MenuText": "Generate Supports",
            "ToolTip": "Automatically generate support structures for selected meshes.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_GenerateSupports.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_GenerateSupports.svg"
            )) else "edit-select-all",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        sel_objs = _get_selected_mesh_objects()
        if not sel_objs:
            FreeCAD.Console.PrintError(
                "[Slicer] Select mesh objects for support generation.\n"
            )
            return

        settings = {
            "angle_threshold": 45.0,
            "density": 1.0,
            "pillar_radius": 0.4,
            "base_radius": 1.5,
            "tip_radius": 0.2,
            "z_gap": 0.1,
        }

        try:
            generator = StandaloneSupportGenerator(settings)
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] SupportGenerator init failed: {traceback.format_exc()}\n"
            )
            return

        meshes = []
        for obj in sel_objs:
            m = _ensure_meshdata(obj)
            if m is not None:
                meshes.append((obj, m))

        if not meshes:
            FreeCAD.Console.PrintError("[Slicer] No valid mesh data.\n")
            return

        FreeCAD.Console.PrintMessage(
            f"[Slicer] Generating supports for {len(meshes)} object(s)...\n"
        )

        for idx, (obj, m) in enumerate(meshes):
            FreeCAD.Console.PrintMessage(
                f"[Slicer] Generating supports for {obj.Name}...\n"
            )
            worker = _SupportWorker(generator, m, settings)
            Slicer_GenerateSupportsCommand._worker = worker
        Slicer_GenerateSupportsCommand._worker = worker

        def _on_progress(pct, msg):
            FreeCAD.Console.PrintMessage(f"[Slicer] Support gen: {pct}% - {msg}\n")

        def _on_finished(result):
            if result is None:
                FreeCAD.Console.PrintError("[Slicer] Support generation returned None.\n")
                return
            FreeCAD.Console.PrintMessage("[Slicer] Support generation complete.\n")
            try:
                panel = SupportPaintWidget()
                Slicer_GenerateSupportsCommand._panel = panel
                FreeCADGui.Control.showDialog(panel)
            except Exception:
                FreeCAD.Console.PrintError(
                    f"[Slicer] Cannot open support editor: {traceback.format_exc()}\n"
                )

        def _on_error(msg):
            FreeCAD.Console.PrintError(f"[Slicer] Support generation failed: {msg}\n")

        worker.progress.connect(_on_progress)
        worker.finished.connect(_on_finished)
        worker.error.connect(_on_error)
        worker.start()

    def IsActive(self):
        return FreeCAD.ActiveDocument is not None and _document_has_meshes()


# ===================================================================
#  Command 6: Slicer_PaintSupportsCommand
# ===================================================================

class Slicer_PaintSupportsCommand:
    """Manually paint / place support points on a mesh."""

    _panel = None

    def GetResources(self):
        return {
            "MenuText": "Paint Supports",
            "ToolTip": "Manually add or remove support points on selected mesh.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_PaintSupports.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_PaintSupports.svg"
            )) else "edit-select-all",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        obj = _get_first_selected_mesh_object()
        if not obj:
            FreeCAD.Console.PrintError(
                "[Slicer] Select a mesh object to paint supports.\n"
            )
            return

        mesh = _ensure_meshdata(obj)
        if mesh is None:
            FreeCAD.Console.PrintError("[Slicer] Object has no mesh data.\n")
            return

        try:
            panel = SupportPaintWidget()
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Cannot open support painter: {traceback.format_exc()}\n"
            )
            return

        Slicer_PaintSupportsCommand._panel = panel

        try:
            FreeCADGui.Control.showDialog(panel)
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Could not show dialog: {traceback.format_exc()}\n"
            )

    def IsActive(self):
        return FreeCAD.ActiveDocument is not None and _document_has_meshes()


# ===================================================================
#  Command 7: Slicer_HollowModelCommand
# ===================================================================

class Slicer_HollowModelCommand:
    """Hollow out a solid mesh for resin printing."""

    _worker = None

    def GetResources(self):
        return {
            "MenuText": "Hollow Model",
            "ToolTip": "Hollow the selected mesh with configurable wall thickness.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_HollowModel.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_HollowModel.svg"
            )) else "part-export",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        obj = _get_first_selected_mesh_object()
        if not obj:
            FreeCAD.Console.PrintError(
                "[Slicer] Select a mesh object to hollow.\n"
            )
            return

        mesh = _ensure_meshdata(obj)
        if mesh is None:
            FreeCAD.Console.PrintError("[Slicer] Object has no mesh data.\n")
            return

        settings = {
            "wall_thickness": 2.0,
            "infill_pattern": "none",
            "infill_density": 0.0,
            "accuracy": 0.1,
        }

        try:
            hollower = MeshHollower()
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] MeshHollower init failed: {traceback.format_exc()}\n"
            )
            return

        FreeCAD.Console.PrintMessage(
            f"[Slicer] Hollowing '{obj.Name}' "
            f"(wall={settings['wall_thickness']}mm)...\n"
        )

        worker = _HollowWorker(hollower, mesh, settings)
        Slicer_HollowModelCommand._worker = worker

        def _on_progress(pct, msg):
            FreeCAD.Console.PrintMessage(f"[Slicer] Hollow: {pct}% - {msg}\n")

        def _on_finished(result):
            if result is None:
                FreeCAD.Console.PrintError("[Slicer] Hollowing returned None.\n")
                return
            FreeCAD.Console.PrintMessage(
                f"[Slicer] Hollowing complete for '{obj.Name}'.\n"
            )
            apply_mesh_to_object(obj, result)

        def _on_error(msg):
            FreeCAD.Console.PrintError(f"[Slicer] Hollowing failed: {msg}\n")

        worker.progress.connect(_on_progress)
        worker.finished.connect(_on_finished)
        worker.error.connect(_on_error)
        worker.start()

    def IsActive(self):
        return FreeCAD.ActiveDocument is not None and _document_has_meshes()


# ===================================================================
#  Command 8: Slicer_DetectCavitiesCommand
# ===================================================================

class Slicer_DetectCavitiesCommand:
    """Detect internal cavities and suggest drain holes for resin printing."""

    _worker = None

    def GetResources(self):
        return {
            "MenuText": "Detect Cavities",
            "ToolTip": (
                "Detect internal cavities in mesh and suggest drain hole positions."
            ),
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_DetectCavities.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_DetectCavities.svg"
            )) else "edit-select-all",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        obj = _get_first_selected_mesh_object()
        if not obj:
            FreeCAD.Console.PrintError(
                "[Slicer] Select a mesh object for cavity detection.\n"
            )
            return

        mesh = _ensure_meshdata(obj)
        if mesh is None:
            FreeCAD.Console.PrintError("[Slicer] Object has no mesh data.\n")
            return

        settings = {
            "min_cavity_volume": 50.0,
            "detection_resolution": 0.5,
            "drain_hole_radius": 1.0,
        }

        try:
            detector = CavityDetector()
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] CavityDetector init failed: {traceback.format_exc()}\n"
            )
            return

        FreeCAD.Console.PrintMessage(
            f"[Slicer] Detecting cavities in '{obj.Name}'...\n"
        )

        worker = _CavityWorker(detector, mesh, settings)
        Slicer_DetectCavitiesCommand._worker = worker

        def _on_progress(pct, msg):
            FreeCAD.Console.PrintMessage(f"[Slicer] Cavity detect: {pct}% - {msg}\n")

        def _on_finished(result):
            if result is None:
                FreeCAD.Console.PrintError("[Slicer] Cavity detection returned None.\n")
                return

            cavities = result.get("cavities", [])
            drain_holes = result.get("suggested_drain_holes", [])

            FreeCAD.Console.PrintMessage(
                f"[Slicer] Found {len(cavities)} cavity/cavities, "
                f"{len(drain_holes)} suggested drain hole(s).\n"
            )

            try:
                from SlicerUI.cavity_result_dialog import CavityResultDialog
                dlg = CavityResultDialog(obj, result)
                dlg.exec_()
            except ImportError:
                FreeCAD.Console.PrintMessage(
                    "[Slicer] Cavities detected. Suggested drain hole positions:\n"
                )
                for i, hole in enumerate(drain_holes):
                    pos = hole.get("position", (0, 0, 0))
                    FreeCAD.Console.PrintMessage(
                        f"  Hole {i+1}: ({pos[0]:.2f}, {pos[1]:.2f}, {pos[2]:.2f})\n"
                    )
            except Exception:
                FreeCAD.Console.PrintError(
                    f"[Slicer] Error showing cavity results: {traceback.format_exc()}\n"
                )

        def _on_error(msg):
            FreeCAD.Console.PrintError(
                f"[Slicer] Cavity detection failed: {msg}\n"
            )

        worker.progress.connect(_on_progress)
        worker.finished.connect(_on_finished)
        worker.error.connect(_on_error)
        worker.start()

    def IsActive(self):
        return FreeCAD.ActiveDocument is not None and _document_has_meshes()


# ===================================================================
#  Command 9: Slicer_ImportModelCommand
# ===================================================================

class Slicer_ImportModelCommand:
    """Import a mesh model file (STL, OBJ, 3MF, PLY, OFF)."""

    def GetResources(self):
        return {
            "MenuText": "Import Model",
            "ToolTip": "Import a mesh file (STL, OBJ, 3MF, PLY, OFF) into the document.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_ImportModel.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_ImportModel.svg"
            )) else "part-import",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        filter_str = (
            "Mesh files (*.stl *.STL *.obj *.OBJ *.3mf *.3MF "
            "*.ply *.PLY *.off *.OFF);;"
            "STL files (*.stl *.STL);;"
            "OBJ files (*.obj *.OBJ);;"
            "3MF files (*.3mf *.3MF);;"
            "PLY files (*.ply *.PLY);;"
            "OFF files (*.off *.OFF);;"
            "All files (*)"
        )

        try:
            path, _ = QtWidgets.QFileDialog.getOpenFileName(
                None, "Import Mesh Model", "", filter_str
            )
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] File dialog error: {traceback.format_exc()}\n"
            )
            return

        if not path:
            return

        FreeCAD.Console.PrintMessage(f"[Slicer] Importing '{path}'...\n")

        result = _safe_call(import_mesh_file, path, doc)
        if result is not None:
            FreeCAD.Console.PrintMessage(
                f"[Slicer] Imported '{os.path.basename(path)}' successfully.\n"
            )
            try:
                FreeCADGui.Selection.clearSelection()
                FreeCADGui.Selection.addSelection(doc.Name, result.Name)
            except Exception:
                pass
        else:
            FreeCAD.Console.PrintError(
                f"[Slicer] Failed to import '{path}'.\n"
            )

    def IsActive(self):
        return FreeCAD.ActiveDocument is not None


# ===================================================================
#  Command 10: Slicer_ExportSlicedCommand
# ===================================================================

class Slicer_ExportSlicedCommand:
    """Export slice results to G-code or resin format file."""

    def GetResources(self):
        return {
            "MenuText": "Export Sliced",
            "ToolTip": "Export the generated G-code or resin slice file.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_ExportSliced.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_ExportSliced.svg"
            )) else "part-export",
        }

    def Activated(self):
        doc = FreeCAD.ActiveDocument
        if not doc:
            FreeCAD.Console.PrintError("[Slicer] No active document.\n")
            return

        slice_data = getattr(doc, "SliceData", None)
        if not slice_data:
            FreeCAD.Console.PrintError(
                "[Slicer] No slice data. Run a slice command first.\n"
            )
            return

        is_resin = slice_data.get("mode", "fdm") == "resin"

        if is_resin:
            filter_str = "Resin files (*.cbddlp *.photon *.sl1 *.pwmx);;All files (*)"
            default_ext = ".cbddlp"
        else:
            filter_str = "G-code files (*.gcode *.gco *.g);;All files (*)"
            default_ext = ".gcode"

        try:
            path, _ = QtWidgets.QFileDialog.getSaveFileName(
                None, "Export Sliced Output", "", filter_str
            )
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] File dialog error: {traceback.format_exc()}\n"
            )
            return

        if not path:
            return

        if not os.path.splitext(path)[1]:
            path += default_ext

        try:
            if is_resin:
                from SlicerCore.resin_formats import write_resin_file
                write_resin_file(slice_data, path)
            else:
                gcode_content = slice_data.get("gcode", "")
                if not gcode_content:
                    FreeCAD.Console.PrintError(
                        "[Slicer] No G-code content in slice data.\n"
                    )
                    return
                with open(path, "w", encoding="utf-8") as f:
                    f.write(gcode_content)

            FreeCAD.Console.PrintMessage(
                f"[Slicer] Exported to '{path}' successfully.\n"
            )
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Export failed: {traceback.format_exc()}\n"
            )

    def IsActive(self):
        if not FreeCAD.ActiveDocument:
            return False
        return getattr(FreeCAD.ActiveDocument, "SliceData", None) is not None


# ===================================================================
#  Command 11: Slicer_PreferencesCommand
# ===================================================================

class Slicer_PreferencesCommand:
    """Open the SlicerWorkbench preferences dialog."""

    def GetResources(self):
        return {
            "MenuText": "Slicer Preferences",
            "ToolTip": "Configure slicer workbench preferences and external tool paths.",
            "Pixmap": os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_Preferences.svg"
            ) if os.path.exists(os.path.join(
                os.path.dirname(__file__), "Icons", "Slicer_Preferences.svg"
            )) else "preferences-system",
        }

    def Activated(self):
        try:
            dlg = SlicerPreferencesDialog()
            dlg.exec_()
        except Exception:
            FreeCAD.Console.PrintError(
                f"[Slicer] Cannot open preferences: {traceback.format_exc()}\n"
            )

    def IsActive(self):
        return True


# ===================================================================
#  Register all commands
# ===================================================================

FreeCADGui.addCommand("Slicer_Slice", Slicer_SliceCommand())
FreeCADGui.addCommand("Slicer_ResinSlice", Slicer_ResinSliceCommand())
FreeCADGui.addCommand("Slicer_PreviewLayers", Slicer_PreviewLayersCommand())
FreeCADGui.addCommand("Slicer_PreviewGcode", Slicer_PreviewGcodeCommand())
FreeCADGui.addCommand("Slicer_GenerateSupports", Slicer_GenerateSupportsCommand())
FreeCADGui.addCommand("Slicer_PaintSupports", Slicer_PaintSupportsCommand())
FreeCADGui.addCommand("Slicer_HollowModel", Slicer_HollowModelCommand())
FreeCADGui.addCommand("Slicer_DetectCavities", Slicer_DetectCavitiesCommand())
FreeCADGui.addCommand("Slicer_ImportModel", Slicer_ImportModelCommand())
FreeCADGui.addCommand("Slicer_ExportSliced", Slicer_ExportSlicedCommand())
FreeCADGui.addCommand("Slicer_Preferences", Slicer_PreferencesCommand())

FreeCAD.Console.PrintMessage("[Slicer] All commands registered.\n")
