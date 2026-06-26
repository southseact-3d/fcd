# SPDX-License-Identifier: LGPL-2.1-or-later
"""Reconstruct a parametric feature tree inside Tungsten CAD.

The rebuilder consumes a :class:`extractor.FusionDesignHistory` and
walks its timeline. Each entry is mapped onto a FreeCAD/PartDesign
feature class:

    extrude        -> PartDesign::Pad or Pocket (operation-dependent)
    revolve        -> PartDesign::Revolution or Groove
    loft           -> PartDesign::AdditiveLoft or SubtractiveLoft
    sweep          -> PartDesign::AdditivePipe or SubtractivePipe
    coil           -> PartDesign::AdditiveHelix or SubtractiveHelix
    fillet         -> PartDesign::Fillet
    chamfer        -> PartDesign::Chamfer
    draft          -> PartDesign::Draft
    shell          -> PartDesign::Thickness
    offset         -> PartDesign::Thickness (best-effort)
    hole           -> PartDesign::Hole
    thread         -> PartDesign::Hole (cosmetic)
    rect_pattern   -> PartDesign::LinearPattern
    circ_pattern   -> PartDesign::PolarPattern
    path_pattern   -> PartDesign::PatternOnPath
    mirror         -> PartDesign::Mirrored
    boolean        -> PartDesign::Boolean
    box, cylinder,
    sphere, torus,
    cone           -> PartDesign::Additive* primitive
    scale          -> PartDesign::Scaled
    replace_face   -> skipped (record-only)
    sketch         -> Sketcher::SketchObject (delegated)
    construction_* -> Part::Datum* (created in body group)
    occurrence     -> Part::Compound + transform
    body           -> PartDesign::Body

Everything happens inside a single ``doc.openTransaction(...)`` call
so the user can undo the entire import with one Ctrl+Z.
"""

from __future__ import annotations

import math
from dataclasses import dataclass, field
from typing import Any

from . import reference_resolver, sketch_rebuilder


@dataclass
class RebuildResult:
    """Summary of a rebuild run for the GUI status panel."""

    body_name: str = ""
    feature_count: int = 0
    sketch_count: int = 0
    construction_count: int = 0
    body_count: int = 0
    occurrence_count: int = 0
    skipped: list[str] = field(default_factory=list)
    warnings: list[str] = field(default_factory=list)


# Fusion 360's FeatureOperations enum (from adsk.fusion.FeatureOperations):
#   JoinFeatureOperation         = 0  -> Pad / Additive* / Boolean "Join"
#   CutFeatureOperation          = 1  -> Pocket / Subtractive* / Boolean "Cut"
#   IntersectFeatureOperation    = 2  -> Subtractive*/Boolean "Intersect"
_JOIN = 0
_CUT = 1
_INTERSECT = 2


def _op_to_pad_pocket(op):
    if op == _CUT:
        return "PartDesign::Pocket"
    if op == _INTERSECT:
        # PartDesign has no direct intersect extrude; use Pocket as a
        # close approximation - users will need to convert.
        return "PartDesign::Pocket"
    return "PartDesign::Pad"


def _op_to_revolve_groove(op):
    if op == _CUT:
        return "PartDesign::Groove"
    return "PartDesign::Revolution"


def _op_to_loft(op):
    if op == _CUT:
        return "PartDesign::SubtractiveLoft"
    return "PartDesign::AdditiveLoft"


def _op_to_pipe(op):
    if op == _CUT:
        return "PartDesign::SubtractivePipe"
    return "PartDesign::AdditivePipe"


def _op_to_helix(op):
    if op == _CUT:
        return "PartDesign::SubtractiveHelix"
    return "PartDesign::AdditiveHelix"


_BOOLEAN_OP_NAMES = {_JOIN: "Join", _CUT: "Cut", _INTERSECT: "Intersect"}


