"""CLI test for PartDesign_Point command (datum point)."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Point"
TIMEOUT = 30
SCRIPT = r"""
import sys, json
import FreeCAD as App
import PartDesign

doc = App.newDocument("test_datum_point")
body = doc.addObject("PartDesign::Body", "Body")
point = body.newObject("PartDesign::Point", "DatumPoint")

errors = []
if point is None:
    errors.append("Point object was not created")
else:
    if not point.TypeId.startswith("PartDesign::Point"):
        errors.append(f"wrong TypeId: {point.TypeId}")
    doc.recompute()
    if not point.Name:
        errors.append("Point has empty Name after recompute")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
