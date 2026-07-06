"""CLI test for PartDesign_AdditiveCoil command."""
from __future__ import annotations
COMMAND_ID = "PartDesign_AdditiveCoil"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_additive_coil")
body = doc.addObject("PartDesign::Body", "Body")
profile = body.newObject("Sketcher::SketchObject", "Profile")
try:
    profile.AttachmentSupport = body.Origin.originFeatures[0]
    profile.MapMode = "FlatFace"
except Exception:
    pass
profile.addGeometry(Part.Circle(App.Vector(5, 0, 0), App.Vector(0, 0, 1), 1.0))
profile.addConstraint(Sketcher.Constraint("Radius", 0, 1.0))
doc.recompute()

coil = body.newObject("PartDesign::AdditiveCoil", "AdditiveCoil")
coil.Profile = profile
try:
    coil.ReferenceAxis = (body.Origin.OriginFeatures[0], '')
except Exception:
    pass
doc.recompute()

errors = []
if coil is None:
    errors.append("AdditiveCoil object was not created")
else:
    if not coil.TypeId.startswith("PartDesign::AdditiveCoil"):
        errors.append(f"wrong TypeId: {coil.TypeId}")
    try:
        if coil.Shape.isNull():
            errors.append("Shape is null")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if body.Tip is not coil:
    errors.append(f"Body.Tip is {body.Tip}, expected the AdditiveCoil")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
