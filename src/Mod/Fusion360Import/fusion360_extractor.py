# SPDX-License-Identifier: LGPL-2.1-or-later
"""Python script executed *inside* Fusion 360 to extract design history.

This string is sent via ``client.execute_script()`` and runs in Fusion
360's main thread with full access to ``adsk.core`` and ``adsk.fusion``.
It must be self-contained (no external imports beyond Fusion's own
modules), and it must emit a single JSON object to stdout that the
client then parses.

The output schema is documented at the bottom of this module as
``HISTORY_SCHEMA`` and is consumed by ``feature_rebuilder.py``.
"""

# We deliberately keep this as a module-level string so the
# fusion360_extractor module can re-export it without parsing.
# Splitting the script into multiple lines keeps the line-count
# manageable and avoids exotic escape gymnastics.

EXTRACT_HISTORY_SCRIPT = r'''
import adsk.core
import adsk.fusion
import json
import math
import os
import tempfile
import traceback


def _safe(value):
    """Convert a Fusion API value into a JSON-serialisable form."""
    if value is None:
        return None
    if isinstance(value, bool):
        return value
    if isinstance(value, (int, float)):
        if isinstance(value, float) and (math.isnan(value) or math.isinf(value)):
            return None
        return value
    if isinstance(value, str):
        return value
    if isinstance(value, (list, tuple)):
        return [_safe(v) for v in value]
    if isinstance(value, dict):
        return {str(k): _safe(v) for k, v in value.items()}
    return str(value)


def _value_or_none(prop):
    """Return a property's .value if it exists, else None."""
    try:
        return _safe(prop.value)
    except Exception:
        return None


def _expression_or_none(prop):
    """Return a property's .expression string if it exists."""
    try:
        expr = prop.expression
        return expr if expr else None
    except Exception:
        return None


def _unit_string(prop):
    """Return a unit string such as 'mm' or 'deg' if available."""
    try:
        unit = prop.unit
        return unit if unit else ""
    except Exception:
        return ""


def _extract_parameters(design):
    """Collect user and model parameters with their expressions."""
    out = {"user": [], "model": []}
    try:
        for p in design.userParameters:
            out["user"].append({
                "name": p.name,
                "value": _value_or_none(p),
                "expression": _expression_or_none(p),
                "unit": _unit_string(p),
                "comment": _safe(getattr(p, "comment", "")),
            })
    except Exception:
        pass
    try:
        for p in design.allParameters:
            out["model"].append({
                "name": p.name,
                "value": _value_or_none(p),
                "expression": _expression_or_none(p),
                "unit": _unit_string(p),
                "comment": _safe(getattr(p, "comment", "")),
            })
    except Exception:
        pass
    return out


def _extract_sketch(sketch):
    """Return a dict describing every curve and constraint in sketch."""
    if sketch is None:
        return None

    def _vec(v):
        try:
            return [_safe(v.x), _safe(v.y), _safe(v.z)]
        except Exception:
            return [_safe(v.x), _safe(v.y)]

    curves = []
    try:
        for i in range(sketch.sketchCurves.count):
            c = sketch.sketchCurves.item(i)
            entry = {"index": i, "type": type(c).__name__}
            try:
                if c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Line2D:
                    entry["start"] = _vec(c.startSketchPoint.geometry)
                    entry["end"] = _vec(c.endSketchPoint.geometry)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Circle2D:
                    entry["center"] = _vec(c.centerSketchPoint.geometry)
                    entry["radius"] = _safe(c.radius)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Arc2D:
                    entry["center"] = _vec(c.centerSketchPoint.geometry)
                    entry["radius"] = _safe(c.radius)
                    entry["start_angle"] = _safe(c.startAngle)
                    entry["end_angle"] = _safe(c.endAngle)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Ellipse2D:
                    entry["center"] = _vec(c.centerSketchPoint.geometry)
                    entry["major_radius"] = _safe(c.majorRadius)
                    entry["minor_radius"] = _safe(c.minorRadius)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.EllipticalArc2D:
                    entry["center"] = _vec(c.centerSketchPoint.geometry)
                    entry["major_radius"] = _safe(c.majorRadius)
                    entry["minor_radius"] = _safe(c.minorRadius)
                    entry["start_angle"] = _safe(c.startAngle)
                    entry["end_angle"] = _safe(c.endAngle)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Spline2D:
                    try:
                        pts = []
                        for p in c.fitPoints:
                            pts.append(_vec(p))
                        entry["fit_points"] = pts
                    except Exception:
                        pass
            except Exception as exc:
                entry["error"] = str(exc)
            curves.append(entry)
    except Exception as exc:
        curves.append({"error": "sketch_curves_failed: " + str(exc)})

    points = []
    try:
        for i in range(sketch.sketchPoints.count):
            p = sketch.sketchPoints.item(i)
            points.append({
                "index": i,
                "x": _safe(p.geometry.x),
                "y": _safe(p.geometry.y),
            })
    except Exception:
        pass

    constraints = []
    try:
        for i in range(sketch.geometricConstraints.count):
            gc = sketch.geometricConstraints.item(i)
            entry = {
                "index": i,
                "type": type(gc).__name__,
                "is_2d": True,
            }
            try:
                ents = []
                for k in range(gc.entityCount):
                    try:
                        ent = gc.entity(k)
                        ents.append(_safe(ent.entityToken))
                    except Exception:
                        ents.append(None)
                entry["entities"] = ents
            except Exception:
                pass
            constraints.append(entry)
        for i in range(sketch.dimensionalConstraints.count):
            dc = sketch.dimensionalConstraints.item(i)
            entry = {
                "index": i,
                "type": type(dc).__name__,
                "value": _value_or_none(dc.parameter),
                "expression": _expression_or_none(dc.parameter),
                "unit": _unit_string(dc.parameter),
            }
            try:
                ents = []
                for k in range(dc.entityCount):
                    try:
                        ent = dc.entity(k)
                        ents.append(_safe(ent.entityToken))
                    except Exception:
                        ents.append(None)
                entry["entities"] = ents
            except Exception:
                pass
            constraints.append(entry)
    except Exception as exc:
        constraints.append({"error": "constraints_failed: " + str(exc)})

    return {
        "name": _safe(sketch.name),
        "curves": curves,
        "points": points,
        "constraints": constraints,
        "is_visible": _safe(sketch.isVisible),
    }


def _extract_profile_shape(profile):
    """Capture the area and bounding box of a profile."""
    try:
        loops = []
        for i in range(profile.profileLoops.count):
            loop = profile.profileLoops.item(i)
            loops.append({"index": i, "is_outer": _safe(loop.isOuter)})
        return {"loop_count": _safe(profile.profileLoops.count), "loops": loops}
    except Exception:
        return {"error": "profile_unavailable"}


def _entity_token(entity):
    try:
        return entity.entityToken
    except Exception:
        return None


def _classify_feature(entity):
    """Return (kind, params) where kind is a stable string we map later."""
    cls = type(entity).__name__
    params = {}

    def _maybe(prop_name):
        if hasattr(entity, prop_name):
            return _value_or_none(getattr(entity, prop_name))
        return None

    def _expr(prop_name):
        if hasattr(entity, prop_name):
            return _expression_or_none(getattr(entity, prop_name))
        return None

    # ExtrudeFeature (Pad/Pocket)
    if cls == "ExtrudeFeature":
        op = _maybe("operation")
        params.update({
            "distance": _maybe("extentOne"),
            "distance_two": _maybe("extentTwo"),
            "operation": _maybe("operation"),
            "is_symmetric": _maybe("isSymmetric"),
            "taper_angle_one": _maybe("taperAngleOne"),
            "taper_angle_two": _maybe("taperAngleTwo"),
            "direction_flipped": _maybe("isDirectionFlipped"),
            "distance_expression": _expr("extentOne"),
            "distance_two_expression": _expr("extentTwo"),
        })
        kind = "extrude"

    # RevolveFeature
    elif cls == "RevolveFeature":
        params.update({
            "angle": _maybe("angle"),
            "angle_expression": _expr("angle"),
            "is_full": _maybe("isFullAngle"),
            "operation": _maybe("operation"),
        })
        kind = "revolve"

    # FilletFeature
    elif cls == "FilletFeature":
        params.update({
            "radius": _maybe("radius"),
            "radius_expression": _expr("radius"),
        })
        kind = "fillet"

    # ChamferFeature
    elif cls == "ChamferFeature":
        params.update({
            "distance": _maybe("distance"),
            "distance_two": _maybe("distanceTwo"),
            "distance_expression": _expr("distance"),
        })
        kind = "chamfer"

    # HoleFeature
    elif cls == "HoleFeature":
        params.update({
            "hole_diameter": _maybe("holeDiameter"),
            "hole_diameter_expression": _expr("holeDiameter"),
            "tip_angle": _maybe("tipAngle"),
            "is_cosmetic": _maybe("isCosmeticThread"),
        })
        try:
            params["thread_type"] = _safe(entity.threadInfo.threadType)
            params["thread_size"] = _safe(entity.threadInfo.threadSize)
            params["thread_designation"] = _safe(entity.threadInfo.designation)
        except Exception:
            pass
        kind = "hole"

    # RectangularPatternFeature
    elif cls == "RectangularPatternFeature":
        params.update({
            "quantity_one": _maybe("quantityOne"),
            "quantity_two": _maybe("quantityTwo"),
            "distance_one": _maybe("distanceOne"),
            "distance_two": _maybe("distanceTwo"),
            "direction_one_entity": _entity_token(_maybe("directionOneEntity")),
            "direction_two_entity": _entity_token(_maybe("directionTwoEntity")),
        })
        kind = "rect_pattern"

    # CircularPatternFeature
    elif cls == "CircularPatternFeature":
        params.update({
            "quantity": _maybe("quantity"),
            "total_angle": _maybe("totalAngle"),
            "axis_entity": _entity_token(_maybe("axis")),
        })
        kind = "circ_pattern"

    # MirrorFeature
    elif cls == "MirrorFeature":
        try:
            params["mirror_plane_entity"] = _entity_token(entity.mirrorPlane)
        except Exception:
            pass
        kind = "mirror"

    # ShellFeature
    elif cls == "ShellFeature":
        params.update({
            "inside_thickness": _maybe("insideThickness"),
            "outside_thickness": _maybe("outsideThickness"),
        })
        kind = "shell"

    # DraftFeature
    elif cls == "DraftFeature":
        params.update({
            "draft_angle": _maybe("draftAngle"),
        })
        kind = "draft"

    # OffsetFeature
    elif cls == "OffsetFacesFeature":
        params.update({
            "offset_distance": _maybe("offsetDistance"),
        })
        kind = "offset"

    # TrimFeature / SplitBody / Stitch etc.
    elif cls == "TrimFeature":
        kind = "trim"
    elif cls == "SplitBodyFeature":
        kind = "split_body"
    elif cls in ("BooleanFeature", "CombineFeature"):
        params.update({
            "operation": _maybe("operation"),
            "target_bodies_count": _maybe("targetBodies_count"),
            "tool_bodies_count": _maybe("toolBodies_count"),
        })
        kind = "boolean"
    elif cls == "LoftFeature":
        kind = "loft"
    elif cls == "SweepFeature":
        kind = "sweep"
    elif cls == "CoilFeature":
        kind = "coil"
    elif cls in ("BoxFeature", "AdditiveBoxFeature"):
        params.update({
            "length": _maybe("length"),
            "width": _maybe("width"),
            "height": _maybe("height"),
        })
        kind = "box"
    elif cls in ("CylinderFeature", "AdditiveCylinderFeature"):
        params.update({
            "radius": _maybe("radius"),
            "height": _maybe("height"),
        })
        kind = "cylinder"
    elif cls in ("SphereFeature",):
        params.update({
            "radius": _maybe("radius"),
        })
        kind = "sphere"
    elif cls in ("TorusFeature",):
        params.update({
            "outer_radius": _maybe("outerRadius"),
            "inner_radius": _maybe("innerRadius"),
        })
        kind = "torus"
    elif cls == "PipeFeature":
        kind = "pipe"
    elif cls == "RibFeature":
        kind = "rib"
    elif cls == "WebFeature":
        kind = "web"
    elif cls == "HoleFeature":
        kind = "hole"
    else:
        kind = cls.lower()

    return kind, params


def _extract_timeline(design):
    """Walk the entire timeline and produce a serialisable list."""
    out = []
    timeline = design.timeline
    for i in range(timeline.count):
        item = timeline.item(i)
        if item is None:
            continue
        entity = item.entity
        entry = {
            "index": i,
            "name": _safe(item.name),
            "is_creation": _safe(getattr(item, "isCreation", None)),
            "is_features": _safe(getattr(item, "isFeatures", None)),
            "is_group": _safe(getattr(item, "isGroup", None)),
            "is_suppressed": _safe(getattr(item, "isSuppressed", None)),
            "entity_type": type(entity).__name__ if entity else None,
            "entity_token": _entity_token(entity),
        }

        # Sketch only - capture geometry/constraints.
        if isinstance(entity, adsk.fusion.Sketch):
            entry["feature_kind"] = "sketch"
            entry["sketch"] = _extract_sketch(entity)
            try:
                plane = entity.referencePlane
                if plane:
                    entry["plane"] = {
                        "name": _safe(plane.name),
                        "entity_token": _entity_token(plane),
                    }
            except Exception:
                pass
            try:
                entry["profiles"] = []
                for pi in range(entity.profiles.count):
                    entry["profiles"].append(
                        _extract_profile_shape(entity.profiles.item(pi))
                    )
            except Exception:
                pass

        # Construction geometry
        elif isinstance(entity, adsk.fusion.ConstructionPlane):
            entry["feature_kind"] = "construction_plane"
        elif isinstance(entity, adsk.fusion.ConstructionAxis):
            entry["feature_kind"] = "construction_axis"
        elif isinstance(entity, adsk.fusion.ConstructionPoint):
            entry["feature_kind"] = "construction_point"

        # Components (occurrences in an assembly)
        elif isinstance(entity, adsk.fusion.Occurrence):
            entry["feature_kind"] = "occurrence"
            try:
                entry["occurrence"] = {
                    "component_name": _safe(entity.component.name),
                    "is_light_bike": _safe(entity.isLightBullet),
                }
            except Exception:
                pass

        # Everything else is treated as a parametric feature.
        else:
            kind, params = _classify_feature(entity)
            entry["feature_kind"] = kind
            entry["params"] = params

            # Try to attach the sketch profile reference.
            try:
                if hasattr(entity, "profile") and entity.profile is not None:
                    entry["profile_token"] = _entity_token(entity.profile)
                if hasattr(entity, "sketch") and entity.sketch is not None:
                    entry["sketch_token"] = _entity_token(entity.sketch)
            except Exception:
                pass

        out.append(entry)
    return out


def _extract_components(root_component):
    """Return a tree of components in the design."""
    out = []

    def walk(comp, depth):
        entry = {
            "name": _safe(comp.name),
            "depth": depth,
            "bodies": _safe(comp.bodies.count),
            "sketches": _safe(comp.sketches.count),
            "features": _safe(comp.features.count),
            "origin": {
                "x": _safe(comp.originGeometry.origin.x),
                "y": _safe(comp.originGeometry.origin.y),
                "z": _safe(comp.originGeometry.origin.z),
            },
            "children": [],
        }
        for occ in comp.occurrences:
            walk(occ.component, depth + 1)
            entry["children"].append({
                "name": _safe(occ.component.name),
                "transform": _safe([
                    _safe(occ.transform.getCell(0, 0)),
                    _safe(occ.transform.getCell(0, 1)),
                    _safe(occ.transform.getCell(0, 2)),
                    _safe(occ.transform.getCell(0, 3)),
                    _safe(occ.transform.getCell(1, 0)),
                    _safe(occ.transform.getCell(1, 1)),
                    _safe(occ.transform.getCell(1, 2)),
                    _safe(occ.transform.getCell(1, 3)),
                    _safe(occ.transform.getCell(2, 0)),
                    _safe(occ.transform.getCell(2, 1)),
                    _safe(occ.transform.getCell(2, 2)),
                    _safe(occ.transform.getCell(2, 3)),
                ]),
            })
        return entry

    return walk(root_component, 0)


def _export_step(design, path):
    export_mgr = design.exportManager
    options = export_mgr.createSTEPExportOptions(path)
    export_mgr.execute(options)
    return os.path.exists(path)


def main():
    app = adsk.core.Application.get()
    if app is None:
        print(json.dumps({"error": "fusion_not_running"}))
        return

    try:
        doc = app.activeDocument
        if doc is None:
            print(json.dumps({"error": "no_active_document"}))
            return

        product = doc.products[0] if doc.products.count > 0 else None
        design = adsk.fusion.Design.cast(product)
        if design is None:
            print(json.dumps({"error": "active_document_is_not_a_design"}))
            return

        # Export STEP into the system temp directory; the client will move it.
        step_path = os.path.join(tempfile.gettempdir(), "fusion_export.step")
        step_exported = False
        try:
            step_exported = _export_step(design, step_path)
        except Exception as exc:
            step_exported = False
            step_error = str(exc)
        else:
            step_error = None

        result = {
            "schema": 1,
            "document": _safe(doc.name),
            "design_name": _safe(design.name),
            "design_type": (
                "parametric" if design.designType == adsk.fusion.DesignTypes.ParametricDesignType
                else "direct" if design.designType == adsk.fusion.DesignTypes.DirectDesignType
                else "unknown"
            ),
            "units": _safe(design.unitsManager.defaultLengthUnits),
            "root_component": _extract_components(design.rootComponent),
            "parameters": _extract_parameters(design),
            "timeline": _extract_timeline(design),
            "step_export": {
                "path": step_path if step_exported else None,
                "size": os.path.getsize(step_path) if step_exported else 0,
                "error": step_error,
            },
        }
        print("__FUSION_EXTRACT_BEGIN__")
        print(json.dumps(result))
        print("__FUSION_EXTRACT_END__")
    except Exception as exc:
        print(json.dumps({
            "error": "extraction_failed",
            "message": str(exc),
            "traceback": traceback.format_exc(),
        }))


main()
'''


# Documented schema for the JSON output produced by EXTRACT_HISTORY_SCRIPT.
# The feature rebuilder relies on this shape.
HISTORY_SCHEMA = {
    "schema": 1,
    "document": "name of the source document",
    "design_name": "name of the design",
    "design_type": "parametric | direct | unknown",
    "units": "default length unit string, e.g. 'mm' or 'in'",
    "root_component": {
        "name": "component name",
        "bodies": "count of BRepBody objects",
        "features": "count of parametric features",
        "children": [
            {
                "name": "occurrence name",
                "transform": "16-float matrix3d as flat list",
            }
        ],
    },
    "parameters": {
        "user": [{"name", "value", "expression", "unit", "comment"}],
        "model": [{"name", "value", "expression", "unit", "comment"}],
    },
    "timeline": [
        # Each entry is one of:
        # {"feature_kind": "sketch", "sketch": {...}}
        # {"feature_kind": "construction_plane"}
        # {"feature_kind": "occurrence", "occurrence": {...}}
        # {"feature_kind": "<feature_type>", "params": {...}}
        # The list is in chronological order.
    ],
    "step_export": {
        "path": "absolute filesystem path of the exported STEP file",
        "size": "byte size",
        "error": "None on success, error string on failure",
    },
}