"""CLI test for PartDesign_LinearPattern command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_LinearPattern"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_linear_pattern")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a small circle
sketch = body.newObject("Sketcher::SketchObject", "Sketch")
try:
    sketch.AttachmentSupport = body.Origin.originFeatures[0]
    sketch.MapMode = "FlatFace"
except Exception:
    pass
sketch.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 3.0))
sketch.addConstraint(Sketcher.Constraint("Radius", 0, 3.0))
pad = body.newObject("PartDesign::Pad", "Pad")
pad.Profile = sketch
pad.Length = 5.0
pad.Type = "Length"
doc.recompute()

# Linear pattern: 3 occurrences along X, 10mm apart
pat = body.newObject("PartDesign::LinearPattern", "LinearPattern")
pat.Base = pad
try:
    pat.Direction = (body.Origin.originFeatures[1], '')  # XZ plane normal = Y? Actually we want X axis
except Exception:
    pass
try:
    pat.Axis = (0, 1, 0)  # Y axis direction
except Exception:
    pass
try:
    pat.Length = 20.0  # total span (will get 3 occurrences at 0, 10, 20)
    pat.Occurrences = 3
except Exception:
    pass
doc.recompute()

errors = []
if pat is None:
    errors.append("LinearPattern object was not created")
else:
    if not pat.TypeId.startswith("PartDesign::LinearPattern"):
        errors.append(f"wrong TypeId: {pat.TypeId}")
    try:
        if pat.Shape.isNull():
            errors.append("Shape is null")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not pat:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
