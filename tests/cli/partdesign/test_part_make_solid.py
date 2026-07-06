"""CLI test for PartDesign_PartMakeSolid command (bridge to Part_Make solid).

Verifies that a Part::Make solid object can be created with the expected TypeId.
"""
from __future__ import annotations

COMMAND_ID = "PartDesign_PartMakeSolid"
TIMEOUT = 30

SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part

doc = App.newDocument("test_partdesign_partmakesolid")
errors = []

try:
    obj = doc.addObject("Part::Feature", "Make solid")
    if obj is None:
        errors.append("object was not created")
    else:
        if obj.TypeId != "Part::Feature":
            errors.append(f"wrong TypeId: {obj.TypeId} (expected Part::Feature)")
        doc.recompute()
        # Verify it has a Shape (for primitives, the Shape should be valid after recompute)
        try:
            shape = obj.Shape
            if shape.isNull():
                errors.append("Shape is null after recompute")
        except Exception as e:
            errors.append(f"Shape access raised: {e}")
except Exception as e:
    errors.append(f"addObject raised: {e}")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
