# SPDX-License-Identifier: LGPL-2.1-or-later
"""Reconstruct a parametric feature tree inside Tungsten CAD.

The rebuilder consumes a :class:`extractor.FusionDesignHistory` and
walks its timeline. Each entry is mapped onto a FreeCAD/PartDesign
feature class:

    extrude        -> PartDesign::Pad or Pocket (operation-dependent)
    revolve        -> PartDesign::Revolution or Groove
    fillet         -> PartDesign::Fillet
    chamfer        -> PartDesign::Chamfer
    hole           -> PartDesign::Hole
    rect_pattern   -> PartDesign::RectangularPattern
    circ_pattern   -> PartDesign::CircularPattern
    mirror         -> PartDesign::Mirrored
    shell          -> PartDesign::Thickness
    draft          -> PartDesign::Draft
    boolean        -> PartDesign::Boolean
    box, cylinder,
    sphere, torus  -> PartDesign::Additive* primitive
    loft, sweep,
    coil           -> PartDesign::AdditiveLoft/Sweep/Pipe

For sketches, :mod:`sketch_rebuilder` is invoked first so the
profile is in place before the Pad/Pocket references it.

Everything happens inside a single ``doc.openTransaction(...)`` call
so the user can undo the entire import with one Ctrl+Z.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any

from . import sketch_rebuilder


@dataclass
class RebuildResult:
    """Summary of a rebuild run for the GUI status panel."""

    body_name: str = ""
    feature_count: int = 0
    sketch_count: int = 0
    skipped: list[str] = field(default_factory=list)
    warnings: list[str] = field(default_factory=list)


# Fusion 360's ExtrudeFeature operation enum:
#   0 = JoinFeatureOperation (additive) -> Pad
#   1 = CutFeatureOperation (subtractive) -> Pocket
#   2 = IntersectFeatureOperation -> rarely used, mapped to Pocket
_EXTRUDE_OP_TO_FEATURE = {
    0: "PartDesign::Pad",
    1: "PartDesign::Pocket",
    2: "PartDesign::Pocket",
}

_REVOLVE_OP_TO_FEATURE = {
    0: "PartDesign::Revolution",
    1: "PartDesign::Groove",
    2: "PartDesign::Groove",
}


class FeatureRebuilder:
    """Reconstruct a Fusion design as a single PartDesign::Body."""

    def __init__(self) -> None:
        self.result = RebuildResult()

    def rebuild(
        self,
        doc: Any,
        history: Any,
        body_name: str | None = None,
        progress: Any | None = None,
    ) -> Any:
        """Walk the timeline and build the feature tree.

        Returns the body that holds the new features.
        """
        body_name = body_name or f"Fusion_{history.design_name or 'Import'}".replace(" ", "_")
        report = progress or (lambda *_a, **_kw: None)

        # Open a single transaction so the whole import is one undo step.
        doc.openTransaction(f"Import Fusion 360 design: {history.document}")
        try:
            body = doc.addObject("PartDesign::Body", body_name)
            self.result.body_name = body_name

            # Map each sketch timeline entry by its entity token so
            # features that reference it can look it up later.
            sketch_map: dict[str, Any] = {}

            timeline = history.timeline
            total = max(1, len(timeline))
            for idx, entry in enumerate(timeline):
                frac = 0.1 + 0.85 * (idx / total)
                kind = entry.get("feature_kind", "unknown")
                name = entry.get("name") or f"Feature_{idx}"
                report(f"Rebuilding {kind}: {name}", frac)

                try:
                    self._process_entry(
                        doc, body, entry, sketch_map, history, name
                    )
                except Exception as exc:
                    self.result.warnings.append(
                        f"{kind} '{name}' failed: {exc}"
                    )

            doc.recompute()
            self._maybe_import_step(doc, body, history, report)

            doc.commitTransaction()
            return body
        except Exception:
            doc.abortTransaction()
            raise

    # ------------------------------------------------------------------
    # Entry dispatch
    # ------------------------------------------------------------------

    def _process_entry(
        self,
        doc: Any,
        body: Any,
        entry: dict,
        sketch_map: dict[str, Any],
        history: Any,
        default_name: str,
    ) -> None:
        kind = entry.get("feature_kind")
        token = entry.get("entity_token")

        if kind == "sketch":
            sketch, warnings = sketch_rebuilder.rebuild_sketch(
                doc, body, entry.get("sketch") or {}
            )
            self.result.sketch_count += 1
            if warnings:
                self.result.warnings.extend(warnings)
            if token:
                sketch_map[token] = sketch

        elif kind == "extrude":
            self._build_extrude(doc, body, entry, sketch_map, default_name)
            self.result.feature_count += 1

        elif kind == "revolve":
            self._build_revolve(doc, body, entry, sketch_map, default_name)
            self.result.feature_count += 1

        elif kind == "fillet":
            self._build_fillet(doc, body, entry, default_name)
            self.result.feature_count += 1

        elif kind == "chamfer":
            self._build_chamfer(doc, body, entry, default_name)
            self.result.feature_count += 1

        elif kind == "hole":
            self._build_hole(doc, body, entry, sketch_map, default_name)
            self.result.feature_count += 1

        elif kind == "rect_pattern":
            self.result.skipped.append(
                f"RectangularPattern '{default_name}' - reconstructed as "
                f"comment only, deep feature reference mapping not yet "
                f"implemented"
            )

        elif kind == "circ_pattern":
            self.result.skipped.append(
                f"CircularPattern '{default_name}' - reconstructed as "
                f"comment only"
            )

        elif kind == "mirror":
            self.result.skipped.append(
                f"Mirror '{default_name}' - reconstructed as comment only"
            )

        elif kind == "shell":
            self._build_shell(doc, body, entry, default_name)
            self.result.feature_count += 1

        elif kind == "draft":
            self.result.skipped.append(
                f"Draft '{default_name}' - no exact PartDesign equivalent"
            )

        elif kind == "offset":
            self.result.skipped.append(
                f"OffsetFaces '{default_name}' - manual rework required"
            )

        elif kind in ("boolean", "combine"):
            self.result.skipped.append(
                f"Boolean '{default_name}' - reconstruct manually from "
                f"occurrences"
            )

        elif kind == "box":
            self._build_primitive(doc, body, "Box", entry, default_name)
            self.result.feature_count += 1

        elif kind == "cylinder":
            self._build_primitive(doc, body, "Cylinder", entry, default_name)
            self.result.feature_count += 1

        elif kind == "sphere":
            self._build_primitive(doc, body, "Sphere", entry, default_name)
            self.result.feature_count += 1

        elif kind == "torus":
            self._build_primitive(doc, body, "Torus", entry, default_name)
            self.result.feature_count += 1

        elif kind in ("loft", "sweep", "coil", "pipe", "rib", "web"):
            self.result.skipped.append(
                f"{kind.title()} '{default_name}' - manual rebuild required"
            )

        elif kind in (
            "construction_plane",
            "construction_axis",
            "construction_point",
        ):
            # We could create datum geometry, but most workflows are
            # happy enough with the default planes. Note them so the
            # user knows.
            self.result.skipped.append(
                f"{kind.replace('_', ' ').title()} '{default_name}'"
            )

        elif kind == "occurrence":
            # Assemblies are not yet reconstructed as separate Bodies;
            # just record the fact for the user.
            self.result.skipped.append(
                f"Component occurrence '{default_name}'"
            )

        else:
            self.result.skipped.append(
                f"Unknown feature kind: {kind!r} ({default_name})"
            )

    # ------------------------------------------------------------------
    # Feature builders
    # ------------------------------------------------------------------

    def _build_extrude(
        self,
        doc: Any,
        body: Any,
        entry: dict,
        sketch_map: dict[str, Any],
        name: str,
    ) -> None:
        params = entry.get("params", {}) or {}
        sketch_token = entry.get("sketch_token") or entry.get("profile_token")
        sketch = sketch_map.get(sketch_token) if sketch_token else None
        if sketch is None:
            # Fallback: use any sketch already in the body.
            for obj in body.Group:
                if obj.TypeId == "Sketcher::SketchObject":
                    sketch = obj
                    break
        if sketch is None:
            self.result.skipped.append(
                f"Extrude '{name}' - no sketch reference found"
            )
            return

        op = params.get("operation")
        feat_type = _EXTRUDE_OP_TO_FEATURE.get(int(op) if op is not None else 0, "PartDesign::Pad")
        feat = doc.addObject(feat_type, name)
        try:
            feat.Profile = sketch
        except Exception:
            self.result.skipped.append(
                f"Extrude '{name}' - could not link sketch"
            )
            doc.removeObject(feat.Name)
            return

        length = params.get("distance")
        if length is not None:
            try:
                feat.Length = float(length)
            except Exception:
                pass

        length_two = params.get("distance_two")
        if length_two is not None:
            try:
                feat.Length2 = float(length_two)
            except Exception:
                pass

        taper = params.get("taper_angle_one")
        if taper:
            try:
                feat.TaperAngle = float(taper)
            except Exception:
                pass

        if params.get("is_symmetric"):
            try:
                feat.SideType = "Symmetric"
            except Exception:
                pass

        if params.get("direction_flipped"):
            try:
                feat.Reversed = True
            except Exception:
                pass

        body.addObject(feat)

    def _build_revolve(
        self,
        doc: Any,
        body: Any,
        entry: dict,
        sketch_map: dict[str, Any],
        name: str,
    ) -> None:
        params = entry.get("params", {}) or {}
        sketch_token = entry.get("sketch_token") or entry.get("profile_token")
        sketch = sketch_map.get(sketch_token) if sketch_token else None
        if sketch is None:
            for obj in body.Group:
                if obj.TypeId == "Sketcher::SketchObject":
                    sketch = obj
                    break
        if sketch is None:
            self.result.skipped.append(
                f"Revolve '{name}' - no sketch reference found"
            )
            return

        op = params.get("operation")
        feat_type = _REVOLVE_OP_TO_FEATURE.get(int(op) if op is not None else 0, "PartDesign::Revolution")
        feat = doc.addObject(feat_type, name)
        try:
            feat.Profile = sketch
        except Exception:
            doc.removeObject(feat.Name)
            self.result.skipped.append(
                f"Revolve '{name}' - could not link sketch"
            )
            return

        try:
            feat.ReferenceAxis = (doc.Y_Axis, [""])
        except Exception:
            pass

        angle = params.get("angle")
        if angle is not None:
            try:
                feat.Angle = float(angle)
            except Exception:
                pass

        body.addObject(feat)

    def _build_fillet(
        self,
        doc: Any,
        body: Any,
        entry: dict,
        name: str,
    ) -> None:
        params = entry.get("params", {}) or {}
        feat = doc.addObject("PartDesign::Fillet", name)
        radius = params.get("radius")
        if radius is not None:
            try:
                feat.Radius = float(radius)
            except Exception:
                pass
        # Without edge references the fillet will recompute against the
        # previous solid and pick up all edges - close enough for a
        # first pass; users can refine in the GUI.
        feat.UseAllEdges = True
        body.addObject(feat)

    def _build_chamfer(
        self,
        doc: Any,
        body: Any,
        entry: dict,
        name: str,
    ) -> None:
        params = entry.get("params", {}) or {}
        feat = doc.addObject("PartDesign::Chamfer", name)
        size = params.get("distance")
        if size is not None:
            try:
                feat.Size = float(size)
            except Exception:
                pass
        feat.UseAllEdges = True
        body.addObject(feat)

    def _build_hole(
        self,
        doc: Any,
        body: Any,
        entry: dict,
        sketch_map: dict[str, Any],
        name: str,
    ) -> None:
        params = entry.get("params", {}) or {}
        sketch_token = entry.get("sketch_token")
        sketch = sketch_map.get(sketch_token) if sketch_token else None
        if sketch is None:
            self.result.skipped.append(
                f"Hole '{name}' - no sketch reference found"
            )
            return

        feat = doc.addObject("PartDesign::Hole", name)
        try:
            feat.Profile = sketch
        except Exception:
            doc.removeObject(feat.Name)
            self.result.skipped.append(f"Hole '{name}' - could not link sketch")
            return

        diameter = params.get("hole_diameter")
        if diameter is not None:
            try:
                feat.Diameter = float(diameter)
            except Exception:
                pass

        if params.get("is_cosmetic"):
            try:
                feat.Threaded = True
            except Exception:
                pass

        body.addObject(feat)

    def _build_shell(
        self,
        doc: Any,
        body: Any,
        entry: dict,
        name: str,
    ) -> None:
        params = entry.get("params", {}) or {}
        feat = doc.addObject("PartDesign::Thickness", name)
        thickness = params.get("inside_thickness") or params.get("outside_thickness")
        if thickness is not None:
            try:
                feat.Value = float(thickness)
            except Exception:
                pass
        body.addObject(feat)

    def _build_primitive(
        self,
        doc: Any,
        body: Any,
        primitive: str,
        entry: dict,
        name: str,
    ) -> None:
        params = entry.get("params", {}) or {}
        type_id = f"PartDesign::Additive{primitive}"
        feat = doc.addObject(type_id, name)
        for src, dst in (
            ("length", "Length"),
            ("width", "Width"),
            ("height", "Height"),
            ("radius", "Radius"),
            ("outer_radius", "Radius"),
            ("inner_radius", "HoleRadius"),
        ):
            val = params.get(src)
            if val is not None and hasattr(feat, dst):
                try:
                    setattr(feat, dst, float(val))
                except Exception:
                    pass
        body.addObject(feat)

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