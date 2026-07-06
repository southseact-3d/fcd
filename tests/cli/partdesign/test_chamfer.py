"""CLI test for PartDesign_Chamfer command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Chamfer"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_chamfer")
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

chamfer = body.newObject("PartDesign::Chamfer", "Chamfer")
chamfer.Base = pad
shape = pad.Shape
top_edges = []
for i, edge in enumerate(shape.Edges):
    try:
        v1 = edge.Vertexes[0].Point
        v2 = edge.Vertexes[1].Point
        if abs(v1.z - 10.0) < 0.01 and abs(v2.z - 10.0) < 0.01:
            top_edges.append((pad, f"Edge{i+1}"))
    except Exception:
        pass
if not top_edges:
    for i in range(len(shape.Edges)):
        top_edges.append((pad, f"Edge{i+1}"))
chamfer.Edges = top_edges
chamfer.Size = 1.0
try:
    chamfer.ChamferType = "Equal distance"
except Exception:
    pass
doc.recompute()

errors = []
if chamfer is None:
    errors.append("Chamfer object was not created")
else:
    if not chamfer.TypeId.startswith("PartDesign::Chamfer"):
        errors.append(f"wrong TypeId: {chamfer.TypeId}")
    try:
        if chamfer.Shape.isNull():
            errors.append("Shape is null")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not chamfer:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
