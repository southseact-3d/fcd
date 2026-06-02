# SPDX-License-Identifier: LGPL-2.1-or-later

"""Part-to-PartDesign command bridge.

This module exposes Part tools under PartDesign-prefixed command names so users can
work from a single workbench while preserving existing functionality.
"""

import FreeCAD as App  # type: ignore
import FreeCADGui as Gui  # type: ignore


class _RelayCommand:
    def __init__(self, source_command, menu_text, tooltip, pixmap):
        self._source_command = source_command
        self._menu_text = menu_text
        self._tooltip = tooltip
        self._pixmap = pixmap

    def GetResources(self):
        return {
            "Pixmap": self._pixmap,
            "MenuText": self._menu_text,
            "ToolTip": self._tooltip,
        }

    def IsActive(self):
        # Delegate availability to the source command where possible.
        return True

    def Activated(self):
        Gui.runCommand(self._source_command, 0)


class _HandlerCommand(_RelayCommand):
    def __init__(self, source_command, menu_text, tooltip, pixmap, handler):
        super().__init__(source_command, menu_text, tooltip, pixmap)
        self._handler = handler

    def Activated(self):
        self._handler()


# Alias table: PartDesign command -> Part command
ALIAS_COMMANDS = {
    "PartDesign_PartBox": ("Part_Box", "Box", "Create a Part box", "Part_Box"),
    "PartDesign_PartCylinder": (
        "Part_Cylinder",
        "Cylinder",
        "Create a Part cylinder",
        "Part_Cylinder",
    ),
    "PartDesign_PartSphere": ("Part_Sphere", "Sphere", "Create a Part sphere", "Part_Sphere"),
    "PartDesign_PartCone": ("Part_Cone", "Cone", "Create a Part cone", "Part_Cone"),
    "PartDesign_PartTorus": ("Part_Torus", "Torus", "Create a Part torus", "Part_Torus"),
    "PartDesign_PartTube": ("Part_Tube", "Tube", "Create a Part tube", "Part_Tube"),
    "PartDesign_PartPrimitives": (
        "Part_Primitives",
        "Primitive",
        "Creates solid geometric primitives parametrically",
        "Part_Primitives",
    ),
    "PartDesign_PartBuilder": (
        "Part_Builder",
        "Shape Builder",
        "Open shape builder",
        "Part_Builder",
    ),
    "PartDesign_PartBoolean": (
        "Part_Boolean",
        "Boolean",
        "Boolean operation dialog",
        "Part_Boolean",
    ),
    "PartDesign_PartCut": ("Part_Cut", "Cut", "Cuts selected shapes", "Part_Cut"),
    "PartDesign_PartFuse": ("Part_Fuse", "Union", "Unites selected shapes", "Part_Fuse"),
    "PartDesign_PartCommon": (
        "Part_Common",
        "Intersection",
        "Intersect selected shapes",
        "Part_Common",
    ),
    "PartDesign_PartSection": (
        "Part_Section",
        "Section",
        "Section operation between two shapes",
        "Part_Section",
    ),
    "PartDesign_PartJoinConnect": (
        "Part_JoinConnect",
        "Connect",
        "Connect shapes preserving voids",
        "Part_JoinConnect",
    ),
    "PartDesign_PartJoinEmbed": (
        "Part_JoinEmbed",
        "Embed",
        "Embed one shape into another",
        "Part_JoinEmbed",
    ),
    "PartDesign_PartJoinCutout": (
        "Part_JoinCutout",
        "Cutout",
        "Subtract one shape from another preserving intersections",
        "Part_JoinCutout",
    ),
    "PartDesign_PartBooleanFragments": (
        "Part_BooleanFragments",
        "Boolean Fragments",
        "Make Boolean fragments",
        "Part_BooleanFragments",
    ),
    "PartDesign_PartSliceApart": (
        "Part_SliceApart",
        "Slice Apart",
        "Slice shapes apart",
        "Part_SliceApart",
    ),
    "PartDesign_PartSlice": ("Part_Slice", "Slice", "Slice shapes", "Part_Slice"),
    "PartDesign_PartXor": (
        "Part_XOR",
        "XOR",
        "Exclusive OR between selected shapes",
        "Part_XOR",
    ),
    "PartDesign_PartCompound": (
        "Part_Compound",
        "Compound",
        "Create compound",
        "Part_Compound",
    ),
    "PartDesign_PartExplodeCompound": (
        "Part_ExplodeCompound",
        "Explode Compound",
        "Explode a compound into children",
        "Part_ExplodeCompound",
    ),
    "PartDesign_PartCompoundFilter": (
        "Part_CompoundFilter",
        "Compound Filter",
        "Filter children of a compound",
        "Part_CompoundFilter",
    ),
    "PartDesign_PartToleranceSet": (
        "Part_ToleranceSet",
        "Set Tolerance",
        "Set shape tolerance",
        "Part_ToleranceSet",
    ),
    "PartDesign_PartSimpleCopy": (
        "Part_SimpleCopy",
        "Simple Copy",
        "Create simple copy",
        "Part_SimpleCopy",
    ),
    "PartDesign_PartTransformedCopy": (
        "Part_TransformedCopy",
        "Transformed Copy",
        "Create transformed copy",
        "Part_TransformedCopy",
    ),
    "PartDesign_PartElementCopy": (
        "Part_ElementCopy",
        "Element Copy",
        "Create copy of sub-elements",
        "Part_ElementCopy",
    ),
    "PartDesign_PartRefineShape": (
        "Part_RefineShape",
        "Refine Shape",
        "Refine shape",
        "Part_RefineShape",
    ),
    "PartDesign_PartMakeSolid": (
        "Part_MakeSolid",
        "Convert to Solid",
        "Convert shell to solid",
        "Part_MakeSolid",
    ),
    "PartDesign_PartReverseShape": (
        "Part_ReverseShape",
        "Reverse Shape",
        "Reverse shape orientation",
        "Part_ReverseShape",
    ),
    "PartDesign_PartMakeFace": (
        "Part_MakeFace",
        "Face from Wires",
        "Create a face from wires",
        "Part_MakeFace",
    ),
    "PartDesign_PartLoft": ("Part_Loft", "Loft", "Create loft", "Part_Loft"),
    "PartDesign_PartSweep": ("Part_Sweep", "Sweep", "Create sweep", "Part_Sweep"),
    "PartDesign_PartThickness": (
        "Part_Thickness",
        "Thickness",
        "Make thickness",
        "Part_Thickness",
    ),
    "PartDesign_PartProjectionOnSurface": (
        "Part_ProjectionOnSurface",
        "Projection on Surface",
        "Project edges and wires on surface",
        "Part_ProjectionOnSurface",
    ),
    "PartDesign_PartSectionCut": (
        "Part_SectionCut",
        "Section Cut",
        "Interactive section cut",
        "Part_SectionCut",
    ),
    "PartDesign_PartEditAttachment": (
        "Part_EditAttachment",
        "Attachment",
        "Edit attachment",
        "Part_EditAttachment",
    ),
    "PartDesign_PartScale": ("Part_Scale", "Scale", "Scale shape", "Part_Scale"),
    "PartDesign_PartOffset": ("Part_Offset", "3D Offset", "Create 3D offset", "Part_Offset"),
    "PartDesign_PartOffset2D": (
        "Part_Offset2D",
        "2D Offset",
        "Create 2D offset",
        "Part_Offset2D",
    ),
    "PartDesign_PartShapeFromMesh": (
        "Part_ShapeFromMesh",
        "Shape from Mesh",
        "Convert mesh to shape",
        "Part_ShapeFromMesh",
    ),
    "PartDesign_PartDefeaturing": (
        "Part_Defeaturing",
        "Defeaturing",
        "Remove selected faces",
        "Part_Defeaturing",
    ),
    "PartDesign_PartCrossSections": (
        "Part_CrossSections",
        "Cross-sections",
        "Create cross-sections",
        "Part_CrossSections",
    ),
    "PartDesign_PartRuledSurface": (
        "Part_RuledSurface",
        "Ruled Surface",
        "Create ruled surface",
        "Part_RuledSurface",
    ),
    "PartDesign_PartCheckGeometry": (
        "Part_CheckGeometry",
        "Check Geometry",
        "Run geometry checks",
        "Part_CheckGeometry",
    ),
    "PartDesign_PartBox2": (
        "Part_Box2",
        "Box Fix 1",
        "Creates a solid box (fix 1)",
        "Part_Box_Parametric",
    ),
    "PartDesign_PartBox3": (
        "Part_Box3",
        "Box Fix 2",
        "Creates a solid box (fix 2)",
        "Part_Box_Parametric",
    ),
    "PartDesign_PartBoxSelection": (
        "Part_BoxSelection",
        "Box Selection",
        "Select objects by box",
        "Part_BoxSelection",
    ),
    "PartDesign_PartChamfer": (
        "Part_Chamfer",
        "Chamfer",
        "Create chamfer",
        "Part_Chamfer",
    ),
    "PartDesign_PartColorPerFace": (
        "Part_ColorPerFace",
        "Color Per Face",
        "Set per-face colors",
        "Part_ColorPerFace",
    ),
    "PartDesign_PartCompCompoundTools": (
        "Part_CompCompoundTools",
        "Compound Tools",
        "Compound tools dialog",
        "Part_CompCompoundTools",
    ),
    "PartDesign_PartCompJoinFeatures": (
        "Part_CompJoinFeatures",
        "Join Features",
        "Join features dialog",
        "Part_CompJoinFeatures",
    ),
    "PartDesign_PartCompOffset": (
        "Part_CompOffset",
        "Compound Offset",
        "Create compound offset",
        "Part_CompOffset",
    ),
    "PartDesign_PartCompSplitFeatures": (
        "Part_CompSplitFeatures",
        "Split Features",
        "Split features dialog",
        "Part_CompSplitFeatures",
    ),
    "PartDesign_PartCoordinateSystem": (
        "Part_CoordinateSystem",
        "Coordinate System",
        "Create a coordinate system",
        "Part_CoordinateSystem",
    ),
    "PartDesign_PartDatumLine": (
        "Part_DatumLine",
        "Datum Line",
        "Create a datum line",
        "Part_DatumLine",
    ),
    "PartDesign_PartDatumPlane": (
        "Part_DatumPlane",
        "Datum Plane",
        "Create a datum plane",
        "Part_DatumPlane",
    ),
    "PartDesign_PartDatumPoint": (
        "Part_DatumPoint",
        "Datum Point",
        "Create a datum point",
        "Part_DatumPoint",
    ),
    "PartDesign_PartDatums": (
        "Part_Datums",
        "Datums",
        "Create datum objects",
        "Part_Datums",
    ),
    "PartDesign_PartEdgeSelection": (
        "Part_EdgeSelection",
        "Edge Selection",
        "Select edges",
        "Part_EdgeSelection",
    ),
    "PartDesign_PartExport": (
        "Part_Export",
        "Export",
        "Exports the selected object",
        "Part_Export",
    ),
    "PartDesign_PartExtrude": (
        "Part_Extrude",
        "Extrude",
        "Create extrusion",
        "Part_Extrude",
    ),
    "PartDesign_PartFaceSelection": (
        "Part_FaceSelection",
        "Face Selection",
        "Select faces",
        "Part_FaceSelection",
    ),
    "PartDesign_PartFillet": (
        "Part_Fillet",
        "Fillet",
        "Create fillet",
        "Part_Fillet",
    ),
    "PartDesign_PartImport": (
        "Part_Import",
        "Import CAD File",
        "Imports a CAD file",
        "Part_Import",
    ),
    "PartDesign_PartImportCurveNet": (
        "Part_ImportCurveNet",
        "Import Curve Net",
        "Import curve network file",
        "Part_ImportCurveNet",
    ),
    "PartDesign_PartMirror": (
        "Part_Mirror",
        "Mirror",
        "Mirror shapes",
        "Part_Mirror",
    ),
    "PartDesign_PartNewDoc": (
        "Part_NewDoc",
        "New Document",
        "Create an empty Part document",
        "New",
    ),
    "PartDesign_PartPickCurveNet": (
        "Part_PickCurveNet",
        "Pick Curve Network",
        "Pick a curve network",
        "Test1",
    ),
    "PartDesign_PartPointsFromMesh": (
        "Part_PointsFromMesh",
        "Points from Mesh",
        "Convert a mesh to points",
        "Part_PointsFromMesh",
    ),
    "PartDesign_PartRemoveSelectionGate": (
        "Part_RemoveSelectionGate",
        "Remove Selection Gate",
        "Remove selection gate",
        "Part_RemoveSelectionGate",
    ),
    "PartDesign_PartRevolve": (
        "Part_Revolve",
        "Revolve",
        "Create a revolution",
        "Part_Revolve",
    ),
    "PartDesign_PartSelectFilter": (
        "Part_SelectFilter",
        "Select Filter",
        "Filter current selection",
        "Part_SelectFilter",
    ),
    "PartDesign_PartShapeInfo": (
        "Part_ShapeInfo",
        "Shape Info",
        "Show shape information",
        "Part_ShapeInfo",
    ),
    "PartDesign_PartSimpleCylinder": (
        "Part_SimpleCylinder",
        "Simple Cylinder",
        "Creates a simple cylinder",
        "Part_SimpleCylinder",
    ),
    "PartDesign_PartVertexSelection": (
        "Part_VertexSelection",
        "Vertex Selection",
        "Select vertices",
        "Part_VertexSelection",
    ),
}


