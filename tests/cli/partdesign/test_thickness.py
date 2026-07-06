"""CLI test for PartDesign_Thickness command (Shell)."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Thickness"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_thickness")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a 20x20 square, length 10
sketch = body.newObject("Sketcher::SketchObject", "Sketch")
try:
    sketch.AttachmentSupport = body.Origin.originFeatures[0]
    sketch.MapMode = "FlatFace"
except Exception:
    pass
pts = [App.Vector(-10,-10,0), App.Vector(10,-10,0), App.Vector(10,10,0), App.Vector(-10,10,0)]
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
pad.Length = 10.0
pad.Type = "Length"
doc.recompute()

# Thickness: remove the top face (z=10), shell with 1mm walls
thickness = body.newObject("PartDesign::Thickness", "Thickness")
thickness.Base = pad
# Find the top face (Face with normal +Z, z=10)
shape = pad.Shape
top_face = None
for i, face in enumerate(shape.Faces):
    try:
        n = face.normalAt(0, 0)
        center = face.CenterOfMass
        if abs(n.z - 1.0) < 0.1 and abs(center.z - 10.0) < 0.5:
            top_face = (pad, f"Face{i+1}")
            break
    except Exception:
        pass
if top_face:
    thickness.Faces = [top_face]
try:
    thickness.Value = 1.0
except Exception:
    pass
try:
    thickness.Reversed = False
except Exception:
    pass
doc.recompute()

errors = []
if thickness is None:
    errors.append("Thickness object was not created")
else:
    if not thickness.TypeId.startswith("PartDesign::Thickness"):
        errors.append(f"wrong TypeId: {thickness.TypeId}")
    try:
        if thickness.Shape.isNull():
            errors.append("Shape is null")
        else:
            vol = thickness.Shape.Volume
            # Original 20*20*10 = 4000. After shell: 4000 - (18*18*9) = 4000 - 2916 = 1084
            # (interior 18*18*9 removed, leaving 1mm walls + 1mm bottom)
            if vol < 500 or vol > 2000:
                errors.append(f"unexpected shell volume: {vol:.2f}")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not thickness:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
