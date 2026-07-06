"""CLI test for PartDesign_AdditiveLoft command.

Creates two sketches (circle at z=0, smaller circle at z=10) and lofts
between them to make a truncated cone.
"""
from __future__ import annotations
COMMAND_ID = "PartDesign_AdditiveLoft"
TIMEOUT = 60
SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part
import PartDesign
import Sketcher

doc = App.newDocument("test_additive_loft")
body = doc.addObject("PartDesign::Body", "Body")

# Sketch 1: circle radius 10 at z=0 (XY plane)
sketch1 = body.newObject("Sketcher::SketchObject", "Sketch1")
try:
    sketch1.AttachmentSupport = body.Origin.OriginFeatures[0]
    sketch1.MapMode = "FlatFace"
except Exception:
    pass
sketch1.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 10.0))
sketch1.addConstraint(Sketcher.Constraint("Radius", 0, 10.0))

# Sketch 2: circle radius 5 at z=10 — need a datum plane offset 10 from XY
plane = body.newObject("PartDesign::Plane", "Plane10")
plane.AttachmentOffset = App.Placement(App.Vector(0, 0, 10), App.Rotation(0, 0, 0))
doc.recompute()

sketch2 = body.newObject("Sketcher::SketchObject", "Sketch2")
try:
    sketch2.AttachmentSupport = plane
    sketch2.MapMode = "FlatFace"
except Exception:
    pass
sketch2.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 5.0))
sketch2.addConstraint(Sketcher.Constraint("Radius", 0, 5.0))
doc.recompute()

# Loft between the two sketches
loft = body.newObject("PartDesign::AdditiveLoft", "AdditiveLoft")
loft.Profile = sketch1
# Sections property is a list — add the second sketch
loft.Sections = [sketch2]
loft.Solid = True  # make a solid, not just a shell
doc.recompute()

errors = []
if loft is None:
    errors.append("AdditiveLoft object was not created")
else:
    if not loft.TypeId.startswith("PartDesign::AdditiveLoft"):
        errors.append(f"wrong TypeId: {loft.TypeId}")
    try:
        shape = loft.Shape
        if shape.isNull():
            errors.append("AdditiveLoft.Shape is null after recompute")
        else:
            vol = shape.Volume
            # Volume of a truncated cone: (1/3) * pi * h * (R^2 + R*r + r^2)
            # = (1/3) * pi * 10 * (100 + 50 + 25) = (1/3) * pi * 1750 = ~1832.6
            expected = (1.0/3.0) * math.pi * 10.0 * (100.0 + 50.0 + 25.0)
            if abs(vol - expected) > 5.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"AdditiveLoft.Shape access raised: {e}")

if body.Tip is not loft:
    errors.append(f"Body.Tip is {body.Tip}, expected the AdditiveLoft")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
