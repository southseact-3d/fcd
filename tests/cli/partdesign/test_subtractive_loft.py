"""CLI test for PartDesign_SubtractiveLoft command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_SubtractiveLoft"
TIMEOUT = 60
SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_subtractive_loft")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a 20x20 square, length 10
sketch1 = body.newObject("Sketcher::SketchObject", "Sketch1")
try:
    sketch1.AttachmentSupport = body.Origin.originFeatures[0]
    sketch1.MapMode = "FlatFace"
except Exception:
    pass
import Part
# 20x20 rectangle from (-10,-10) to (10,10)
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

# Two sketches for the loft cut: circle radius 5 at z=0, circle radius 3 at z=10
plane10 = body.newObject("PartDesign::Plane", "Plane10")
plane10.AttachmentOffset = App.Placement(App.Vector(0, 0, 10), App.Rotation(0, 0, 0))
doc.recompute()

cut_sketch1 = body.newObject("Sketcher::SketchObject", "CutSketch1")
try:
    cut_sketch1.AttachmentSupport = body.Origin.originFeatures[0]
    cut_sketch1.MapMode = "FlatFace"
except Exception:
    pass
cut_sketch1.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 5.0))
cut_sketch1.addConstraint(Sketcher.Constraint("Radius", 0, 5.0))

cut_sketch2 = body.newObject("Sketcher::SketchObject", "CutSketch2")
try:
    cut_sketch2.AttachmentSupport = plane10
    cut_sketch2.MapMode = "FlatFace"
except Exception:
    pass
cut_sketch2.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 3.0))
cut_sketch2.addConstraint(Sketcher.Constraint("Radius", 0, 3.0))
doc.recompute()

loft = body.newObject("PartDesign::SubtractiveLoft", "SubtractiveLoft")
loft.Profile = cut_sketch1
loft.Sections = [cut_sketch2]
loft.Solid = True
doc.recompute()

errors = []
if loft is None:
    errors.append("SubtractiveLoft object was not created")
else:
    if not loft.TypeId.startswith("PartDesign::SubtractiveLoft"):
        errors.append(f"wrong TypeId: {loft.TypeId}")
    try:
        shape = loft.Shape
        if shape.isNull():
            errors.append("Shape is null")
        else:
            vol = shape.Volume
            # Pad vol = 20*20*10 = 4000
            # Loft (truncated cone) vol = (1/3)*pi*10*(25+15+9) = (1/3)*pi*490 = ~513.1
            expected = 4000.0 - (1.0/3.0) * math.pi * 10.0 * (25.0 + 15.0 + 9.0)
            if abs(vol - expected) > 5.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not loft:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
