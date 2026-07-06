"""CLI test for PartDesign_StoneTexture command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_StoneTexture"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_stone_texture")
body = doc.addObject("PartDesign::Body", "Body")

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
pad.Length = 5.0
pad.Type = "Length"
doc.recompute()

bt = body.newObject("PartDesign::StoneTexture", "StoneTexture")
bt.Base = pad
shape = pad.Shape
top_face = None
for i, face in enumerate(shape.Faces):
    try:
        n = face.normalAt(0, 0)
        if abs(n.z - 1.0) < 0.1:
            top_face = (pad, f"Face{i+1}")
            break
    except Exception:
        pass
if top_face:
    try:
        bt.Faces = [top_face]
    except Exception:
        pass
doc.recompute()

errors = []
if bt is None:
    errors.append("StoneTexture object was not created")
else:
    if not bt.TypeId.startswith("PartDesign::StoneTexture"):
        errors.append(f"wrong TypeId: {bt.TypeId}")
    try:
        if bt.Shape.isNull():
            errors.append("Shape is null")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not bt:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