class FeatureRebuilder:
    """Reconstruct a Fusion design as one or more PartDesign::Bodies.

    The rebuilder maintains a per-document index from Fusion tokens to
    the FreeCAD objects it created. This lets later features link to
    earlier ones (for example, a Pattern's ``Originals`` property
    pointing at the Pad it duplicates).
    """

    def __init__(self) -> None:
        self.result = RebuildResult()
        self._token_index: dict[str, dict[str, Any]] = {}
        self._sketch_token_to_obj: dict[str, Any] = {}
        self._feature_token_to_obj: dict[str, Any] = {}
        self._body_token_to_obj: dict[str, Any] = {}
        self._doc = None
        self._active_body = None

    # ------------------------------------------------------------------
    # Top-level entry point
    # ------------------------------------------------------------------

    def rebuild(
        self,
        doc: Any,
        history: Any,
        body_name: str | None = None,
        progress: Any | None = None,
    ) -> Any:
        """Walk the timeline and build the feature tree.

        Returns the *primary* body. Additional bodies (from
        occurrences) are added to the document and tracked in
        ``self._body_token_to_obj``.
        """
        body_name = body_name or self._safe_name(history.design_name or "FusionImport")
        report = progress or (lambda *_a, **_kw: None)
        self._doc = doc

        doc.openTransaction(f"Import Fusion 360 design: {history.document}")
        try:
            # Apply user / model parameters as Spreadsheet cells (best effort).
            self._apply_parameters(doc, history)

            body = doc.addObject("PartDesign::Body", body_name)
            self.result.body_name = body_name
            self._active_body = body
            self.result.body_count = 1

            timeline = history.timeline
            total = max(1, len(timeline))
            for idx, entry in enumerate(timeline):
                frac = 0.1 + 0.85 * (idx / total)
                kind = entry.get("feature_kind", "unknown")
                name = entry.get("name") or f"Feature_{idx}"
                report(f"Rebuilding {kind}: {name}", frac)
                try:
                    self._process_entry(entry, history, name)
                except Exception as exc:
                    self.result.warnings.append(
                        f"{kind} '{name}' failed: {exc}"
                    )

            doc.recompute()
            self._maybe_import_step(doc, body, history, report)

            doc.commitTransaction()
            self._post_token_scan(doc)
            return body
        except Exception:
            doc.abortTransaction()
            raise

    # ------------------------------------------------------------------
    # Token index maintenance
    # ------------------------------------------------------------------

    def _post_token_scan(self, doc: Any) -> None:
        """Build a global token index for later cross-reference lookups."""
        self._token_index = reference_resolver.build_global_index(doc)

    def _register_object(self, token: str | None, obj: Any, kind: str | None = None) -> None:
        if not token:
            return
        self._token_index.setdefault(
            token, {"object": obj, "kind": obj.TypeId if obj else kind}
        )

    def _resolve(self, ref: dict | None) -> tuple[Any | None, list[str]]:
        """Translate a Fusion reference to a (FreeCAD object, subnames) pair."""
        return reference_resolver.resolve_link_sub(ref, self._token_index)

    def _resolve_one(self, ref: dict | None) -> tuple[Any | None, list[str]]:
        return self._resolve(ref)

    # ------------------------------------------------------------------
    # Parameter handling
    # ------------------------------------------------------------------

    def _apply_parameters(self, doc: Any, history: Any) -> None:
        """Create a Spreadsheet holding all user parameters."""
        user_params = history.user_parameters
        if not user_params:
            return
        sheet_name = self._safe_name("FusionParams")
        # Avoid clashing with existing sheets.
        i = 0
        while doc.getObject(sheet_name):
            i += 1
            sheet_name = f"{sheet_name}_{i}"
        try:
            sheet = doc.addObject("Spreadsheet::Sheet", sheet_name)
        except Exception:
            # Spreadsheet module may be disabled.
            return
        sheet.set("A1", "Name")
        sheet.set("B1", "Expression")
        sheet.set("C1", "Value")
        sheet.set("D1", "Unit")
        sheet.set("E1", "Comment")
        for i, p in enumerate(user_params, start=2):
            sheet.set(f"A{i}", p.get("name") or "")
            expr = p.get("expression") or ""
            val = p.get("value")
            unit = p.get("unit") or ""
            comment = p.get("comment") or ""
            sheet.set(f"B{i}", expr)
            sheet.set(f"C{i}", val if val is not None else "")
            sheet.set(f"D{i}", unit)
            sheet.set(f"E{i}", comment)
            # If the expression is a literal, expose it as an alias so
            # the rest of the document can use ``<<params>>name``.
            if expr:
                try:
                    sheet.setAlias(f"B{i}", f"params_{p['name']}")
                except Exception:
                    pass
        self._param_sheet = sheet

    @staticmethod
    def _safe_name(name: str | None) -> str:
        if not name:
            return "FusionImport"
        cleaned = "".join(c if c.isalnum() or c in "-_." else "_" for c in name)
        if not cleaned[0].isalpha() and cleaned[0] != "_":
            cleaned = "_" + cleaned
        return cleaned[:64]

    # ------------------------------------------------------------------
    # Per-entry dispatch
    # ------------------------------------------------------------------

    def _process_entry(
        self,
        entry: dict,
        history: Any,
        default_name: str,
    ) -> None:
        kind = entry.get("feature_kind")
        token = entry.get("entity_token")

        if kind == "sketch":
            sketch, warnings = sketch_rebuilder.rebuild_sketch(
                self._doc, self._active_body, entry.get("sketch") or {}
            )
            self.result.sketch_count += 1
            if warnings:
                self.result.warnings.extend(warnings)
            if token:
                self._sketch_token_to_obj[token] = sketch
            self._register_object(token, sketch, "sketch")

        elif kind == "construction_plane":
            self._build_construction_plane(entry, default_name)
            self.result.construction_count += 1
        elif kind == "construction_axis":
            self._build_construction_axis(entry, default_name)
            self.result.construction_count += 1
        elif kind == "construction_point":
            self._build_construction_point(entry, default_name)
            self.result.construction_count += 1

        elif kind == "occurrence":
            self._build_occurrence(entry, default_name, history)
            self.result.occurrence_count += 1

        elif kind == "body":
            # Rare - usually bodies are added implicitly by other features.
            self.result.skipped.append(
                f"Body '{default_name}' - standalone bodies cannot be "
                f"recreated in a single PartDesign::Body container"
            )

        elif kind == "extrude":
            self._build_extrude(entry, default_name)
            self.result.feature_count += 1
        elif kind == "revolve":
            self._build_revolve(entry, default_name)
            self.result.feature_count += 1
        elif kind == "loft":
            self._build_loft(entry, default_name)
            self.result.feature_count += 1
        elif kind == "sweep":
            self._build_sweep(entry, default_name)
            self.result.feature_count += 1
        elif kind == "coil":
            self._build_coil(entry, default_name)
            self.result.feature_count += 1
        elif kind == "fillet":
            self._build_fillet(entry, default_name)
            self.result.feature_count += 1
        elif kind == "chamfer":
            self._build_chamfer(entry, default_name)
            self.result.feature_count += 1
        elif kind == "draft":
            self._build_draft(entry, default_name)
            self.result.feature_count += 1
        elif kind == "shell":
            self._build_shell(entry, default_name)
            self.result.feature_count += 1
        elif kind == "offset":
            self._build_offset(entry, default_name)
            self.result.feature_count += 1
        elif kind == "hole":
            self._build_hole(entry, default_name)
            self.result.feature_count += 1
        elif kind == "thread":
            self._build_thread(entry, default_name)
            self.result.feature_count += 1
        elif kind == "rect_pattern":
            self._build_rect_pattern(entry, default_name)
            self.result.feature_count += 1
        elif kind == "circ_pattern":
            self._build_polar_pattern(entry, default_name)
            self.result.feature_count += 1
        elif kind == "path_pattern":
            self._build_path_pattern(entry, default_name)
            self.result.feature_count += 1
        elif kind == "mirror":
            self._build_mirror(entry, default_name)
            self.result.feature_count += 1
        elif kind == "boolean":
            self._build_boolean(entry, default_name)
            self.result.feature_count += 1
        elif kind == "scale":
            self._build_scale(entry, default_name)
            self.result.feature_count += 1
        elif kind == "replace_face":
            self.result.skipped.append(
                f"ReplaceFace '{default_name}' - no Tungsten CAD equivalent"
            )

        elif kind in ("box", "cylinder", "sphere", "torus", "cone"):
            self._build_primitive(kind, entry, default_name)
            self.result.feature_count += 1

        else:
            # Unknown feature type - record but skip.
            self.result.skipped.append(
                f"Unhandled feature kind: {kind!r} ({default_name})"
            )

    # ------------------------------------------------------------------
    # Sketch-based features
    # ------------------------------------------------------------------

    def _lookup_sketch(self, params: dict, name: str) -> Any:
        """Find a sketch object referenced by a feature's params."""
        token = (params or {}).get("sketch_token") or (params or {}).get("profile_token")
        if token and token in self._sketch_token_to_obj:
            return self._sketch_token_to_obj[token]
        # Fallback: use the most recently created sketch in the body.
        for obj in reversed(self._active_body.Group):
            if obj.TypeId == "Sketcher::SketchObject":
                return obj
        self.result.warnings.append(f"{name}: no sketch reference found")
        return None

    def _build_extrude(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        sketch = self._lookup_sketch(params, name)
        if sketch is None:
            self.result.skipped.append(f"Extrude '{name}' - no sketch reference")
            return

        op = params.get("operation")
        feat = self._doc.addObject(_op_to_pad_pocket(op), name)
        try:
            feat.Profile = sketch
        except Exception as exc:
            self._doc.removeObject(feat.Name)
            self.result.skipped.append(
                f"Extrude '{name}' - could not link sketch: {exc}"
            )
            return

        # Length / type / direction
        eo = params.get("extent_one") or {}
        if eo.get("value") is not None:
            try:
                feat.Length = float(eo["value"])
            except Exception:
                pass
        if eo.get("expression"):
            try:
                feat.setExpression("Length", eo["expression"])
            except Exception:
                pass

        et = params.get("extent_two") or {}
        if et.get("value") is not None:
            try:
                feat.Length2 = float(et["value"])
            except Exception:
                pass

        ta1 = params.get("taper_angle_one") or {}
        if ta1.get("value") is not None:
            try:
                feat.TaperAngle = float(ta1["value"])
            except Exception:
                pass

        if params.get("is_symmetric"):
            try:
                feat.SideType = "Symmetric"
            except Exception:
                pass

        if params.get("operation_string") == "Cut":
            try:
                feat.Reversed = True
            except Exception:
                pass

        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "extrude")

    def _build_revolve(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        sketch = self._lookup_sketch(params, name)
        if sketch is None:
            self.result.skipped.append(f"Revolve '{name}' - no sketch reference")
            return

        op = params.get("operation")
        feat = self._doc.addObject(_op_to_revolve_groove(op), name)
        try:
            feat.Profile = sketch
        except Exception as exc:
            self._doc.removeObject(feat.Name)
            self.result.skipped.append(
                f"Revolve '{name}' - could not link sketch: {exc}"
            )
            return

        # Axis - default to Y axis (common choice for revolves).
        try:
            feat.ReferenceAxis = (self._doc.Y_Axis, [""])
        except Exception:
            pass

        ang = params.get("angle") or {}
        if ang.get("value") is not None:
            try:
                feat.Angle = float(ang["value"])
            except Exception:
                pass
        if ang.get("expression"):
            try:
                feat.setExpression("Angle", ang["expression"])
            except Exception:
                pass

        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "revolve")

    def _build_loft(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        op = params.get("operation")
        feat = self._doc.addObject(_op_to_loft(op), name)
        try:
            feat.Profile = self._lookup_sketch(params, name)
        except Exception:
            pass
        # Sections - all sketches after the first profile.
        section_tokens = [
            s.get("sketch_token") or s.get("profile_token")
            for s in (params.get("sections") or [])
        ]
        sections = [
            self._sketch_token_to_obj[t]
            for t in section_tokens
            if t and t in self._sketch_token_to_obj
        ]
        if sections:
            try:
                feat.Sections = sections
            except Exception as exc:
                self.result.warnings.append(
                    f"Loft '{name}' could not set Sections: {exc}"
                )

        try:
            if params.get("is_closed"):
                feat.Closed = True
        except Exception:
            pass
        try:
            if params.get("is_ruled"):
                feat.Ruled = True
        except Exception:
            pass

        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "loft")

    def _build_sweep(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        op = params.get("operation")
        feat = self._doc.addObject(_op_to_pipe(op), name)
        try:
            feat.Profile = self._lookup_sketch(params, name)
        except Exception:
            pass
        # Spine - try to find the sketch referenced by path_token.
        spine_token = params.get("path_token")
        spine = self._sketch_token_to_obj.get(spine_token)
        if spine is not None:
            try:
                feat.Spine = spine
            except Exception as exc:
                self.result.warnings.append(
                    f"Sweep '{name}' could not set Spine: {exc}"
                )
        else:
            self.result.warnings.append(
                f"Sweep '{name}' - no spine sketch resolved"
            )
        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "sweep")

    def _build_coil(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        op = params.get("operation")
        feat = self._doc.addObject(_op_to_helix(op), name)
        try:
            feat.Profile = self._lookup_sketch(params, name)
        except Exception:
            pass
        try:
            feat.ReferenceAxis = (self._doc.Y_Axis, [""])
        except Exception:
            pass

        height = params.get("height") or {}
        pitch = params.get("pitch") or {}
        revs = params.get("revolutions") or {}
        ang = params.get("angle") or {}
        section = params.get("section_size") or {}

        # Use "pitch-height-angle" mode by default.
        try:
            feat.Mode = "pitch-height-angle"
        except Exception:
            pass

        if pitch.get("value") is not None:
            try:
                feat.Pitch = float(pitch["value"])
            except Exception:
                pass
        if height.get("value") is not None:
            try:
                feat.Height = float(height["value"])
            except Exception:
                pass
        if revs.get("value") is not None:
            try:
                feat.Turns = float(revs["value"])
            except Exception:
                pass
        if ang.get("value") is not None:
            try:
                feat.Angle = float(ang["value"])
            except Exception:
                pass

        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "coil")

    # ------------------------------------------------------------------
    # Modification features
    # ------------------------------------------------------------------

    def _link_edges(self, feat: Any, edges: list[dict] | None) -> bool:
        """Try to link a list of edge references onto a DressUp feature.

        Returns True on success, False if no edges could be linked.
        """
        if not edges:
            return False
        # Resolve tokens via the global index.
        sub_objs: list[tuple[Any, list[str]]] = []
        for e in edges:
            obj, subs = self._resolve(e)
            if obj is not None:
                sub_objs.append((obj, subs))
        if not sub_objs:
            return False
        try:
            feat.Base = sub_objs
            return True
        except Exception:
            return False

    def _build_fillet(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        feat = self._doc.addObject("PartDesign::Fillet", name)
        radius = params.get("radius") or {}
        if radius.get("value") is not None:
            try:
                feat.Radius = float(radius["value"])
            except Exception:
                pass
        if radius.get("expression"):
            try:
                feat.setExpression("Radius", radius["expression"])
            except Exception:
                pass

        linked = self._link_edges(feat, params.get("edges"))
        if not linked:
            # Fall back to using all edges.
            feat.UseAllEdges = True
        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "fillet")

    def _build_chamfer(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        feat = self._doc.addObject("PartDesign::Chamfer", name)
        size = params.get("distance") or {}
        if size.get("value") is not None:
            try:
                feat.Size = float(size["value"])
            except Exception:
                pass
        if params.get("is_two_distances"):
            try:
                feat.ChamferType = "TwoDistances"
                size2 = params.get("distance_two") or {}
                if size2.get("value") is not None:
                    feat.Size2 = float(size2["value"])
            except Exception:
                pass

        linked = self._link_edges(feat, params.get("edges"))
        if not linked:
            feat.UseAllEdges = True
        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "chamfer")

    def _build_draft(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        feat = self._doc.addObject("PartDesign::Draft", name)
        ang = params.get("draft_angle") or {}
        if ang.get("value") is not None:
            try:
                feat.Angle = float(ang["value"])
            except Exception:
                pass
        # Faces + neutral plane + pull direction - best effort.
        faces = params.get("faces") or []
        if faces:
            refs = []
            for f in faces:
                obj, subs = self._resolve(f)
                if obj is not None:
                    refs.append((obj, subs))
            if refs:
                try:
                    feat.Base = refs
                except Exception:
                    pass
        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "draft")

    def _build_shell(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        feat = self._doc.addObject("PartDesign::Thickness", name)
        thickness = params.get("outside_thickness") or params.get("inside_thickness")
        if thickness and thickness.get("value") is not None:
            try:
                feat.Value = float(thickness["value"])
            except Exception:
                pass
        # If thickness is "inside", set Mode to Skin.
        if params.get("inside_thickness"):
            try:
                feat.Reversed = True
            except Exception:
                pass
        # Faces to remove.
        faces = params.get("faces") or []
        if faces:
            refs = []
            for f in faces:
                obj, subs = self._resolve(f)
                if obj is not None:
                    refs.append((obj, subs))
            if refs:
                try:
                    feat.Base = refs
                except Exception:
                    pass
        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "shell")

    def _build_offset(self, entry: dict, name: str) -> None:
        """Offset faces - approximate via Thickness (only works if uniform)."""
        params = entry.get("params", {}) or {}
        self.result.skipped.append(
            f"OffsetFaces '{name}' - approximated as Thickness; refine in GUI"
        )
        self._build_shell(entry, name)

    def _build_hole(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        sketch = self._lookup_sketch(params, name)
        if sketch is None:
            self.result.skipped.append(f"Hole '{name}' - no sketch reference")
            return

        feat = self._doc.addObject("PartDesign::Hole", name)
        try:
            feat.Profile = sketch
        except Exception as exc:
            self._doc.removeObject(feat.Name)
            self.result.skipped.append(f"Hole '{name}' - could not link sketch: {exc}")
            return

        diameter = params.get("hole_diameter") or {}
        if diameter.get("value") is not None:
            try:
                feat.Diameter = float(diameter["value"])
            except Exception:
                pass

        if params.get("is_cosmetic_thread") or params.get("thread_designation"):
            try:
                feat.Threaded = True
                designation = params.get("thread_designation") or ""
                thread_type = params.get("thread_type")
                if thread_type and hasattr(feat, "ThreadType"):
                    # Map Fusion thread type names to FreeCAD enum values.
                    type_map = {
                        "ISOmetric": "ISOmetric",
                        "ISOmetricfine": "ISOmetricfine",
                        "UNC": "UNC",
                        "UNF": "UNF",
                        "UNEF": "UNEF",
                        "NPT": "NPT",
                        "BSP": "BSP",
                        "BSW": "BSW",
                        "BSF": "BSF",
                    }
                    mapped = type_map.get(thread_type)
                    if mapped:
                        feat.ThreadType = mapped
                if designation:
                    try:
                        feat.ThreadSize = designation
                    except Exception:
                        pass
            except Exception as exc:
                self.result.warnings.append(
                    f"Hole '{name}' thread setup partial: {exc}"
                )

        # Placement at point vs sketch.
        try:
            if params.get("face_token") and params.get("point"):
                face_obj, face_subs = self._resolve_one({"token": params["face_token"]})
                if face_obj is not None:
                    feat.Placement = "AtPoint"
                    feat.PlacementFace = (face_obj, face_subs)
                    if params["point"] is not None:
                        feat.PlacementPoint = (
                            params["point"][0] or 0.0,
                            params["point"][1] or 0.0,
                            params["point"][2] or 0.0,
                        )
        except Exception:
            pass

        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "hole")

    def _build_thread(self, entry: dict, name: str) -> None:
        """Cosmetic threads are recorded but rarely rebuild to Hole."""
        params = entry.get("params", {}) or {}
        self.result.skipped.append(
            f"Thread '{name}' - cosmetic threads recorded; add a Hole feature "
            f"manually with Threaded=True if you need the model thread"
        )

    # ------------------------------------------------------------------
    # Patterns and transforms
    # ------------------------------------------------------------------

    def _resolve_originals(self, params: dict) -> list[Any]:
        """Resolve a Pattern's Originals property to a list of features."""
        originals = []
        for orig in params.get("originals") or []:
            token = orig.get("token")
            if token and token in self._feature_token_to_obj:
                originals.append(self._feature_token_to_obj[token])
            else:
                obj, _ = self._resolve_one(orig)
                if obj is not None:
                    originals.append(obj)
        return originals

    def _build_rect_pattern(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        feat = self._doc.addObject("PartDesign::LinearPattern", name)
        originals = self._resolve_originals(params)
        if originals:
            try:
                feat.Originals = originals
            except Exception as exc:
                self.result.warnings.append(
                    f"RectangularPattern '{name}' could not set Originals: {exc}"
                )
        # Direction 1.
        dir_ref = params.get("direction_one_token")
        if dir_ref:
            obj, subs = self._resolve_one({"token": dir_ref})
            if obj is not None:
                try:
                    feat.Direction = (obj, subs)
                except Exception:
                    pass
        # Direction 2.
        dir_ref2 = params.get("direction_two_token")
        if dir_ref2:
            obj, subs = self._resolve_one({"token": dir_ref2})
            if obj is not None:
                try:
                    feat.Direction2 = (obj, subs)
                except Exception:
                    pass
        # Counts / distances.
        try:
            feat.Occurrences = int(params.get("quantity_one") or 2)
        except Exception:
            pass
        d1 = params.get("distance_one") or {}
        if d1.get("value") is not None:
            try:
                # Fusion's distance_one is the *overall* length; FreeCAD's
                # Length is the same, with Offset being the per-step.
                feat.Length = float(d1["value"])
                if int(params.get("quantity_one") or 2) > 1:
                    feat.Offset = float(d1["value"]) / (int(params["quantity_one"]) - 1)
            except Exception:
                pass

        if params.get("quantity_two") and int(params["quantity_two"]) > 1:
            try:
                feat.Occurrences2 = int(params["quantity_two"])
                d2 = params.get("distance_two") or {}
                if d2.get("value") is not None:
                    feat.Length2 = float(d2["value"])
                    feat.Offset2 = float(d2["value"]) / (int(params["quantity_two"]) - 1)
            except Exception:
                pass

        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "rect_pattern")

    def _build_polar_pattern(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        feat = self._doc.addObject("PartDesign::PolarPattern", name)
        originals = self._resolve_originals(params)
        if originals:
            try:
                feat.Originals = originals
            except Exception as exc:
                self.result.warnings.append(
                    f"CircularPattern '{name}' could not set Originals: {exc}"
                )

        axis_ref = params.get("axis_token")
        if axis_ref:
            obj, subs = self._resolve_one({"token": axis_ref})
            if obj is not None:
                try:
                    feat.Axis = (obj, subs)
                except Exception:
                    pass

        try:
            feat.Occurrences = int(params.get("quantity") or 4)
        except Exception:
            pass

        ang = params.get("total_angle") or {}
        if ang.get("value") is not None:
            try:
                feat.Angle = float(ang["value"])
            except Exception:
                pass

        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "polar_pattern")

    def _build_path_pattern(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        feat = self._doc.addObject("PartDesign::PatternOnPath", name)
        originals = self._resolve_originals(params)
        if originals:
            try:
                feat.Originals = originals
            except Exception as exc:
                self.result.warnings.append(
                    f"PathPattern '{name}' could not set Originals: {exc}"
                )
        path_ref = params.get("path_token")
        if path_ref:
            obj, subs = self._resolve_one({"token": path_ref})
            if obj is not None:
                try:
                    feat.Path = (obj, subs)
                except Exception:
                    pass
        try:
            feat.Occurrences = int(params.get("quantity") or 4)
        except Exception:
            pass
        dist = params.get("distance") or {}
        if dist.get("value") is not None:
            try:
                feat.Length = float(dist["value"])
            except Exception:
                pass
        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "path_pattern")

    def _build_mirror(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        feat = self._doc.addObject("PartDesign::Mirrored", name)
        originals = self._resolve_originals(params)
        if originals:
            try:
                feat.Originals = originals
            except Exception as exc:
                self.result.warnings.append(
                    f"Mirror '{name}' could not set Originals: {exc}"
                )

        plane_ref = params.get("mirror_plane_token")
        if plane_ref:
            obj, subs = self._resolve_one({"token": plane_ref})
            if obj is not None:
                try:
                    feat.MirrorPlane = (obj, subs)
                except Exception:
                    self.result.warnings.append(
                        f"Mirror '{name}' could not resolve mirror plane"
                    )
        else:
            self.result.warnings.append(
                f"Mirror '{name}' - no mirror plane resolved"
            )

        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "mirror")

    def _build_scale(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        feat = self._doc.addObject("PartDesign::Scaled", name)
        originals = self._resolve_originals(params)
        if originals:
            try:
                feat.Originals = originals
            except Exception as exc:
                self.result.warnings.append(
                    f"Scale '{name}' could not set Originals: {exc}"
                )
        factor = params.get("scale_factor")
        if factor is not None:
            try:
                feat.Factor = float(factor)
            except Exception:
                pass
        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "scale")

    # ------------------------------------------------------------------
    # Boolean
    # ------------------------------------------------------------------

    def _build_boolean(self, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        op = params.get("operation")
        op_str = params.get("operation_string") or _BOOLEAN_OP_NAMES.get(op, "Join")

        # The PartDesign::Boolean takes target bodies via setObjects(); it
        # lives inside the destination body. We add it to the *active*
        # body and use the first target as the BaseFeature, then list
        # any extra targets + tools as inputs.
        targets = []
        for t in params.get("targets") or []:
            obj, _ = self._resolve_one(t)
            if obj is not None:
                targets.append(obj)
        tools = []
        for t in params.get("tools") or []:
            obj, _ = self._resolve_one(t)
            if obj is not None:
                tools.append(obj)

        if not targets and not tools:
            self.result.skipped.append(
                f"Boolean '{name}' - no target or tool bodies resolved"
            )
            return

        feat = self._doc.addObject("PartDesign::Boolean", name)
        try:
            feat.Type = op_str
        except Exception:
            pass

        inputs = (targets + tools) or targets or tools
        try:
            feat.setObjects(inputs)
        except Exception as exc:
            self._doc.removeObject(feat.Name)
            self.result.warnings.append(
                f"Boolean '{name}' could not set objects: {exc}"
            )
            return

        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, "boolean")

    # ------------------------------------------------------------------
    # Primitives
    # ------------------------------------------------------------------

    def _build_primitive(self, kind: str, entry: dict, name: str) -> None:
        params = entry.get("params", {}) or {}
        type_id = f"PartDesign::Additive{kind.capitalize()}"
        feat = self._doc.addObject(type_id, name)
        # Field name -> FreeCAD property name mapping.
        field_map = {
            "length": "Length",
            "width": "Width",
            "height": "Height",
            "radius": "Radius",
            "outer_radius": "Radius1",
            "inner_radius": "Radius2",
            "base_radius": "Radius1",
            "top_radius": "Radius2",
        }
        for src, dst in field_map.items():
            entry_param = params.get(src) or {}
            val = entry_param.get("value") if isinstance(entry_param, dict) else entry_param
            if val is not None and hasattr(feat, dst):
                try:
                    setattr(feat, dst, float(val))
                except Exception:
                    pass
                expr = entry_param.get("expression") if isinstance(entry_param, dict) else None
                if expr:
                    try:
                        feat.setExpression(dst, expr)
                    except Exception:
                        pass
        self._active_body.addObject(feat)
        self._feature_token_to_obj[entry.get("entity_token")] = feat
        self._register_object(entry.get("entity_token"), feat, kind)

    # ------------------------------------------------------------------
    # Construction geometry
    # ------------------------------------------------------------------

    def _attach_to_origin(self, datum: Any) -> None:
        """Default-attach a datum to the body's origin."""
        try:
            datum.AttachmentSupport = (self._active_body.Origin, [""])
            datum.MapMode = "ObjectXY"
            self._active_body.addObject(datum)
        except Exception:
            self._doc.addObject(datum.TypeId, datum.Name)

    def _build_construction_plane(self, entry: dict, name: str) -> None:
        info = entry.get("construction") or {}
        datum = self._doc.addObject("Part::DatumPlane", name)
        origin = info.get("origin")
        normal = info.get("normal")
        u_axis = info.get("u_axis")
        if origin and normal and u_axis:
            try:
                placement = self._make_placement(origin, normal, u_axis)
                datum.Placement = placement
            except Exception:
                pass
        self._attach_to_origin(datum)

    def _build_construction_axis(self, entry: dict, name: str) -> None:
        info = entry.get("construction") or {}
        datum = self._doc.addObject("Part::DatumLine", name)
        origin = info.get("origin")
        direction = info.get("direction")
        if origin and direction:
            try:
                datum.Placement = self._make_placement(
                    origin, direction, [1, 0, 0]
                )
            except Exception:
                pass
        self._attach_to_origin(datum)

    def _build_construction_point(self, entry: dict, name: str) -> None:
        info = entry.get("construction") or {}
        datum = self._doc.addObject("Part::DatumPoint", name)
        pt = info.get("point")
        if pt:
            try:
                datum.Placement.Base = (pt[0] or 0.0, pt[1] or 0.0, pt[2] or 0.0)
            except Exception:
                pass
        self._attach_to_origin(datum)

    @staticmethod
    def _make_placement(origin, normal, x_axis):
        """Build a FreeCAD Placement from origin + normal + x_axis."""
        import FreeCAD
        return FreeCAD.Placement(
            FreeCAD.Vector(*origin),
            FreeCAD.Rotation(
                FreeCAD.Vector(*x_axis),
                FreeCAD.Vector(*normal),
            ),
        )

    # ------------------------------------------------------------------
    # Occurrences (assembly instances)
    # ------------------------------------------------------------------

    def _build_occurrence(self, entry: dict, name: str, history: Any) -> None:
        info = entry.get("occurrence") or {}
        # Create a new Body for each occurrence (top-level only).
        occ_body = self._doc.addObject("PartDesign::Body", f"Occ_{name}")
        try:
            tr = info.get("transform")
            if tr and len(tr) == 12:
                import FreeCAD
                m = FreeCAD.Matrix(
                    tr[0], tr[1], tr[2], tr[3],
                    tr[4], tr[5], tr[6], tr[7],
                    tr[8], tr[9], tr[10], tr[11],
                )
                occ_body.Placement.Base = m.translation()
                occ_body.Placement.Rotation = FreeCAD.Rotation(m.linear())
        except Exception as exc:
            self.result.warnings.append(
                f"Occurrence '{name}' transform could not be applied: {exc}"
            )

        # Walk child component for features.
        comp_token = info.get("component_token")
        for root in [history.root_component] + self._collect_descendants(history.root_component):
            if root.get("token") == comp_token:
                self._populate_body_from_component(occ_body, root, history)
                break
        self._register_object(info.get("token"), occ_body, "occurrence")
        self._body_token_to_obj[comp_token] = occ_body

    def _collect_descendants(self, comp: dict) -> list[dict]:
        out = []
        for child in comp.get("children", []) or []:
            nested = child.get("child") or {}
            out.append(nested)
            out.extend(self._collect_descendants(nested))
        return out

    def _populate_body_from_component(
        self, body: Any, comp: dict, history: Any
    ) -> None:
        """Re-create sketches + features belonging to a component."""
        # Sketches and features inside this component are *not* in the
        # global timeline because they belong to a child component. We
        # leave the body empty in that case and let the user re-model
        # the part manually. This is a known limitation: assemblies with
        # parametric child parts cannot be fully reconstructed from
        # F3D alone - the F3D's per-component timeline is collapsed.
        self.result.warnings.append(
            f"Component '{comp.get('name', '?')}' inside occurrence was not "
            f"fully reconstructed; rebuild the part manually."
        )

    # ------------------------------------------------------------------
    # STEP reference shape
    # ------------------------------------------------------------------

    def _maybe_import_step(
        self,
        doc: Any,
        body: Any,
        history: Any,
        report: Any,
    ) -> None:
        """If the STEP file is available, create a reference shape."""
        step = getattr(history, "step_path", None)
        if not step:
            return
        try:
            import Part  # local import: may be heavy
            shape = Part.Shape()
            shape.read(step)
            report("Importing STEP geometry as verification shape...", 0.95)
            ref = doc.addObject("Part::Feature", f"{body.Name}_Reference")
            ref.Shape = shape
        except Exception as exc:
            self.result.warnings.append(
                f"Could not import STEP for verification: {exc}"
            )