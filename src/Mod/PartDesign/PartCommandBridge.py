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


def _list_commands():
    try:
        return set(Gui.listCommands())
    except Exception:
        return set()


def register_alias_commands():
    known_commands = _list_commands()

    for alias_name, (source_name, menu_text, tooltip, pixmap) in ALIAS_COMMANDS.items():
        if alias_name in known_commands:
            continue
        has_custom_handler = alias_name in CUSTOM_ACTIVATORS
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
                    CUSTOM_ACTIVATORS[alias_name],
                ),
            )
        else:
            Gui.addCommand(alias_name, _RelayCommand(source_name, menu_text, tooltip, pixmap))


def map_legacy_command(command_name):
    """Map old Part command name to PartDesign bridge command name if available."""
    for alias_name, (source_name, _menu, _tip, _pixmap) in ALIAS_COMMANDS.items():
        if source_name == command_name:
            return alias_name
    return command_name


def run_mapped_command(command_name):
    mapped = map_legacy_command(command_name)
    Gui.runCommand(mapped, 0)


def initialize_bridge():
    register_alias_commands()
    App.Console.PrintLog("PartDesign bridge commands initialized.\n")
