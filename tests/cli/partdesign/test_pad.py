"""CLI test for PartDesign_Pad command.

Creates a Body + Sketch with a circle, then pads it 5mm.
Verifies the Pad has the expected volume (~pi * r^2 * h = pi * 100 * 5 = 1570.8).
"""
from __future__ import annotations

COMMAND_ID = "PartDesign_Pad"
TIMEOUT = 60

SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part
import PartDesign
import Sketcher

# --- Setup: doc, body, sketch with a circle ---
doc = App.newDocument("test_pad")
body = doc.addObject("PartDesign::Body", "Body")
sketch = body.newObject("Sketcher::SketchObject", "Sketch")
try:
    sketch.AttachmentSupport = body.Origin.OriginFeatures[0]
    sketch.MapMode = "FlatFace"
except Exception:
    pass

sketch.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 10.0))
sketch.addConstraint(Sketcher.Constraint("Radius", 0, 10.0))
doc.recompute()

# --- Action: create a Pad ---
pad = body.newObject("PartDesign::Pad", "Pad")
pad.Profile = sketch
pad.Length = 5.0
pad.Type = "Length"  # Dimension mode (vs UpToLast/UpToFace)
doc.recompute()

# --- Assertions ---
errors = []
if pad is None:
    errors.append("Pad object was not created")
else:
    if not pad.TypeId.startswith("PartDesign::Pad"):
        errors.append(f"wrong TypeId: {pad.TypeId}")
    try:
        shape = pad.Shape
        if shape.isNull():
            errors.append("Pad.Shape is null after recompute")
        else:
            vol = shape.Volume
            expected = math.pi * 100.0 * 5.0  # pi * r^2 * h
            if abs(vol - expected) > 1.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"Pad.Shape access raised: {e}")

# Verify the Pad is the tip of the body
if body.Tip is not pad:
    errors.append(f"Body.Tip is {body.Tip}, expected the Pad")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
else:
    print("TEST_PASS")
    sys.exit(0)
"""
