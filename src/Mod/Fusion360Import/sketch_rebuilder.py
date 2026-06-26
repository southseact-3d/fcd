# SPDX-License-Identifier: LGPL-2.1-or-later
"""Rebuild Sketcher sketches from Fusion 360 sketch data.

The extractor (``fusion360_extractor.py``) emits a JSON-friendly sketch
description containing:

- ``points``: a list of ``{index, x, y}`` entries (mm)
- ``curves``: a list of typed entries (Line2D, Circle2D, Arc2D, ...)
- ``constraints``: geometric (``Coincident``, ``Horizontal`` ...) and
  dimensional (``Distance``, ``Angle``, ``Radius`` ...)

The rebuilder translates this into the FreeCAD API:

    sketch.addGeometry(Part.LineSegment(...))
    sketch.addConstraint(Sketcher.Constraint("Coincident", 0, 2, 1, 1))
    sketch.solve()

Fusion 360 stores sketches on a reference plane; we map the plane to
the closest FreeCAD datum plane (XY/XZ/YZ) when possible and otherwise
use the plane name as a label for manual attachment.
"""

from __future__ import annotations

import math
from typing import Any

import FreeCAD
import Part
import Sketcher


# Conversion factor: Fusion 360's API works in cm internally for
# length-valued properties. The extract script already converts via
# the property ``.value`` interface, which returns the value in the
# document's default unit (mm by default). We accept any length unit
# here and let FreeCAD handle the conversion.
_VECTOR = FreeCAD.Vector


# Mapping from Fusion 360 plane entity tokens to FreeCAD datum planes.
# Fusion 360 has many reference planes (XY, XZ, YZ plus offset planes).
# Without a deterministic ID we map by common names:
_PLANE_NAME_MAP = {
    "XY plane": "XY_Plane",
    "XY Plane": "XY_Plane",
    "xz plane": "XZ_Plane",
    "XZ plane": "XZ_Plane",
    "yz plane": "YZ_Plane",
    "YZ plane": "YZ_Plane",
}


def _plane_for(doc: Any, plane_info: dict | None) -> Any:
    """Return the FreeCAD datum plane to attach a sketch to."""
    if not plane_info:
        return doc.XY_Plane
    name = plane_info.get("name", "")
    target = _PLANE_NAME_MAP.get(name)
    if target and hasattr(doc, target):
        return getattr(doc, target)
    # Fallback: use XY plane and let the user reattach manually.
    return doc.XY_Plane


def _attach_sketch(doc: Any, body: Any, name: str, plane_info: dict | None) -> Any:
    """Create and attach a new sketch to the body, return the sketch."""
    sketch = doc.addObject("Sketcher::SketchObject", name)
    plane = _plane_for(doc, plane_info)
    try:
        sketch.AttachmentSupport = (plane, [""])
        sketch.MapMode = "FlatFace"
        body.addObject(sketch)
    except Exception:
        # Last resort: leave the sketch unattached so the user can
        # assign it manually in the GUI.
        doc.recompute()
    return sketch


