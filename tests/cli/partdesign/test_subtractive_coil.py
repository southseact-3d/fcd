"""CLI test for PartDesign_SubtractiveCoil command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_SubtractiveCoil"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_subtractive_coil")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a 20x20x10 block
sketch1 = body.newObject("Sketcher::SketchObject", "Sketch1")
try:
    sketch1.AttachmentSupport = body.Origin.originFeatures[0]
    sketch1.MapMode = "FlatFace"
except Exception:
    pass
pts = [App.Vector(-10,-10,0), App.Vector(10,-10,0), App.Vector(10,10,0), App.Vector(-10,10,0)]
lines = []
for i in range(4):
    g = sketch1.addGeometry(Part.LineSegment(pts[i], pts[(i+1)%4]))
    lines.append(g)
for i in range(4):
    sketch1.addConstraint(Sketcher.Constraint("Coincident", lines[i], 2, lines[(i+1)%4], 1))
sketch1.addConstraint(Sketcher.Constraint("Horizontal", lines[0]))
sketch1.addConstraint(Sketcher.Constraint("Horizontal", lines[2]))
sketch1.addConstraint(Sketcher.Constraint("Vertical", lines[1]))
sketch1.addConstraint(Sketcher.Constraint("Vertical", lines[3]))
pad = body.newObject("PartDesign::Pad", "Pad")
pad.Profile = sketch1
pad.Length = 10.0
pad.Type = "Length"
doc.recompute()

profile = body.newObject("Sketcher::SketchObject", "Profile")
try:
    profile.AttachmentSupport = body.Origin.originFeatures[0]
    profile.MapMode = "FlatFace"
except Exception:
    pass
profile.addGeometry(Part.Circle(App.Vector(5, 0, 0), App.Vector(0, 0, 1), 1.0))
profile.addConstraint(Sketcher.Constraint("Radius", 0, 1.0))
doc.recompute()

coil = body.newObject("PartDesign::SubtractiveCoil", "SubtractiveCoil")
coil.Profile = profile
try:
    coil.ReferenceAxis = (body.Origin.originFeatures[0], '')
except Exception:
    pass
doc.recompute()

errors = []
if coil is None:
    errors.append("SubtractiveCoil object was not created")
else:
    if not coil.TypeId.startswith("PartDesign::SubtractiveCoil"):
        errors.append(f"wrong TypeId: {coil.TypeId}")
    try:
        if coil.Shape.isNull():
            errors.append("Shape is null")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not coil:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
