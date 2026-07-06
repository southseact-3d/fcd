"""CLI test for PartDesign_AdditiveHelix command.

Adds a helical solid. Hard to assert exact volume (depends on helix params),
so we just verify the object is created with a non-null Shape.
"""
from __future__ import annotations
COMMAND_ID = "PartDesign_AdditiveHelix"
TIMEOUT = 60
SCRIPT = r"""
import sys, json
import FreeCAD as App
import Part
import PartDesign
import Sketcher

doc = App.newDocument("test_additive_helix")
body = doc.addObject("PartDesign::Body", "Body")

# Profile: small circle that will trace the helix
profile = body.newObject("Sketcher::SketchObject", "ProfileSketch")
try:
    profile.AttachmentSupport = body.Origin.originFeatures[0]
    profile.MapMode = "FlatFace"
except Exception:
    pass
profile.addGeometry(Part.Circle(App.Vector(5, 0, 0), App.Vector(0, 0, 1), 1.0))
profile.addConstraint(Sketcher.Constraint("Radius", 0, 1.0))
doc.recompute()

helix = body.newObject("PartDesign::AdditiveHelix", "AdditiveHelix")
helix.Profile = profile
helix.ReferenceAxis = (body.Origin.OriginFeatures[0], '')  # XY plane normal = Z axis
# Try setting basic properties — these may vary by version
try:
    helix.Pitch = 2.0
    helix.Height = 10.0
except Exception:
    pass
doc.recompute()

errors = []
if helix is None:
    errors.append("AdditiveHelix object was not created")
else:
    if not helix.TypeId.startswith("PartDesign::AdditiveHelix"):
        errors.append(f"wrong TypeId: {helix.TypeId}")
    try:
        shape = helix.Shape
        if shape.isNull():
            # Don't fail hard — helix setup is fragile across versions
            errors.append("AdditiveHelix.Shape is null after recompute")
    except Exception as e:
        errors.append(f"AdditiveHelix.Shape access raised: {e}")

if body.Tip is not helix:
    errors.append(f"Body.Tip is {body.Tip}, expected the AdditiveHelix")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
