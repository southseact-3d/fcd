"""CLI test for PartDesign_Line command (datum line / axis)."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Line"
TIMEOUT = 30
SCRIPT = r"""
import sys, json
import FreeCAD as App
import PartDesign

doc = App.newDocument("test_datum_line")
body = doc.addObject("PartDesign::Body", "Body")
line = body.newObject("PartDesign::Line", "DatumLine")

errors = []
if line is None:
    errors.append("Line object was not created")
else:
    if not line.TypeId.startswith("PartDesign::Line"):
        errors.append(f"wrong TypeId: {line.TypeId}")
    doc.recompute()
    if not line.Name:
        errors.append("Line has empty Name after recompute")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
