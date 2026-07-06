"""CLI test for PartDesign_Groove command.

Creates a Body with a Revolution (tube), then grooves a smaller circle
through it to make a hollow tube with an axial hole.
"""
from __future__ import annotations

COMMAND_ID = "PartDesign_Groove"
TIMEOUT = 60

SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part
import PartDesign
import Sketcher

doc = App.newDocument("test_groove")
body = doc.addObject("PartDesign::Body", "Body")

# Sketch 1: rectangle from x=5 to x=10, y=-5 to y=5 — revolve around Y to make a tube
sketch1 = body.newObject("Sketcher::SketchObject", "Sketch1")
try:
    sketch1.AttachmentSupport = body.Origin.originFeatures[0]
    sketch1.MapMode = "FlatFace"
except Exception:
    pass
l1 = sketch1.addGeometry(Part.LineSegment(App.Vector(5, -5, 0), App.Vector(10, -5, 0)))
l2 = sketch1.addGeometry(Part.LineSegment(App.Vector(10, -5, 0), App.Vector(10, 5, 0)))
l3 = sketch1.addGeometry(Part.LineSegment(App.Vector(10, 5, 0), App.Vector(5, 5, 0)))
l4 = sketch1.addGeometry(Part.LineSegment(App.Vector(5, 5, 0), App.Vector(5, -5, 0)))
sketch1.addConstraint(Sketcher.Constraint("Coincident", l1, 2, l2, 1))
sketch1.addConstraint(Sketcher.Constraint("Coincident", l2, 2, l3, 1))
sketch1.addConstraint(Sketcher.Constraint("Coincident", l3, 2, l4, 1))
sketch1.addConstraint(Sketcher.Constraint("Coincident", l4, 2, l1, 1))
sketch1.addConstraint(Sketcher.Constraint("Horizontal", l1))
sketch1.addConstraint(Sketcher.Constraint("Horizontal", l3))
sketch1.addConstraint(Sketcher.Constraint("Vertical", l2))
sketch1.addConstraint(Sketcher.Constraint("Vertical", l4))
rev = body.newObject("PartDesign::Revolution", "Revolution")
rev.Profile = sketch1
rev.Axis = (0, 1, 0)
rev.Base = (0, 0, 0)
rev.Angle = 360.0
doc.recompute()

# Sketch 2: circle at x=7.5 (midpoint of tube wall), radius 1 — groove it around Y
sketch2 = body.newObject("Sketcher::SketchObject", "Sketch2")
try:
    sketch2.AttachmentSupport = body.Origin.originFeatures[0]
    sketch2.MapMode = "FlatFace"
except Exception:
    pass
sketch2.addGeometry(Part.Circle(App.Vector(7.5, 0, 0), App.Vector(0, 0, 1), 1.0))
sketch2.addConstraint(Sketcher.Constraint("Radius", 0, 1.0))

# Groove around Y axis
groove = body.newObject("PartDesign::Groove", "Groove")
groove.Profile = sketch2
groove.Axis = (0, 1, 0)
groove.Base = (0, 0, 0)
groove.Angle = 360.0
doc.recompute()

# --- Assertions ---
errors = []
if groove is None:
    errors.append("Groove object was not created")
else:
    if not groove.TypeId.startswith("PartDesign::Groove"):
        errors.append(f"wrong TypeId: {groove.TypeId}")
    try:
        shape = groove.Shape
        if shape.isNull():
            errors.append("Groove.Shape is null after recompute")
        else:
            vol = shape.Volume
            # The groove cuts a torus out of the tube.
            # Tube volume: pi * (10^2 - 5^2) * 10 = pi * 750
            # Torus volume: 2 * pi^2 * R * r^2 = 2 * pi^2 * 7.5 * 1 = ~148.0
            tube_vol = math.pi * (100.0 - 25.0) * 10.0
            torus_vol = 2.0 * math.pi * math.pi * 7.5 * 1.0 * 1.0
            expected = tube_vol - torus_vol
            if abs(vol - expected) > 5.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"Groove.Shape access raised: {e}")

if body.Tip is not groove:
    errors.append(f"Body.Tip is {body.Tip}, expected the Groove")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
else:
    print("TEST_PASS")
    sys.exit(0)
"""
