"""CLI test for PartDesign_Extrude command (standalone, not body-aware)."""
from __future__ import annotations
COMMAND_ID = "PartDesign_Extrude"
TIMEOUT = 60
SCRIPT = r"""
import sys, json, math
import FreeCAD as App
import Part, PartDesign, Sketcher

doc = App.newDocument("test_extrude")
# PartDesign_Extrude operates on a Part::Feature (not a Body feature)
sketch = doc.addObject("Sketcher::SketchObjectSketch", "Sketch")
sketch.addGeometry(Part.Circle(App.Vector(0, 0, 0), App.Vector(0, 0, 1), 10.0))
sketch.addConstraint(Sketcher.Constraint("Radius", 0, 10.0))
doc.recompute()

# Extrude the sketch
import Part
extrude = doc.addObject("Part::Extrusion", "Extrude")
extrude.Base = sketch
extrude.Dir = App.Vector(0, 0, 1)
extrude.LenFwd = 5.0
doc.recompute()

errors = []
if extrude is None:
    errors.append("Extrude object was not created")
else:
    try:
        shape = extrude.Shape
        if shape.isNull():
            errors.append("Shape is null")
        else:
            vol = shape.Volume
            expected = math.pi * 100.0 * 5.0
            if abs(vol - expected) > 1.0:
                errors.append(f"unexpected volume: {vol:.2f} (expected ~{expected:.2f})")
    except Exception as e:
        errors.append(f"Shape access raised: {e}")
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
