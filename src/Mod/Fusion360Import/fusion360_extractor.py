# SPDX-License-Identifier: LGPL-2.1-or-later
"""Python script executed *inside* Fusion 360 to extract design history.

This string is sent via ``client.execute_script()`` and runs in Fusion
360's main thread with full access to ``adsk.core`` and ``adsk.fusion``.
It must be self-contained (no external imports beyond Fusion's own
modules), and it must emit a single JSON object to stdout that the
client then parses.

The output schema is documented at the bottom of this module as
``HISTORY_SCHEMA`` and is consumed by :mod:`feature_rebuilder`.

The extraction strategy is intentionally comprehensive: every
parametric feature, every sketch, every occurrence, every construction
plane/axis/point, every appearance and material reference, and every
user/model parameter is captured. We even serialise body/face/edge
entity tokens so the rebuilder can map references back to Tungsten CAD
sub-element names (``Face1``, ``Edge3`` ...).
"""

# The script is split across several helper functions to keep it
# readable while still being a single string.

EXTRACT_HISTORY_SCRIPT = r'''
import adsk.core
import adsk.fusion
import json
import math
import os
import tempfile
import traceback


# ---------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------

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


def _vec(v):
    """Convert a Fusion Vector3D into a flat list."""
    try:
        return [_safe(v.x), _safe(v.y), _safe(v.z)]
    except Exception:
        try:
            return [_safe(v.x), _safe(v.y)]
        except Exception:
            return None


def _value(prop):
    try:
        return _safe(prop.value)
    except Exception:
        return None


def _expression(prop):
    try:
        expr = prop.expression
        return expr if expr else None
    except Exception:
        return None


def _unit(prop):
    try:
        unit = prop.unit
        return unit if unit else ""
    except Exception:
        return ""


def _token(entity):
    """Return the entity's token (UUID-like string used for refs)."""
    try:
        if entity is None:
            return None
        return entity.entityToken
    except Exception:
        return None


# ---------------------------------------------------------------------
# Sketches
# ---------------------------------------------------------------------

def _extract_sketch(sketch):
    if sketch is None:
        return None

    curves = []
    try:
        for i in range(sketch.sketchCurves.count):
            c = sketch.sketchCurves.item(i)
            entry = {"index": i, "type": type(c).__name__, "token": _token(c)}
            try:
                if c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Line2D:
                    entry["start"] = _vec(c.startSketchPoint.geometry)
                    entry["end"] = _vec(c.endSketchPoint.geometry)
                    entry["start_token"] = _token(c.startSketchPoint)
                    entry["end_token"] = _token(c.endSketchPoint)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Circle2D:
                    entry["center"] = _vec(c.centerSketchPoint.geometry)
                    entry["center_token"] = _token(c.centerSketchPoint)
                    entry["radius"] = _value(c.radius)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Arc2D:
                    entry["center"] = _vec(c.centerSketchPoint.geometry)
                    entry["center_token"] = _token(c.centerSketchPoint)
                    entry["radius"] = _value(c.radius)
                    entry["start_angle"] = _value(c.startAngle)
                    entry["end_angle"] = _value(c.endAngle)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Ellipse2D:
                    entry["center"] = _vec(c.centerSketchPoint.geometry)
                    entry["center_token"] = _token(c.centerSketchPoint)
                    entry["major_radius"] = _value(c.majorRadius)
                    entry["minor_radius"] = _value(c.minorRadius)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.EllipticalArc2D:
                    entry["center"] = _vec(c.centerSketchPoint.geometry)
                    entry["center_token"] = _token(c.centerSketchPoint)
                    entry["major_radius"] = _value(c.majorRadius)
                    entry["minor_radius"] = _value(c.minorRadius)
                    entry["start_angle"] = _value(c.startAngle)
                    entry["end_angle"] = _value(c.endAngle)
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Spline2D:
                    pts = []
                    pt_tokens = []
                    for p in c.fitPoints:
                        pts.append(_vec(p))
                    try:
                        for p in c.controlPoints:
                            pt_tokens.append(_token(p))
                    except Exception:
                        pass
                    entry["fit_points"] = pts
                    entry["fit_point_tokens"] = pt_tokens
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.Spline3D:
                    pts = []
                    for p in c.fitPoints:
                        pts.append(_vec(p))
                    entry["fit_points"] = pts
                elif c.objectType == adsk.fusion.SketchCurve.ObjectType2D.FittedSpline2D:
                    pts = []
                    try:
                        for p in c.fitPoints:
                            pts.append(_vec(p))
                    except Exception:
                        pass
                    entry["fit_points"] = pts
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
                "x": _value(p.geometry.x),
                "y": _value(p.geometry.y),
                "token": _token(p),
            })
    except Exception:
        pass

    # Build a geometry-index -> token map for translating constraints.
    geometry_token_map = {i: c.get("token") for i, c in enumerate(curves) if c.get("token")}
    point_token_map = {i: p.get("token") for i, p in enumerate(points) if p.get("token")}

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
                indices = []
                for k in range(gc.entityCount):
                    try:
                        ent = gc.entity(k)
                        ents.append(_safe(ent.entityToken))
                        # Geometry index within the sketch
                        try:
                            indices.append(_find_sketch_geo_index(sketch, ent))
                        except Exception:
                            indices.append(None)
                    except Exception:
                        ents.append(None)
                        indices.append(None)
                entry["entities"] = ents
                entry["entity_indices"] = indices
            except Exception:
                pass
            constraints.append(entry)
        for i in range(sketch.dimensionalConstraints.count):
            dc = sketch.dimensionalConstraints.item(i)
            entry = {
                "index": i,
                "type": type(dc).__name__,
                "value": _value(dc.parameter),
                "expression": _expression(dc.parameter),
                "unit": _unit(dc.parameter),
            }
            try:
                ents = []
                indices = []
                for k in range(dc.entityCount):
                    try:
                        ent = dc.entity(k)
                        ents.append(_safe(ent.entityToken))
                        try:
                            indices.append(_find_sketch_geo_index(sketch, ent))
                        except Exception:
                            indices.append(None)
                    except Exception:
                        ents.append(None)
                        indices.append(None)
                entry["entities"] = ents
                entry["entity_indices"] = indices
            except Exception:
                pass
            constraints.append(entry)
    except Exception as exc:
        constraints.append({"error": "constraints_failed: " + str(exc)})

    profile_info = []
    try:
        for pi in range(sketch.profiles.count):
            prof = sketch.profiles.item(pi)
            profile_info.append({
                "index": pi,
                "is_outer": _safe(prof.profileLoops.item(0).isOuter)
                             if prof.profileLoops.count > 0 else None,
                "area": _safe(prof.areaProperties().area),
                "token": _token(prof),
                "loops": _safe(prof.profileLoops.count),
            })
    except Exception:
        pass

    # Plane the sketch lives on.
    plane_info = None
    try:
        plane = sketch.referencePlane
        if plane:
            plane_info = {
                "name": _safe(plane.name),
                "token": _token(plane),
                "entity_type": type(plane).__name__,
            }
            try:
                plane_info["origin"] = _vec(plane.origin)
                plane_info["normal"] = _vec(plane.normal)
                plane_info["u_axis"] = _vec(plane.uDirection)
                plane_info["v_axis"] = _vec(plane.vDirection)
            except Exception:
                pass
    except Exception:
        pass

    return {
        "name": _safe(sketch.name),
        "token": _token(sketch),
        "curves": curves,
        "points": points,
        "constraints": constraints,
        "profiles": profile_info,
        "is_visible": _safe(sketch.isVisible),
        "plane": plane_info,
        "geometry_token_map": geometry_token_map,
        "point_token_map": point_token_map,
    }


def _find_sketch_geo_index(sketch, entity):
    """Return the geometry index of an entity within a sketch."""
    try:
        for i in range(sketch.sketchCurves.count):
            if sketch.sketchCurves.item(i).entityToken == entity.entityToken:
                return i
        for i in range(sketch.sketchPoints.count):
            if sketch.sketchPoints.item(i).entityToken == entity.entityToken:
                return i
    except Exception:
        pass
    return None


# ---------------------------------------------------------------------
# Bodies
# ---------------------------------------------------------------------

def _extract_body(body):
    """Serialise every BRepFace, BRepEdge and BRepVertex of a body.

    We persist the entity *token* (a Fusion UUID) plus a stable index
    that can be used by the rebuilder to construct sub-element names
    like ``Face1`` / ``Edge3``. Body name + token are required so the
    rebuilder can look the body up after reconstruction.
    """
    if body is None:
        return None
    faces = []
    edges = []
    vertices = []
    try:
        for i in range(body.faces.count):
            f = body.faces.item(i)
            faces.append({
                "index": i,
                "token": _token(f),
                "geometry_type": _safe(f.geometry.objectType)
                                  if hasattr(f, "geometry") and f.geometry else None,
                "area": _safe(f.area),
            })
    except Exception:
        pass
    try:
        for i in range(body.edges.count):
            e = body.edges.item(i)
            edges.append({
                "index": i,
                "token": _token(e),
                "curve_type": _safe(e.geometry.objectType)
                                if hasattr(e, "geometry") and e.geometry else None,
                "length": _safe(e.length),
            })
    except Exception:
        pass
    try:
        for i in range(body.vertices.count):
            v = body.vertices.item(i)
            vertices.append({
                "index": i,
                "token": _token(v),
                "point": _vec(v.geometry) if v.geometry else None,
            })
    except Exception:
        pass
    appearance = None
    try:
        ap = body.appearance
        if ap:
            appearance = {
                "name": _safe(ap.name),
                "id": _safe(getattr(ap, "id", None)),
            }
    except Exception:
        pass
    material = None
    try:
        m = body.material
        if m:
            material = {"name": _safe(m.name), "id": _safe(getattr(m, "id", None))}
    except Exception:
        pass
    return {
        "name": _safe(body.name),
        "token": _token(body),
        "is_visible": _safe(body.isVisible),
        "faces": faces,
        "edges": edges,
        "vertices": vertices,
        "appearance": appearance,
        "material": material,
        "volume": _safe(body.volume),
        "bbox_min": _vec(body.boundingBox.minPoint) if body.boundingBox else None,
        "bbox_max": _vec(body.boundingBox.maxPoint) if body.boundingBox else None,
    }


# ---------------------------------------------------------------------
# Features
# ---------------------------------------------------------------------

def _extract_param_value(prop):
    return {"value": _value(prop), "expression": _expression(prop), "unit": _unit(prop)}


def _classify_feature(entity):
    """Return (kind, params_dict). 'kind' is a stable string we map later."""
    cls = type(entity).__name__
    p = {}

    def _maybe(attr, transform=None):
        if not hasattr(entity, attr):
            return None
        try:
            v = getattr(entity, attr)
        except Exception:
            return None
        if v is None:
            return None
        if transform:
            try:
                return transform(v)
            except Exception:
                return None
        return _safe(v)

    def _expr(attr):
        if not hasattr(entity, attr):
            return None
        try:
            return _expression(getattr(entity, attr))
        except Exception:
            return None

    # ------ Sketch-based primitives (extrude / revolve / etc.) ------
    if cls == "ExtrudeFeature":
        op = _maybe("operation")
        p["operation"] = op
        p["operation_string"] = {
            adsk.fusion.FeatureOperations.JoinFeatureOperation: "Join",
            adsk.fusion.FeatureOperations.CutFeatureOperation: "Cut",
            adsk.fusion.FeatureOperations.IntersectFeatureOperation: "Intersect",
        }.get(op, None)
        p["extent_one"] = _extract_param_value(entity.extentOne)
        p["extent_two"] = _extract_param_value(entity.extentTwo)
        p["is_symmetric"] = _maybe("isSymmetric")
        p["taper_angle_one"] = _extract_param_value(entity.taperAngleOne)
        p["taper_angle_two"] = _extract_param_value(entity.taperAngleTwo)
        p["start_extent"] = _extract_param_value(entity.startExtent)
        try:
            p["profile_token"] = _token(entity.profile)
        except Exception:
            pass
        try:
            p["sketch_token"] = _token(entity.sketch)
        except Exception:
            pass
        kind = "extrude"

    elif cls == "RevolveFeature":
        op = _maybe("operation")
        p["operation"] = op
        p["operation_string"] = {
            adsk.fusion.FeatureOperations.JoinFeatureOperation: "Join",
            adsk.fusion.FeatureOperations.CutFeatureOperation: "Cut",
            adsk.fusion.FeatureOperations.IntersectFeatureOperation: "Intersect",
        }.get(op, None)
        p["angle"] = _extract_param_value(entity.angle)
        p["is_full_angle"] = _maybe("isFullAngle")
        try:
            p["profile_token"] = _token(entity.profile)
        except Exception:
            pass
        try:
            p["sketch_token"] = _token(entity.sketch)
        except Exception:
            pass
        try:
            axis = entity.axis
            p["axis_token"] = _token(axis)
        except Exception:
            pass
        kind = "revolve"

    elif cls == "LoftFeature":
        op = _maybe("operation")
        p["operation"] = op
        p["operation_string"] = {
            adsk.fusion.FeatureOperations.JoinFeatureOperation: "Join",
            adsk.fusion.FeatureOperations.CutFeatureOperation: "Cut",
        }.get(op, "Join")
        p["is_closed"] = _maybe("isClosed")
        p["is_ruled"] = _maybe("isRuled")
        sections = []
        try:
            for i in range(entity.loftSections.count):
                sec = entity.loftSections.item(i)
                sections.append({
                    "index": i,
                    "token": _token(sec),
                    "entity_type": type(sec).__name__,
                    "profile_token": _token(sec.profile) if hasattr(sec, "profile") else None,
                    "sketch_token": _token(sec.sketch) if hasattr(sec, "sketch") else None,
                })
        except Exception:
            pass
        p["sections"] = sections
        try:
            p["profile_token"] = _token(entity.profile)
        except Exception:
            pass
        try:
            p["sketch_token"] = _token(entity.sketch)
        except Exception:
            pass
        kind = "loft"

    elif cls == "SweepFeature":
        op = _maybe("operation")
        p["operation"] = op
        p["operation_string"] = {
            adsk.fusion.FeatureOperations.JoinFeatureOperation: "Join",
            adsk.fusion.FeatureOperations.CutFeatureOperation: "Cut",
        }.get(op, "Join")
        try:
            p["profile_token"] = _token(entity.profile)
            p["profile_entity_type"] = type(entity.profile).__name__
        except Exception:
            pass
        try:
            p["sketch_token"] = _token(entity.sketch)
        except Exception:
            pass
        # Path: try .path then .spine
        path_token = None
        try:
            path_token = _token(entity.path)
        except Exception:
            try:
                path_token = _token(entity.spine)
            except Exception:
                pass
        p["path_token"] = path_token
        kind = "sweep"

    elif cls == "CoilFeature":
        op = _maybe("operation")
        p["operation"] = op
        p["operation_string"] = {
            adsk.fusion.FeatureOperations.JoinFeatureOperation: "Join",
            adsk.fusion.FeatureOperations.CutFeatureOperation: "Cut",
        }.get(op, "Join")
        p["is_revolution"] = _maybe("isRevolution")
        p["is_pitch_and_revolution"] = _maybe("isPitchAndRevolution")
        p["is_height_and_revolution"] = _maybe("isHeightAndRevolution")
        p["height"] = _extract_param_value(entity.height)
        p["pitch"] = _extract_param_value(entity.pitch)
        p["revolutions"] = _extract_param_value(entity.revolutions)
        p["angle"] = _extract_param_value(entity.angle)
        p["section_size"] = _extract_param_value(entity.sectionSize)
        p["is_internal_section"] = _maybe("isInternalSection")
        p["is_flattened_at_end"] = _maybe("isFlattenedAtEnd")
        try:
            axis = entity.axis
            p["axis_token"] = _token(axis)
        except Exception:
            pass
        try:
            p["profile_token"] = _token(entity.profile)
        except Exception:
            pass
        try:
            p["sketch_token"] = _token(entity.sketch)
        except Exception:
            pass
        kind = "coil"

    # ------ Modification ------
    elif cls == "FilletFeature":
        p["radius"] = _extract_param_value(entity.radius)
        try:
            edges = []
            for i in range(entity.edges.count):
                e = entity.edges.item(i)
                edges.append({"index": i, "token": _token(e)})
            p["edges"] = edges
        except Exception:
            pass
        try:
            p["is_tangent_chain"] = _maybe("isTangentChain")
        except Exception:
            pass
        kind = "fillet"

    elif cls == "ChamferFeature":
        p["distance"] = _extract_param_value(entity.distance)
        p["distance_two"] = _extract_param_value(entity.distanceTwo)
        try:
            edges = []
            for i in range(entity.edges.count):
                e = entity.edges.item(i)
                edges.append({"index": i, "token": _token(e)})
            p["edges"] = edges
        except Exception:
            pass
        try:
            p["is_two_distances"] = _maybe("isTwoDistances")
        except Exception:
            pass
        kind = "chamfer"

    elif cls == "DraftFeature":
        p["draft_angle"] = _extract_param_value(entity.draftAngle)
        try:
            faces = []
            for i in range(entity.faces.count):
                f = entity.faces.item(i)
                faces.append({"index": i, "token": _token(f)})
            p["faces"] = faces
        except Exception:
            pass
        try:
            p["pull_direction_token"] = _token(entity.pullDirectionEntity)
        except Exception:
            pass
        try:
            p["neutral_plane_token"] = _token(entity.neutralPlane)
        except Exception:
            pass
        kind = "draft"

    elif cls == "ShellFeature":
        p["inside_thickness"] = _extract_param_value(entity.insideThickness)
        p["outside_thickness"] = _extract_param_value(entity.outsideThickness)
        try:
            faces = []
            for i in range(entity.faces.count):
                f = entity.faces.item(i)
                faces.append({"index": i, "token": _token(f)})
            p["faces"] = faces
        except Exception:
            pass
        kind = "shell"

    elif cls == "OffsetFacesFeature":
        p["offset_distance"] = _extract_param_value(entity.offsetDistance)
        try:
            faces = []
            for i in range(entity.faces.count):
                f = entity.faces.item(i)
                faces.append({"index": i, "token": _token(f)})
            p["faces"] = faces
        except Exception:
            pass
        kind = "offset"

    elif cls == "HoleFeature":
        p["hole_diameter"] = _extract_param_value(entity.holeDiameter)
        p["tip_angle"] = _extract_param_value(entity.tipAngle)
        p["is_cosmetic_thread"] = _maybe("isCosmeticThread")
        p["is_tapered"] = _maybe("isTapered")
        p["taper_angle"] = _extract_param_value(entity.taperAngle)
        p["is_associative"] = _maybe("isAssociative")
        try:
            p["thread_type"] = _safe(entity.threadInfo.threadType)
            p["thread_size"] = _safe(entity.threadInfo.threadSize)
            p["thread_designation"] = _safe(entity.threadInfo.designation)
            p["thread_class"] = _safe(entity.threadInfo.threadClass)
        except Exception:
            pass
        try:
            p["sketch_token"] = _token(entity.sketch)
        except Exception:
            pass
        try:
            p["face_token"] = _token(entity.placementFace)
        except Exception:
            pass
        try:
            p["point"] = _vec(entity.placementPoint)
        except Exception:
            pass
        kind = "hole"

    elif cls == "ThreadFeature":
        try:
            p["thread_type"] = _safe(entity.threadInfo.threadType)
            p["thread_size"] = _safe(entity.threadInfo.threadSize)
            p["thread_designation"] = _safe(entity.threadInfo.designation)
        except Exception:
            pass
        try:
            faces = []
            for i in range(entity.faces.count):
                f = entity.faces.item(i)
                faces.append({"index": i, "token": _token(f)})
            p["faces"] = faces
        except Exception:
            pass
        kind = "thread"

    # ------ Patterns ------
    elif cls == "RectangularPatternFeature":
        p["quantity_one"] = _maybe("quantityOne")
        p["quantity_two"] = _maybe("quantityTwo")
        p["distance_one"] = _extract_param_value(entity.distanceOne)
        p["distance_two"] = _extract_param_value(entity.distanceTwo)
        try:
            p["direction_one_token"] = _token(entity.directionOneEntity)
        except Exception:
            pass
        try:
            p["direction_two_token"] = _token(entity.directionTwoEntity)
        except Exception:
            pass
        # Originals - features this pattern operates on
        originals = []
        try:
            for i in range(entity.parentFeatures.count):
                originals.append({
                    "index": i,
                    "token": _token(entity.parentFeatures.item(i)),
                    "name": _safe(entity.parentFeatures.item(i).name),
                })
        except Exception:
            pass
        p["originals"] = originals
        try:
            p["body_token"] = _token(entity.targetBody)
        except Exception:
            pass
        kind = "rect_pattern"

    elif cls == "CircularPatternFeature":
        p["quantity"] = _maybe("quantity")
        p["total_angle"] = _extract_param_value(entity.totalAngle)
        try:
            p["axis_token"] = _token(entity.axis)
        except Exception:
            pass
        originals = []
        try:
            for i in range(entity.parentFeatures.count):
                originals.append({
                    "index": i,
                    "token": _token(entity.parentFeatures.item(i)),
                    "name": _safe(entity.parentFeatures.item(i).name),
                })
        except Exception:
            pass
        p["originals"] = originals
        try:
            p["body_token"] = _token(entity.targetBody)
        except Exception:
            pass
        kind = "circ_pattern"

    elif cls == "MirrorFeature":
        try:
            p["mirror_plane_token"] = _token(entity.mirrorPlane)
        except Exception:
            pass
        originals = []
        try:
            for i in range(entity.parentFeatures.count):
                originals.append({
                    "index": i,
                    "token": _token(entity.parentFeatures.item(i)),
                    "name": _safe(entity.parentFeatures.item(i).name),
                })
        except Exception:
            pass
        p["originals"] = originals
        try:
            p["body_token"] = _token(entity.targetBody)
        except Exception:
            pass
        kind = "mirror"

    elif cls == "PathPatternFeature":
        p["quantity"] = _maybe("quantity")
        p["distance"] = _extract_param_value(entity.distance)
        try:
            p["path_token"] = _token(entity.path)
        except Exception:
            pass
        originals = []
        try:
            for i in range(entity.parentFeatures.count):
                originals.append({
                    "index": i,
                    "token": _token(entity.parentFeatures.item(i)),
                    "name": _safe(entity.parentFeatures.item(i).name),
                })
        except Exception:
            pass
        p["originals"] = originals
        try:
            p["body_token"] = _token(entity.targetBody)
        except Exception:
            pass
        kind = "path_pattern"

    # ------ Boolean / Combine ------
    elif cls in ("BooleanFeature", "CombineFeature"):
        op = _maybe("operation")
        p["operation"] = op
        p["operation_string"] = {
            adsk.fusion.FeatureOperations.JoinFeatureOperation: "Join",
            adsk.fusion.FeatureOperations.CutFeatureOperation: "Cut",
            adsk.fusion.FeatureOperations.IntersectFeatureOperation: "Intersect",
        }.get(op, "Join")
        p["keep_tools"] = _maybe("keepTools") if hasattr(entity, "keepTools") else None
        targets = []
        tools = []
        try:
            for i in range(entity.targetBody.count):
                b = entity.targetBody.item(i)
                targets.append({"index": i, "token": _token(b), "name": _safe(b.name)})
        except Exception:
            pass
        try:
            for i in range(entity.toolBodies.count):
                b = entity.toolBodies.item(i)
                tools.append({"index": i, "token": _token(b), "name": _safe(b.name)})
        except Exception:
            pass
        p["targets"] = targets
        p["tools"] = tools
        kind = "boolean"

    # ------ Primitives ------
    elif cls in ("BoxFeature", "AdditiveBoxFeature", "SubtractiveBoxFeature"):
        p["length"] = _extract_param_value(entity.length)
        p["width"] = _extract_param_value(entity.width)
        p["height"] = _extract_param_value(entity.height)
        kind = "box"

    elif cls in ("CylinderFeature", "AdditiveCylinderFeature", "SubtractiveCylinderFeature"):
        p["radius"] = _extract_param_value(entity.radius)
        p["height"] = _extract_param_value(entity.height)
        kind = "cylinder"

    elif cls in ("SphereFeature", "AdditiveSphereFeature", "SubtractiveSphereFeature"):
        p["radius"] = _extract_param_value(entity.radius)
        kind = "sphere"

    elif cls in ("TorusFeature", "AdditiveTorusFeature", "SubtractiveTorusFeature"):
        p["outer_radius"] = _extract_param_value(entity.outerRadius)
        p["inner_radius"] = _extract_param_value(entity.innerRadius)
        kind = "torus"

    elif cls in ("ConeFeature", "AdditiveConeFeature", "SubtractiveConeFeature"):
        p["base_radius"] = _extract_param_value(entity.baseRadius)
        p["top_radius"] = _extract_param_value(entity.topRadius)
        p["height"] = _extract_param_value(entity.height)
        kind = "cone"

    # ------ Other ------
    elif cls == "ScaleFeature":
        p["scale_factor"] = _value(entity.scaleFactor) if hasattr(entity, "scaleFactor") else None
        try:
            p["body_token"] = _token(entity.targetBody)
        except Exception:
            pass
        kind = "scale"

    elif cls == "ReplaceFaceFeature":
        try:
            p["source_faces"] = [
                {"index": i, "token": _token(entity.sourceFaces.item(i))}
                for i in range(entity.sourceFaces.count)
            ]
        except Exception:
            pass
        try:
            p["target_faces"] = [
                {"index": i, "token": _token(entity.targetFaces.item(i))}
                for i in range(entity.targetFaces.count)
            ]
        except Exception:
            pass
        try:
            p["is_offset"] = _maybe("isOffset")
        except Exception:
            pass
        try:
            p["offset"] = _extract_param_value(entity.offset)
        except Exception:
            pass
        kind = "replace_face"

    else:
        kind = cls.lower()

    return kind, p


# ---------------------------------------------------------------------
# Parameters
# ---------------------------------------------------------------------

def _extract_parameters(design):
    out = {"user": [], "model": []}
    try:
        for p in design.userParameters:
            out["user"].append({
                "name": p.name,
                "value": _value(p),
                "expression": _expression(p),
                "unit": _unit(p),
                "comment": _safe(getattr(p, "comment", "")),
                "token": _token(p),
            })
    except Exception:
        pass
    try:
        for p in design.allParameters:
            out["model"].append({
                "name": p.name,
                "value": _value(p),
                "expression": _expression(p),
                "unit": _unit(p),
                "comment": _safe(getattr(p, "comment", "")),
                "token": _token(p),
            })
    except Exception:
        pass
    return out


# ---------------------------------------------------------------------
# Construction geometry
# ---------------------------------------------------------------------

def _extract_construction(plane_or_axis_or_point):
    try:
        if isinstance(plane_or_axis_or_point, adsk.fusion.ConstructionPlane):
            return {
                "kind": "plane",
                "name": _safe(plane_or_axis_or_point.name),
                "token": _token(plane_or_axis_or_point),
                "origin": _vec(plane_or_axis_or_point.origin),
                "normal": _vec(plane_or_axis_or_point.normal),
                "u_axis": _vec(plane_or_axis_or_point.uDirection),
                "v_axis": _vec(plane_or_axis_or_point.vDirection),
                "is_visible": _safe(plane_or_axis_or_point.isVisible),
            }
        elif isinstance(plane_or_axis_or_point, adsk.fusion.ConstructionAxis):
            return {
                "kind": "axis",
                "name": _safe(plane_or_axis_or_point.name),
                "token": _token(plane_or_axis_or_point),
                "origin": _vec(plane_or_axis_or_point.origin),
                "direction": _vec(plane_or_axis_or_point.direction),
                "is_visible": _safe(plane_or_axis_or_point.isVisible),
            }
        elif isinstance(plane_or_axis_or_point, adsk.fusion.ConstructionPoint):
            return {
                "kind": "point",
                "name": _safe(plane_or_axis_or_point.name),
                "token": _token(plane_or_axis_or_point),
                "point": _vec(plane_or_axis_or_point.geometry),
                "is_visible": _safe(plane_or_axis_or_point.isVisible),
            }
    except Exception as exc:
        return {"error": str(exc)}
    return None


# ---------------------------------------------------------------------
# Timeline walk
# ---------------------------------------------------------------------

def _extract_timeline(design):
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
            "entity_token": _token(entity),
        }

        if isinstance(entity, adsk.fusion.Sketch):
            entry["feature_kind"] = "sketch"
            entry["sketch"] = _extract_sketch(entity)

        elif isinstance(entity, adsk.fusion.ConstructionPlane):
            entry["feature_kind"] = "construction_plane"
            entry["construction"] = _extract_construction(entity)
        elif isinstance(entity, adsk.fusion.ConstructionAxis):
            entry["feature_kind"] = "construction_axis"
            entry["construction"] = _extract_construction(entity)
        elif isinstance(entity, adsk.fusion.ConstructionPoint):
            entry["feature_kind"] = "construction_point"
            entry["construction"] = _extract_construction(entity)

        elif isinstance(entity, adsk.fusion.Occurrence):
            entry["feature_kind"] = "occurrence"
            entry["occurrence"] = _extract_occurrence(entity)

        elif isinstance(entity, adsk.fusion.Body):
            entry["feature_kind"] = "body"
            entry["body"] = _extract_body(entity)

        else:
            kind, params = _classify_feature(entity)
            entry["feature_kind"] = kind
            entry["params"] = params

        out.append(entry)
    return out


# ---------------------------------------------------------------------
# Occurrences and components
# ---------------------------------------------------------------------

def _extract_occurrence(occ):
    """An occurrence is an instance of a component in an assembly."""
    out = {
        "name": _safe(occ.name),
        "token": _token(occ),
        "is_visible": _safe(occ.isVisible),
    }
    try:
        comp = occ.component
        out["component_name"] = _safe(comp.name)
        out["component_token"] = _token(comp)
    except Exception:
        pass
    try:
        tr = occ.transform
        out["transform"] = [
            _safe(tr.getCell(0, 0)), _safe(tr.getCell(0, 1)),
            _safe(tr.getCell(0, 2)), _safe(tr.getCell(0, 3)),
            _safe(tr.getCell(1, 0)), _safe(tr.getCell(1, 1)),
            _safe(tr.getCell(1, 2)), _safe(tr.getCell(1, 3)),
            _safe(tr.getCell(2, 0)), _safe(tr.getCell(2, 1)),
            _safe(tr.getCell(2, 2)), _safe(tr.getCell(2, 3)),
        ]
    except Exception:
        out["transform"] = None
    return out


def _extract_components(root_component):
    """Walk the entire component tree of the design."""
    components = []
    appearances = []

    def _walk(comp, depth):
        bodies = []
        for b in comp.bodies:
            bodies.append(_extract_body(b))
        appearances_list = []
        try:
            for i in range(comp.appearances.count):
                appearances_list.append({
                    "name": _safe(comp.appearances.item(i).name),
                })
        except Exception:
            pass
        sketches = []
        for s in comp.sketches:
            sketches.append({
                "name": _safe(s.name),
                "token": _token(s),
            })
        construction = []
        for cp in comp.constructionPlanes:
            construction.append(_extract_construction(cp))
        for ca in comp.constructionAxes:
            construction.append(_extract_construction(ca))
        for cpt in comp.constructionPoints:
            construction.append(_extract_construction(cpt))

        component = {
            "name": _safe(comp.name),
            "token": _token(comp),
            "depth": depth,
            "bodies": bodies,
            "sketches": sketches,
            "construction": construction,
            "appearances": appearances_list,
            "origin": _vec(comp.originGeometry.origin) if comp.originGeometry else None,
            "x_axis": _vec(comp.originGeometry.xAxis) if comp.originGeometry else None,
            "y_axis": _vec(comp.originGeometry.yAxis) if comp.originGeometry else None,
            "z_axis": _vec(comp.originGeometry.zAxis) if comp.originGeometry else None,
            "occurrences": [],
            "children": [],
        }
        for occ in comp.occurrences:
            occ_entry = {
                "name": _safe(occ.name),
                "token": _token(occ),
                "transform": [
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
                ] if occ.transform else None,
                "child": _walk(occ.component, depth + 1),
            }
            component["children"].append(occ_entry)
        return component

    return _walk(root_component, 0)


# ---------------------------------------------------------------------
# STEP export
# ---------------------------------------------------------------------

def _export_step(design, path):
    export_mgr = design.exportManager
    options = export_mgr.createSTEPExportOptions(path)
    export_mgr.execute(options)
    return os.path.exists(path)


# ---------------------------------------------------------------------
# Main entry point
# ---------------------------------------------------------------------

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

        # Export STEP for geometry verification.
        step_path = os.path.join(tempfile.gettempdir(), "fusion_export.step")
        step_exported = False
        step_error = None
        try:
            step_exported = _export_step(design, step_path)
        except Exception as exc:
            step_error = str(exc)

        # Capture all appearances defined in the document.
        doc_appearances = []
        try:
            lib = app.materialLibraries
            for i in range(lib.count):
                libobj = lib.item(i)
                try:
                    for j in range(libobj.materials.count):
                        m = libobj.materials.item(j)
                        doc_appearances.append({
                            "name": _safe(m.name),
                            "id": _safe(getattr(m, "id", None)),
                            "library": _safe(libobj.name),
                        })
                except Exception:
                    pass
        except Exception:
            pass

        result = {
            "schema": 2,
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
            "appearances": doc_appearances,
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
HISTORY_SCHEMA = """{
    "schema": 2,
    "document": "name of the source document",
    "design_name": "name of the design",
    "design_type": "parametric | direct | unknown",
    "units": "default length unit string, e.g. 'mm' or 'in'",
    "root_component": {
        "name": "component name",
        "token": "Fusion entity token",
        "bodies": [
            {
                "name", "token", "faces": [...], "edges": [...],
                "vertices": [...], "appearance": {...}, "material": {...},
                "volume", "bbox_min", "bbox_max"
            }
        ],
        "sketches": [{"name", "token"}],
        "construction": [{"kind": "plane|axis|point", "name", "token", ...}],
        "appearances": [{"name"}],
        "origin", "x_axis", "y_axis", "z_axis",
        "children": [
            {"name", "token", "transform": [16 floats], "child": {...}}
        ],
    },
    "parameters": {
        "user": [{"name", "value", "expression", "unit", "comment", "token"}],
        "model": [{"name", "value", "expression", "unit", "comment", "token"}],
    },
    "appearances": [
        {"name", "id", "library"}
    ],
    "timeline": [
        # Sketches
        {"feature_kind": "sketch", "sketch": {...}},
        # Construction
        {"feature_kind": "construction_plane|axis|point", "construction": {...}},
        # Occurrences (assembly instances)
        {"feature_kind": "occurrence", "occurrence": {...}},
        # Bodies (added directly, rare but possible)
        {"feature_kind": "body", "body": {...}},
        # Parametric features
        {"feature_kind": "extrude|revolve|loft|sweep|coil|fillet|chamfer|...|boolean",
         "params": {...}},
    ],
    "step_export": {
        "path": "absolute filesystem path of the exported STEP file",
        "size": "byte size",
        "error": "None on success, error string on failure",
    },
}"""