def _activate_join_connect():
    from PartDesign.BOPTools import JoinFeatures

    JoinFeatures.CommandConnect().Activated()


def _activate_join_embed():
    from PartDesign.BOPTools import JoinFeatures

    JoinFeatures.CommandEmbed().Activated()


def _activate_join_cutout():
    from PartDesign.BOPTools import JoinFeatures

    JoinFeatures.CommandCutout().Activated()


def _activate_boolean_fragments():
    from PartDesign.BOPTools import SplitFeatures

    SplitFeatures.CommandBooleanFragments().Activated()


def _activate_slice_apart():
    from PartDesign.BOPTools import SplitFeatures

    SplitFeatures.CommandSliceApart().Activated()


def _activate_slice():
    from PartDesign.BOPTools import SplitFeatures

    SplitFeatures.CommandSlice().Activated()


def _activate_xor():
    from PartDesign.BOPTools import SplitFeatures

    SplitFeatures.CommandXOR().Activated()


def _activate_compound_filter():
    from PartDesign.CompoundTools import _CommandCompoundFilter

    _CommandCompoundFilter._CommandCompoundFilter().Activated()


def _activate_explode_compound():
    from PartDesign.CompoundTools import _CommandExplodeCompound

    _CommandExplodeCompound._CommandExplodeCompound().Activated()