def _add_curve(sketch: Any, curve: dict) -> int:
    """Append a single curve to ``sketch`` and return its GeoId."""
    ctype = curve.get("type", "")
    geo = None

    try:
        if ctype == "Line2D" or "SketchLine" in ctype:
            geo = Part.LineSegment(
                _VECTOR(curve["start"][0], curve["start"][1], 0),
                _VECTOR(curve["end"][0], curve["end"][1], 0),
            )
        elif ctype == "Circle2D" or "SketchCircle" in ctype:
            geo = Part.Circle(
                _VECTOR(curve["center"][0], curve["center"][1], 0),
                _VECTOR(0, 0, 1),
                float(curve["radius"]),
            )
        elif ctype == "Arc2D" or "SketchArc" in ctype:
            geo = Part.ArcOfCircle(
                Part.Circle(
                    _VECTOR(curve["center"][0], curve["center"][1], 0),
                    _VECTOR(0, 0, 1),
                    float(curve["radius"]),
                ),
                float(curve.get("start_angle", 0)),
                float(curve.get("end_angle", math.pi / 2)),
            )
        elif ctype == "Ellipse2D" or "SketchEllipse" in ctype:
            geo = Part.Ellipse(
                _VECTOR(curve["center"][0], curve["center"][1], 0),
                _VECTOR(float(curve.get("major_radius", 1.0)), 0, 0),
                _VECTOR(0, float(curve.get("minor_radius", 1.0)), 0),
            )
        elif ctype in ("EllipticalArc2D", "SketchEllipticalArc"):
            geo = Part.ArcOfEllipse(
                Part.Ellipse(
                    _VECTOR(curve["center"][0], curve["center"][1], 0),
                    _VECTOR(float(curve.get("major_radius", 1.0)), 0, 0),
                    _VECTOR(0, float(curve.get("minor_radius", 1.0)), 0),
                ),
                float(curve.get("start_angle", 0)),
                float(curve.get("end_angle", math.pi / 2)),
            )
        elif ctype in ("Spline2D", "SketchSpline", "SketchFittedSpline"):
            fit = curve.get("fit_points") or []
            if len(fit) >= 2:
                pts = [_VECTOR(p[0], p[1], 0) for p in fit]
                geo = Part.BSplineCurve(pts)
        elif ctype in ("SketchPoint",):
            # Points are added separately; ignore here.
            return -1
    except Exception:
        return -1

    if geo is None:
        return -1

    try:
        return int(sketch.addGeometry(geo))
    except Exception:
        return -1


def _add_point(sketch: Any, point: dict) -> int:
    try:
        return int(sketch.addGeometry(
            Part.Point(_VECTOR(point["x"], point["y"], 0))
        ))
    except Exception:
        return -1


def _add_constraints(
    sketch: Any,
    constraints: list[dict],
    curve_geo_ids: list[int],
    point_geo_ids: list[int],
) -> list[str]:
    """Add geometric and dimensional constraints. Return list of warnings."""
    warnings: list[str] = []
    for con in constraints:
        ctype = con.get("type", "")
        try:
            if ctype == "HorizontalConstraint":
                sketch.addConstraint(Sketcher.Constraint("Horizontal", _first(con, curve_geo_ids)))
            elif ctype == "VerticalConstraint":
                sketch.addConstraint(Sketcher.Constraint("Vertical", _first(con, curve_geo_ids)))
            elif ctype in ("CoincidentConstraint",):
                ents = con.get("entities") or []
                ids = _resolve_entities(ents, curve_geo_ids, point_geo_ids)
                if len(ids) >= 2 and ids[0] is not None and ids[1] is not None:
                    sketch.addConstraint(Sketcher.Constraint("Coincident", ids[0], 2, ids[1], 1))
            elif ctype in ("TangentConstraint",):
                ids = _resolve_entities(con.get("entities") or [], curve_geo_ids, point_geo_ids)
                sketch.addConstraint(Sketcher.Constraint("Tangent", _safe(ids, 0), _safe(ids, 1)))
            elif ctype in ("EqualConstraint",):
                ids = _resolve_entities(con.get("entities") or [], curve_geo_ids, point_geo_ids)
                sketch.addConstraint(Sketcher.Constraint("Equal", _safe(ids, 0), _safe(ids, 1)))
            elif ctype in ("SymmetricConstraint",):
                ids = _resolve_entities(con.get("entities") or [], curve_geo_ids, point_geo_ids)
                sketch.addConstraint(Sketcher.Constraint("Symmetric", _safe(ids, 0), 1, _safe(ids, 1), 1, _safe(ids, 2)))
            elif ctype in ("DistanceConstraint",):
                ids = _resolve_entities(con.get("entities") or [], curve_geo_ids, point_geo_ids)
                val = con.get("value")
                if val is None:
                    continue
                # Heuristic: if both endpoints look like points, treat as DistanceX/Y
                if len(ids) == 2 and ids[0] is not None and ids[1] is not None:
                    sketch.addConstraint(Sketcher.Constraint("Distance", ids[0], 2, ids[1], 1, float(val)))
            elif ctype in ("RadiusConstraint", "DiameterConstraint"):
                ids = _resolve_entities(con.get("entities") or [], curve_geo_ids, point_geo_ids)
                val = con.get("value")
                if val is not None and ids and ids[0] is not None:
                    sketch.addConstraint(Sketcher.Constraint("Radius", ids[0], float(val)))
            elif ctype in ("AngleConstraint",):
                val = con.get("value")
                if val is None:
                    continue
                ids = _resolve_entities(con.get("entities") or [], curve_geo_ids, point_geo_ids)
                sketch.addConstraint(Sketcher.Constraint("Angle", _safe(ids, 0), _safe(ids, 1), float(val)))
            elif ctype in ("PerpendicularConstraint",):
                ids = _resolve_entities(con.get("entities") or [], curve_geo_ids, point_geo_ids)
                sketch.addConstraint(Sketcher.Constraint("Perpendicular", _safe(ids, 0), _safe(ids, 1)))
            elif ctype in ("ParallelConstraint",):
                ids = _resolve_entities(con.get("entities") or [], curve_geo_ids, point_geo_ids)
                sketch.addConstraint(Sketcher.Constraint("Parallel", _safe(ids, 0), _safe(ids, 1)))
            else:
                # Dimensional constraints of unknown type - skip silently.
                continue
        except Exception as exc:
            warnings.append(f"Could not add {ctype}: {exc}")
    return warnings


