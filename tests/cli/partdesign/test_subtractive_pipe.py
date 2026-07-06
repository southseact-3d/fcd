"""CLI test for PartDesign_SubtractivePipe command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_SubtractivePipe"
TIMEOUT = 60
SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_subtractive_pipe")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a 20x20 square, length 10
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

# Profile for the pipe: circle radius 3 at z=0
cut_profile = body.newObject("Sketcher::SketchObject", "CutProfile")
try:
    cut_profile.AttachmentSupport = body.Origin.originFeatures[0]
    cut_profile.MapMode = "FlatFace"
except Exception:
    pass
cut_profile.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 3.0))
cut_profile.addConstraint(Sketcher.Constraint("Radius", 0, 3.0))

# Path: line along Z axis from (0,0,0) to (0,0,10) in XZ plane
path = body.newObject("Sketcher::SketchObject", "PathSketch")
try:
    path.AttachmentSupport = body.Origin.originFeatures[1]
    path.MapMode = "FlatFace"
except Exception:
    pass
path.addGeometry(Part.LineSegment(App.Vector(0, 0, 0), App.Vector(0, 10, 0)))
doc.recompute()

pipe = body.newObject("PartDesign::SubtractivePipe", "SubtractivePipe")
pipe.Profile = cut_profile
pipe.Spine = [path]
pipe.Solid = True
doc.recompute()

errors = []
if pipe is None:
    errors.append("SubtractivePipe object was not created")
else:
    if not pipe.TypeId.startswith("PartDesign::SubtractivePipe"):
        errors.append(f"wrong TypeId: {pipe.TypeId}")
    try:
        shape = pipe.Shape
        if shape.isNull():
            errors.append("Shape is null")
        else:
            vol = shape.Volume
            # Pad vol = 4000. Pipe (cylinder cut) vol = pi*9*10 = ~282.7
            expected = 4000.0 - math.pi * 9.0 * 10.0
            if abs(vol - expected) > 5.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not pipe:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
