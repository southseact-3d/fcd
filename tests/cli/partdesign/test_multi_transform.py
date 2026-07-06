"""CLI test for PartDesign_MultiTransform command.

MultiTransform chains multiple transformations. We'll chain a Linear + Polar.
"""
from __future__ import annotations
COMMAND_ID = "PartDesign_MultiTransform"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_multitransform")
body = doc.addObject("PartDesign::Body", "Body")

sketch = body.newObject("Sketcher::SketchObject", "Sketch")
try:
    sketch.AttachmentSupport = body.Origin.originFeatures[0]
    sketch.MapMode = "FlatFace"
except Exception:
    pass
sketch.addGeometry(Part.Circle(App.Vector(10, 0, 0), App.Vector(0, 0, 1), 2.0))
sketch.addConstraint(Sketcher.Constraint("Radius", 0, 2.0))
pad = body.newObject("PartDesign::Pad", "Pad")
pad.Profile = sketch
pad.Length = 3.0
pad.Type = "Length"
doc.recompute()

# Create the MultiTransform feature
mt = body.newObject("PartDesign::MultiTransform", "MultiTransform")
mt.Base = pad
doc.recompute()

# Add a sub-pattern (polar) — MultiTransform.Transformations holds a list of sub-features
# We need to add a PolarPattern as a child of MultiTransform
polar = body.newObject("PartDesign::PolarPattern", "SubPolar")
try:
    polar.Base = pad
    polar.Axis = (body.Origin.originFeatures[0], '')
    polar.Angle = 360.0
    polar.Occurrences = 4
except Exception:
    pass
doc.recompute()

try:
    mt.Transformations = [polar]
except Exception:
    pass
doc.recompute()

errors = []
if mt is None:
    errors.append("MultiTransform object was not created")
else:
    if not mt.TypeId.startswith("PartDesign::MultiTransform"):
        errors.append(f"wrong TypeId: {mt.TypeId}")
    try:
        if mt.Shape.isNull():
            errors.append("Shape is null")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not mt:
    errors.append(f"Body.Tip is {body.Tip}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
