"""CLI test for PartDesign_Scaled command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Scaled"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_scaled")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a small cube
sketch = body.newObject("Sketcher::SketchObject", "Sketch")
try:
    sketch.AttachmentSupport = body.Origin.originFeatures[0]
    sketch.MapMode = "FlatFace"
except Exception:
    pass
pts = [App.Vector(0,0,0), App.Vector(10,0,0), App.Vector(10,10,0), App.Vector(0,10,0)]
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

# Scale by 2x
scaled = body.newObject("PartDesign::Scaled", "Scaled")
scaled.Base = pad
try:
    scaled.Factor = 2.0
    scaled.Occurrences = 1
except Exception:
    pass
doc.recompute()

errors = []
if scaled is None:
    errors.append("Scaled object was not created")
else:
    if not scaled.TypeId.startswith("PartDesign::Scaled"):
        errors.append(f"wrong TypeId: {scaled.TypeId}")
    try:
        if scaled.Shape.isNull():
            errors.append("Shape is null")
        else:
            vol = scaled.Shape.Volume
            # Original 10*10*10 = 1000, scaled 2x = 8000
            if abs(vol - 8000.0) > 10.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~8000)")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not scaled:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
