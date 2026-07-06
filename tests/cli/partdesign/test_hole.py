"""CLI test for PartDesign_Hole command.

Creates a Pad with a circle, then drills a ThroughAll hole on top.
Verifies the volume decreases by the expected amount.
"""
from __future__ import annotations
COMMAND_ID = "PartDesign_Hole"
TIMEOUT = 60
SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_hole")
body = doc.addObject("PartDesign::Body", "Body")

# Pad a circle radius 10, length 5
sketch1 = body.newObject("Sketcher::SketchObject", "Sketch1")
try:
    sketch1.AttachmentSupport = body.Origin.originFeatures[0]
    sketch1.MapMode = "FlatFace"
except Exception:
    pass
sketch1.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 10.0))
sketch1.addConstraint(Sketcher.Constraint("Radius", 0, 10.0))
pad = body.newObject("PartDesign::Pad", "Pad")
pad.Profile = sketch1
pad.Length = 5.0
pad.Type = "Length"
doc.recompute()

# Hole sketch: circle radius 2 at origin (will be drilled through)
sketch2 = body.newObject("Sketcher::SketchObject", "Sketch2")
try:
    sketch2.AttachmentSupport = body.Origin.originFeatures[0]
    sketch2.MapMode = "FlatFace"
except Exception:
    pass
sketch2.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 2.0))
sketch2.addConstraint(Sketcher.Constraint("Radius", 0, 2.0))
doc.recompute()

hole = body.newObject("PartDesign::Hole", "Hole")
hole.Profile = sketch2
hole.DepthType = "ThroughAll"
# Diameter is driven by the sketch radius (2mm radius = 4mm diameter)
doc.recompute()

errors = []
if hole is None:
    errors.append("Hole object was not created")
else:
    if not hole.TypeId.startswith("PartDesign::Hole"):
        errors.append(f"wrong TypeId: {hole.TypeId}")
    try:
        shape = hole.Shape
        if shape.isNull():
            errors.append("Hole.Shape is null after recompute")
        else:
            vol = shape.Volume
            # Expected: pi*100*5 - pi*4*5 = pi*480 = ~1508.0
            expected = math.pi * (100.0 * 5.0 - 4.0 * 5.0)
            if abs(vol - expected) > 2.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"Hole.Shape access raised: {e}")
if body.Tip is not hole:
    errors.append(f"Body.Tip is {body.Tip}, expected the Hole")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
