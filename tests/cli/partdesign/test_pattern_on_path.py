"""CLI test for PartDesign_PatternOnPath command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_PatternOnPath"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_pattern_on_path")
body = doc.addObject("PartDesign::Body", "Body")

# Profile: small circle at origin
profile_sketch = body.newObject("Sketcher::SketchObject", "Profile")
try:
    profile_sketch.AttachmentSupport = body.Origin.originFeatures[0]
    profile_sketch.MapMode = "FlatFace"
except Exception:
    pass
profile_sketch.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 1.0))
profile_sketch.addConstraint(Sketcher.Constraint("Radius", 0, 1.0))
pad = body.newObject("PartDesign::Pad", "Pad")
pad.Profile = profile_sketch
pad.Length = 2.0
pad.Type = "Length"
doc.recompute()

# Path: line from (5,0,0) to (15,0,0) in XY plane
path_sketch = body.newObject("Sketcher::SketchObject", "Path")
try:
    path_sketch.AttachmentSupport = body.Origin.originFeatures[0]
    path_sketch.MapMode = "FlatFace"
except Exception:
    pass
path_sketch.addGeometry(Part.LineSegment(App.Vector(5, 0, 0), App.Vector(15, 0, 0)))
doc.recompute()

pat = body.newObject("PartDesign::PatternOnPath", "PatternOnPath")
pat.Base = pad
try:
    pat.Path = [path_sketch]
    pat.Occurrences = 3
except Exception:
    pass
doc.recompute()

errors = []
if pat is None:
    errors.append("PatternOnPath object was not created")
else:
    if not pat.TypeId.startswith("PartDesign::PatternOnPath"):
        errors.append(f"wrong TypeId: {pat.TypeId}")
    try:
        if pat.Shape.isNull():
            errors.append("Shape is null")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not pat:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
