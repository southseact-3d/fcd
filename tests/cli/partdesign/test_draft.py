"""CLI test for PartDesign_Draft command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Draft"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_draft")
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
pad.Length = 10.0
pad.Type = "Length"
doc.recompute()

draft = body.newObject("PartDesign::Draft", "Draft")
draft.Base = pad
# Pull direction: +Z
try:
    draft.Direction = (pad, 'Edge1')  # any vertical edge will give Z direction
except Exception:
    pass
try:
    draft.PullDirection = App.Vector(0, 0, 1)
except Exception:
    pass
# Neutral plane: XY plane (z=0)
try:
    draft.ReferenceAxis = (body.Origin.originFeatures[0], '')
except Exception:
    pass
try:
    draft.NeutralPlane = body.Origin.originFeatures[0]
except Exception:
    pass
try:
    draft.Angle = 5.0
except Exception:
    pass

# Select vertical faces for drafting
shape = pad.Shape
vertical_faces = []
for i, face in enumerate(shape.Faces):
    try:
        # Vertical faces have a normal with z-component near 0
        n = face.normalAt(0, 0)
        if abs(n.z) < 0.1:
            vertical_faces.append((pad, f"Face{i+1}"))
    except Exception:
        pass
try:
    draft.Faces = vertical_faces
except Exception:
    pass
doc.recompute()

errors = []
if draft is None:
    errors.append("Draft object was not created")
else:
    if not draft.TypeId.startswith("PartDesign::Draft"):
        errors.append(f"wrong TypeId: {draft.TypeId}")
    try:
        if draft.Shape.isNull():
            errors.append("Shape is null")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not draft:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
