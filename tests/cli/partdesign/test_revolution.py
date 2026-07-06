"""CLI test for PartDesign_Revolution command.

Creates a Body + Sketch with a rectangle offset from the Y axis,
then revolves it 360° around the Y axis to make a tube/ring.
Verifies the resulting volume.
"""
from __future__ import annotations

COMMAND_ID = "PartDesign_Revolution"
TIMEOUT = 60

SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part
import PartDesign
import Sketcher

doc = App.newDocument("test_revolution")
body = doc.addObject("PartDesign::Body", "Body")
sketch = body.newObject("Sketcher::SketchObject", "Sketch")
try:
    sketch.AttachmentSupport = body.Origin.OriginFeatures[0]
    sketch.MapMode = "FlatFace"
except Exception:
    pass

# Rectangle from x=5 to x=10, y=-5 to y=5 (offset from Y axis)
# When revolved around the Y axis, this makes a tube/ring.
l1 = sketch.addGeometry(Part.LineSegment(App.Vector(5, -5, 0), App.Vector(10, -5, 0)))
l2 = sketch.addGeometry(Part.LineSegment(App.Vector(10, -5, 0), App.Vector(10, 5, 0)))
l3 = sketch.addGeometry(Part.LineSegment(App.Vector(10, 5, 0), App.Vector(5, 5, 0)))
l4 = sketch.addGeometry(Part.LineSegment(App.Vector(5, 5, 0), App.Vector(5, -5, 0)))
sketch.addConstraint(Sketcher.Constraint("Coincident", l1, 2, l2, 1))
sketch.addConstraint(Sketcher.Constraint("Coincident", l2, 2, l3, 1))
sketch.addConstraint(Sketcher.Constraint("Coincident", l3, 2, l4, 1))
sketch.addConstraint(Sketcher.Constraint("Coincident", l4, 2, l1, 1))
sketch.addConstraint(Sketcher.Constraint("Horizontal", l1))
sketch.addConstraint(Sketcher.Constraint("Horizontal", l3))
sketch.addConstraint(Sketcher.Constraint("Vertical", l2))
sketch.addConstraint(Sketcher.Constraint("Vertical", l4))
# Distance constraints to lock dimensions
sketch.addConstraint(Sketcher.Constraint("DistanceX", App.Vector(0,0,0), l4, 1, 5.0))  # left edge at x=5
sketch.addConstraint(Sketcher.Constraint("DistanceX", App.Vector(0,0,0), l2, 1, 10.0))  # right edge at x=10
sketch.addConstraint(Sketcher.Constraint("DistanceY", App.Vector(0,0,0), l1, 1, -5.0))  # bottom at y=-5
sketch.addConstraint(Sketcher.Constraint("DistanceY", App.Vector(0,0,0), l3, 1, 5.0))   # top at y=5
doc.recompute()

# Revolution around Y axis
rev = body.newObject("PartDesign::Revolution", "Revolution")
rev.Profile = sketch
rev.Axis = (0, 1, 0)  # Y axis
rev.Base = (0, 0, 0)
rev.Angle = 360.0
doc.recompute()

# --- Assertions ---
errors = []
if rev is None:
    errors.append("Revolution object was not created")
else:
    if not rev.TypeId.startswith("PartDesign::Revolution"):
        errors.append(f"wrong TypeId: {rev.TypeId}")
    try:
        shape = rev.Shape
        if shape.isNull():
            errors.append("Revolution.Shape is null after recompute")
        else:
            vol = shape.Volume
            # Volume of a tube = pi * (R^2 - r^2) * h, where R=10, r=5, h=10 (height of revolved rect)
            # = pi * (100 - 25) * 10 = pi * 750 = ~2356.2
            expected = math.pi * (100.0 - 25.0) * 10.0
            if abs(vol - expected) > 5.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"Revolution.Shape access raised: {e}")

if body.Tip is not rev:
    errors.append(f"Body.Tip is {body.Tip}, expected the Revolution")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
else:
    print("TEST_PASS")
    sys.exit(0)
"""
