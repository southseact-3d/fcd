"""CLI test for PartDesign_Plane command (datum plane)."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Plane"
TIMEOUT = 30
SCRIPT = r"""
import sys, json
import FreeCAD as App
import PartDesign

doc = App.newDocument("test_datum_plane")
body = doc.addObject("PartDesign::Body", "Body")

plane = body.newObject("PartDesign::Plane", "DatumPlane")
errors = []
if plane is None:
    errors.append("Plane object was not created")
else:
    if not plane.TypeId.startswith("PartDesign::Plane"):
        errors.append(f"wrong TypeId: {plane.TypeId}")
    # A new datum plane with no attachment should still be a valid object
    doc.recompute()
    if not plane.Name:
        errors.append("Plane has empty Name after recompute")

if body.Tip is not None and body.Tip is not plane:
    pass  # Datum objects don't become the Tip, that's fine

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
