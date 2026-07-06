"""CLI test for PartDesign_Mirrored command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Mirrored"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_mirrored")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a small square offset from YZ plane (at x=5..15)
sketch = body.newObject("Sketcher::SketchObject", "Sketch")
try:
    sketch.AttachmentSupport = body.Origin.originFeatures[0]
    sketch.MapMode = "FlatFace"
except Exception:
    pass
pts = [App.Vector(5,-5,0), App.Vector(15,-5,0), App.Vector(15,5,0), App.Vector(5,5,0)]
lines = []
for i in range(4):
    g = sketch.addGeometry(Part.LineSegment(pts[i], pts[(i+1)%4]))
    lines.append(g)
for i in range(4):
    sketch.addConstraint(Sketcher.Constraint("Coincident", lines[i], 2, lines[(i+1)%4], 1))
sketch.addConstraint(Sketcher.Constraint("Horizontal", lines[0]))
sketch.addConstraint(Sketcher.Constraint("Horizontal", lines[2]))
sketch.addConstraint(Sketcher.Constraint("Vertical", lines[1]))
sketch.addConstraint(Sketcher.Constraint("Vertical", lines[3]))
pad = body.newObject("PartDesign::Pad", "Pad")
pad.Profile = sketch
pad.Length = 5.0
pad.Type = "Length"
doc.recompute()

# Mirror across YZ plane (x=0)
mirror = body.newObject("PartDesign::Mirrored", "Mirrored")
mirror.Base = pad
try:
    mirror.MirrorPlane = (body.Origin.originFeatures[2], '')  # YZ plane = originFeatures[2]
except Exception:
    pass
doc.recompute()

errors = []
if mirror is None:
    errors.append("Mirrored object was not created")
else:
    if not mirror.TypeId.startswith("PartDesign::Mirrored"):
        errors.append(f"wrong TypeId: {mirror.TypeId}")
    try:
        if mirror.Shape.isNull():
            errors.append("Shape is null")
        else:
            vol = mirror.Shape.Volume
            # Original: 10*10*5 = 500. After mirror: 1000.
            if abs(vol - 1000.0) > 5.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~1000)")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not mirror:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