def _activate_tolerance_set():
    from PartDesign.BOPTools import ToleranceFeatures

    ToleranceFeatures.CommandToleranceSet().Activated()


CUSTOM_ACTIVATORS = {
    "PartDesign_PartJoinConnect": _activate_join_connect,
    "PartDesign_PartJoinEmbed": _activate_join_embed,
    "PartDesign_PartJoinCutout": _activate_join_cutout,
    "PartDesign_PartBooleanFragments": _activate_boolean_fragments,
    "PartDesign_PartSliceApart": _activate_slice_apart,
    "PartDesign_PartSlice": _activate_slice,
    "PartDesign_PartXor": _activate_xor,
    "PartDesign_PartCompoundFilter": _activate_compound_filter,
    "PartDesign_PartExplodeCompound": _activate_explode_compound,
    "PartDesign_PartToleranceSet": _activate_tolerance_set,
}

SOURCE_TO_ALIAS = {
    source_name: alias_name
    for alias_name, (source_name, _menu_text, _tooltip, _pixmap) in ALIAS_COMMANDS.items()
}

_BRIDGE_INITIALIZED = False


def _list_commands():
    try:
        return set(Gui.listCommands())
    except Exception:
        return set()


def register_alias_commands():
    known_commands = _list_commands()
    custom_activators = CUSTOM_ACTIVATORS

    for alias_name, (source_name, menu_text, tooltip, pixmap) in ALIAS_COMMANDS.items():
        if alias_name in known_commands:
            continue
        handler = custom_activators.get(alias_name)
        has_custom_handler = handler is not None
        if not has_custom_handler and source_name not in known_commands:
            continue

        if has_custom_handler:
            Gui.addCommand(
                alias_name,
                _HandlerCommand(
                    source_name,
                    menu_text,
                    tooltip,
                    pixmap,
                    handler,
                ),
            )
        else:
            Gui.addCommand(alias_name, _RelayCommand(source_name, menu_text, tooltip, pixmap))
        known_commands.add(alias_name)


def map_legacy_command(command_name):
    """Map old Part command name to PartDesign bridge command name if available."""
    return SOURCE_TO_ALIAS.get(command_name, command_name)


def run_mapped_command(command_name):
    mapped = map_legacy_command(command_name)
    Gui.runCommand(mapped, 0)


def initialize_bridge():
    global _BRIDGE_INITIALIZED
    if _BRIDGE_INITIALIZED:
        return

    register_alias_commands()
    _BRIDGE_INITIALIZED = True
    App.Console.PrintLog("PartDesign bridge commands initialized.\n")
