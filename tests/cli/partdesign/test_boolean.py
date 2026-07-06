"""CLI test for PartDesign_Boolean command (Body-level Boolean)."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Boolean"
TIMEOUT = 60
SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_boolean")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a 20x20x5 base
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
pad.Length = 5.0
pad.Type = "Length"
doc.recompute()

# Create a second body with a cylinder (to fuse with the first)
body2 = doc.addObject("PartDesign::Body", "Body2")
sketch2 = body2.newObject("Sketcher::SketchObject", "Sketch2")
try:
    sketch2.AttachmentSupport = body2.Origin.originFeatures[0]
    sketch2.MapMode = "FlatFace"
except Exception:
    pass
sketch2.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 5.0))
sketch2.addConstraint(Sketcher.Constraint("Radius", 0, 5.0))
pad2 = body2.newObject("PartDesign::Pad", "Pad2")
pad2.Profile = sketch2
pad2.Length = 8.0
pad2.Type = "Length"
doc.recompute()

# Boolean fuse: body + body2
boolean = body.newObject("PartDesign::Boolean", "Boolean")
boolean.Base = pad
try:
    boolean.Tool = [pad2]
    boolean.Type = "Fuse"  # or 0
except Exception:
    pass
doc.recompute()

errors = []
if boolean is None:
    errors.append("Boolean object was not created")
else:
    if not boolean.TypeId.startswith("PartDesign::Boolean"):
        errors.append(f"wrong TypeId: {boolean.TypeId}")
    try:
        if boolean.Shape.isNull():
            errors.append("Shape is null")
        else:
            vol = boolean.Shape.Volume
            # Pad 20*20*5 = 2000. Cylinder pi*25*8 = ~628.3
            # Fuse: 2000 + 628.3 = 2628.3 (cylinder pokes through pad, no overlap removed)
            expected = 2000.0 + math.pi * 25.0 * 8.0
            if abs(vol - expected) > 20.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not boolean:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
