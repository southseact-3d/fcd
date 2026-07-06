"""Shared Python snippet for creating a Body + Sketch with a closed profile.

Used as a helper by test_pad.py, test_pocket.py, test_revolution.py,
test_groove.py, etc. Each test file embeds this as a string prefix to
avoid duplication.
"""

# A helper that creates a doc, a Body, a Sketch attached to XY plane,
# with a closed circular profile of given radius.
# Returns the document, body, sketch as JSON-decodable names.
SKETCH_HELPER = r'''
import sys, json, math
import FreeCAD as App
import Part
import PartDesign
import Sketcher

def make_doc_with_body_and_sketch(doc_name, body_name="Body", sketch_name="Sketch"):
    """Create a doc, add a Body, add a Sketch inside the Body,
    attach to XY plane, return (doc, body, sketch)."""
    doc = App.newDocument(doc_name)
    body = doc.addObject("PartDesign::Body", body_name)
    sketch = body.newObject("Sketcher::SketchObject", sketch_name)

    # Attach to Body's Origin XY plane (originFeatures[0] is XY)
    try:
        sketch.AttachmentSupport = body.Origin.OriginFeatures[0]
        sketch.MapMode = "FlatFace"
    except Exception:
        pass  # In CLI mode attachment may not fully resolve; geometry still works

    doc.recompute()
    return doc, body, sketch


def add_circle(sketch, radius=10.0, cx=0.0, cy=0.0):
    """Add a closed circle to the sketch and constrain its radius."""
    geom_idx = sketch.addGeometry(Part.Circle(App.Vector(cx, cy, 0), App.Vector(0, 0, 1), radius))
    if geom_idx < 0:
        raise RuntimeError(f"addGeometry returned {geom_idx}")
    sketch.addConstraint(Sketcher.Constraint("Radius", geom_idx, radius))
    return geom_idx


def add_rectangle(sketch, w=20.0, h=15.0, cx=0.0, cy=0.0):
    """Add a closed rectangle (4 lines + 4 coincident constraints + 1 horizontal + 1 vertical)."""
    x0, y0 = cx - w/2, cy - h/2
    x1, y1 = cx + w/2, cy + h/2
    l1 = sketch.addGeometry(Part.LineSegment(App.Vector(x0, y0, 0), App.Vector(x1, y0, 0)))
    l2 = sketch.addGeometry(Part.LineSegment(App.Vector(x1, y0, 0), App.Vector(x1, y1, 0)))
    l3 = sketch.addGeometry(Part.LineSegment(App.Vector(x1, y1, 0), App.Vector(x0, y1, 0)))
    l4 = sketch.addGeometry(Part.LineSegment(App.Vector(x0, y1, 0), App.Vector(x0, y0, 0)))
    sketch.addConstraint(Sketcher.Constraint("Coincident", l1, 2, l2, 1))
    sketch.addConstraint(Sketcher.Constraint("Coincident", l2, 2, l3, 1))
    sketch.addConstraint(Sketcher.Constraint("Coincident", l3, 2, l4, 1))
    sketch.addConstraint(Sketcher.Constraint("Coincident", l4, 2, l1, 1))
    sketch.addConstraint(Sketcher.Constraint("Horizontal", l1))
    sketch.addConstraint(Sketcher.Constraint("Horizontal", l3))
    sketch.addConstraint(Sketcher.Constraint("Vertical", l2))
    sketch.addConstraint(Sketcher.Constraint("Vertical", l4))
    return [l1, l2, l3, l4]
'''
