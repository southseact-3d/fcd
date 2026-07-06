"""CLI test for PartDesign_Pocket command.

Creates a Body with a Pad (cylinder), then pockets a smaller circle
through it to make a tube. Verifies the resulting volume.
"""
from __future__ import annotations

COMMAND_ID = "PartDesign_Pocket"
TIMEOUT = 60

SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part
import PartDesign
import Sketcher

doc = App.newDocument("test_pocket")
body = doc.addObject("PartDesign::Body", "Body")

# Sketch 1: outer circle (radius 10)
sketch1 = body.newObject("Sketcher::SketchObject", "Sketch1")
try:
    sketch1.AttachmentSupport = body.Origin.OriginFeatures[0]
    sketch1.MapMode = "FlatFace"
except Exception:
    pass
sketch1.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 10.0))
sketch1.addConstraint(Sketcher.Constraint("Radius", 0, 10.0))

# Pad the outer circle to make a cylinder of length 5
pad = body.newObject("PartDesign::Pad", "Pad")
pad.Profile = sketch1
pad.Length = 5.0
pad.Type = "Length"
doc.recompute()

# Sketch 2: inner circle (radius 5), on the top face of the pad
# For CLI simplicity, attach to the same XY plane — the pocket will still cut through.
sketch2 = body.newObject("Sketcher::SketchObject", "Sketch2")
try:
    sketch2.AttachmentSupport = body.Origin.originFeatures[0]
    sketch2.MapMode = "FlatFace"
except Exception:
    pass
sketch2.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 5.0))
sketch2.addConstraint(Sketcher.Constraint("Radius", 0, 5.0))
doc.recompute()

# Pocket the inner circle through all
pocket = body.newObject("PartDesign::Pocket", "Pocket")
pocket.Profile = sketch2
pocket.Type = "ThroughAll"
doc.recompute()

# --- Assertions ---
errors = []
if pocket is None:
    errors.append("Pocket object was not created")
else:
    if not pocket.TypeId.startswith("PartDesign::Pocket"):
        errors.append(f"wrong TypeId: {pocket.TypeId}")
    try:
        shape = pocket.Shape
        if shape.isNull():
            errors.append("Pocket.Shape is null after recompute")
        else:
            vol = shape.Volume
            # Expected: outer cylinder (pi * 100 * 5) minus inner cylinder (pi * 25 * 5)
            expected = math.pi * (100.0 - 25.0) * 5.0  # = pi * 375 = ~1178.1
            if abs(vol - expected) > 2.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"Pocket.Shape access raised: {e}")

if body.Tip is not pocket:
    errors.append(f"Body.Tip is {body.Tip}, expected the Pocket")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
else:
    print("TEST_PASS")
    sys.exit(0)
"""
