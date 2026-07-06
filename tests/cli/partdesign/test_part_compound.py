"""CLI test for PartDesign_PartCompound command (bridge to Part_Compound).

Verifies that a Part::Compound object can be created with the expected TypeId.
"""
from __future__ import annotations

COMMAND_ID = "PartDesign_PartCompound"
TIMEOUT = 30

SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part

doc = App.newDocument("test_partdesign_partcompound")
errors = []

try:
    obj = doc.addObject("Part::Compound", "Compound")
    if obj is None:
        errors.append("object was not created")
    else:
        if obj.TypeId != "Part::Compound":
            errors.append(f"wrong TypeId: {obj.TypeId} (expected Part::Compound)")
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