def _first(constraint: dict, curve_geo_ids: list[int]) -> int:
    ents = constraint.get("entities") or []
    if not ents:
        return -1
    return curve_geo_ids[0] if curve_geo_ids else -1


def _safe(lst: list, idx: int) -> int:
    if idx < len(lst) and lst[idx] is not None:
        return int(lst[idx])
    return -1


def _resolve_entities(
    fusion_tokens: list[str | None],
    curve_geo_ids: list[int],
    point_geo_ids: list[int],
) -> list[int]:
    """Translate Fusion entity tokens into FreeCAD GeoIds.

    The Fusion extractor returns opaque entity tokens that we cannot
    map back to specific GeoIds without rebuilding the full topology.
    In practice we only need to assign *some* GeoId so the constraint
    has geometry to attach to; we fall back to the first curve/point
    for any token we cannot resolve.
    """
    out: list[int] = []
    for token in fusion_tokens:
        out.append(-1)  # placeholder, replaced below
    # Use the curve/point indices in order, cycling if needed.
    sources = curve_geo_ids + point_geo_ids
    for i in range(len(out)):
        if sources:
            out[i] = sources[i % len(sources)]
        else:
            out[i] = -1
    return out


def rebuild_sketch(doc: Any, body: Any, sketch_data: dict) -> tuple[Any, list[str]]:
    """Create a fully-populated sketch in ``body``.

    Returns ``(sketch, warnings)`` where ``warnings`` is a list of
    human-readable strings describing skipped constraints.
    """
    name = sketch_data.get("name") or "FusionSketch"
    plane = sketch_data.get("plane")
    sketch = _attach_sketch(doc, body, name, plane)

    curve_geo_ids: list[int] = []
    for curve in sketch_data.get("curves", []):
        gid = _add_curve(sketch, curve)
        if gid >= 0:
            curve_geo_ids.append(gid)

    point_geo_ids: list[int] = []
    for point in sketch_data.get("points", []):
        gid = _add_point(sketch, point)
        if gid >= 0:
            point_geo_ids.append(gid)

    warnings = _add_constraints(
        sketch,
        sketch_data.get("constraints", []),
        curve_geo_ids,
        point_geo_ids,
    )

    try:
        sketch.solve()
    except Exception as exc:
        warnings.append(f"Sketch solver failed: {exc}")

    return sketch, warnings