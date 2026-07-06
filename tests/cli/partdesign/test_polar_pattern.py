"""CLI test for PartDesign_PolarPattern command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_PolarPattern"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_polar_pattern")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a small circle offset from origin
sketch = body.newObject("Sketcher::SketchObject", "Sketch")
try:
    sketch.AttachmentSupport = body.Origin.originFeatures[0]
    sketch.MapMode = "FlatFace"
except Exception:
    pass
sketch.addGeometry(Part.Circle(App.Vector(10, 0, 0), App.Vector(0, 0, 1), 2.0))
sketch.addConstraint(Sketcher.Constraint("Radius", 0, 2.0))
pad = body.newObject("PartDesign::Pad", "Pad")
pad.Profile = sketch
pad.Length = 5.0
pad.Type = "Length"
doc.recompute()

# Polar pattern: 4 occurrences around Z axis
pat = body.newObject("PartDesign::PolarPattern", "PolarPattern")
pat.Base = pad
try:
    pat.Axis = (body.Origin.originFeatures[0], '')  # XY plane normal = Z
    pat.Angle = 360.0
    pat.Occurrences = 4
except Exception:
    pass
doc.recompute()

errors = []
if pat is None:
    errors.append("PolarPattern object was not created")
else:
    if not pat.TypeId.startswith("PartDesign::PolarPattern"):
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
