"""CLI test for PartDesign_CoordinateSystem command (local CS datum)."""
from __future__ import annotations
COMMAND_ID = "PartDesign_CoordinateSystem"
TIMEOUT = 30
SCRIPT = r"""
import sys, json
import FreeCAD as App
import PartDesign

doc = App.newDocument("test_coordinate_system")
body = doc.addObject("PartDesign::Body", "Body")
cs = body.newObject("PartDesign::CoordinateSystem", "LocalCS")

errors = []
if cs is None:
    errors.append("CoordinateSystem object was not created")
else:
    if not cs.TypeId.startswith("PartDesign::CoordinateSystem"):
        errors.append(f"wrong TypeId: {cs.TypeId}")
    doc.recompute()
    if not cs.Name:
        errors.append("CoordinateSystem has empty Name after recompute")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
