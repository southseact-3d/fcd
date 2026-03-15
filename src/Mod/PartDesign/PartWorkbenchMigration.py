# SPDX-License-Identifier: LGPL-2.1-or-later

"""Utilities to migrate legacy Part workbench command usage to PartDesign bridge commands."""

from __future__ import annotations

import re
from pathlib import Path


COMMAND_REPLACEMENTS = {
    "Part_Box": "PartDesign_PartBox",
    "Part_Cylinder": "PartDesign_PartCylinder",
    "Part_Sphere": "PartDesign_PartSphere",
    "Part_Cone": "PartDesign_PartCone",
    "Part_Torus": "PartDesign_PartTorus",
    "Part_Tube": "PartDesign_PartTube",
    "Part_Primitives": "PartDesign_PartPrimitives",
    "Part_Builder": "PartDesign_PartBuilder",
    "Part_Boolean": "PartDesign_PartBoolean",
    "Part_Cut": "PartDesign_PartCut",
    "Part_Fuse": "PartDesign_PartFuse",
    "Part_Common": "PartDesign_PartCommon",
    "Part_Section": "PartDesign_PartSection",
    "Part_JoinConnect": "PartDesign_PartJoinConnect",
    "Part_JoinEmbed": "PartDesign_PartJoinEmbed",
    "Part_JoinCutout": "PartDesign_PartJoinCutout",
    "Part_BooleanFragments": "PartDesign_PartBooleanFragments",
    "Part_SliceApart": "PartDesign_PartSliceApart",
    "Part_Slice": "PartDesign_PartSlice",
    "Part_XOR": "PartDesign_PartXor",
    "Part_Compound": "PartDesign_PartCompound",
    "Part_ExplodeCompound": "PartDesign_PartExplodeCompound",
    "Part_CompoundFilter": "PartDesign_PartCompoundFilter",
    "Part_ToleranceSet": "PartDesign_PartToleranceSet",
    "Part_SimpleCopy": "PartDesign_PartSimpleCopy",
    "Part_TransformedCopy": "PartDesign_PartTransformedCopy",
    "Part_ElementCopy": "PartDesign_PartElementCopy",
    "Part_RefineShape": "PartDesign_PartRefineShape",
    "Part_MakeSolid": "PartDesign_PartMakeSolid",
    "Part_ReverseShape": "PartDesign_PartReverseShape",
    "Part_MakeFace": "PartDesign_PartMakeFace",
    "Part_Loft": "PartDesign_PartLoft",
    "Part_Sweep": "PartDesign_PartSweep",
    "Part_Thickness": "PartDesign_PartThickness",
    "Part_ProjectionOnSurface": "PartDesign_PartProjectionOnSurface",
    "Part_SectionCut": "PartDesign_PartSectionCut",
    "Part_EditAttachment": "PartDesign_PartEditAttachment",
    "Part_Scale": "PartDesign_PartScale",
    "Part_Offset": "PartDesign_PartOffset",
    "Part_Offset2D": "PartDesign_PartOffset2D",
    "Part_ShapeFromMesh": "PartDesign_PartShapeFromMesh",
    "Part_Defeaturing": "PartDesign_PartDefeaturing",
    "Part_CrossSections": "PartDesign_PartCrossSections",
    "Part_RuledSurface": "PartDesign_PartRuledSurface",
    "Part_CheckGeometry": "PartDesign_PartCheckGeometry",
}


def remap_command_name(command_name: str) -> str:
    return COMMAND_REPLACEMENTS.get(command_name, command_name)


def rewrite_macro_content(content: str) -> str:
    rewritten = content
    for old_name, new_name in COMMAND_REPLACEMENTS.items():
        rewritten = re.sub(rf"\b{re.escape(old_name)}\b", new_name, rewritten)
    return rewritten


def migrate_macro_file(path: str, in_place: bool = False) -> str:
    source = Path(path)
    original = source.read_text(encoding="utf-8")
    migrated = rewrite_macro_content(original)

    if in_place:
        source.write_text(migrated, encoding="utf-8")

    return migrated
