"""CLI test for PartDesign_AdditivePipe command.

Sweeps a circle along a straight path to make a cylinder (verifies the
sweep machinery works).
"""
from __future__ import annotations
COMMAND_ID = "PartDesign_AdditivePipe"
TIMEOUT = 60
SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part
import PartDesign
import Sketcher

doc = App.newDocument("test_additive_pipe")
body = doc.addObject("PartDesign::Body", "Body")

# Profile: circle radius 5 in XY plane (will be swept along Z)
profile = body.newObject("Sketcher::SketchObject", "ProfileSketch")
try:
    profile.AttachmentSupport = body.Origin.OriginFeatures[0]
    profile.MapMode = "FlatFace"
except Exception:
    pass
profile.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 5.0))
profile.addConstraint(Sketcher.Constraint("Radius", 0, 5.0))

# Path: line from (0,0,0) to (0,0,10) in XZ plane
# Need a sketch in the XZ plane — originFeatures[1] is XZ in FreeCAD
path = body.newObject("Sketcher::SketchObject", "PathSketch")
try:
    path.AttachmentSupport = body.Origin.originFeatures[1]  # XZ plane
    path.MapMode = "FlatFace"
except Exception:
    pass
# In XZ plane, y maps to z. So line from (0,0) to (0,10) sweeps along Z axis.
path.addGeometry(Part.LineSegment(App.Vector(0, 0, 0), App.Vector(0, 10, 0)))
doc.recompute()

# Pipe (sweep) the profile along the path
pipe = body.newObject("PartDesign::AdditivePipe", "AdditivePipe")
pipe.Profile = profile
pipe.Spine = [path]
pipe.Solid = True
doc.recompute()

errors = []
if pipe is None:
    errors.append("AdditivePipe object was not created")
else:
    if not pipe.TypeId.startswith("PartDesign::AdditivePipe"):
        errors.append(f"wrong TypeId: {pipe.TypeId}")
    try:
        shape = pipe.Shape
        if shape.isNull():
            errors.append("AdditivePipe.Shape is null after recompute")
        else:
            vol = shape.Volume
            # Cylinder: pi * r^2 * h = pi * 25 * 10 = ~785.4
            expected = math.pi * 25.0 * 10.0
            if abs(vol - expected) > 5.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"AdditivePipe.Shape access raised: {e}")

if body.Tip is not pipe:
    errors.append(f"Body.Tip is {body.Tip}, expected the AdditivePipe")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
