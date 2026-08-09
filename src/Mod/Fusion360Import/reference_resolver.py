# SPDX-License-Identifier: LGPL-2.1-or-later
"""Resolve Fusion 360 entity references to FreeCAD sub-element names.

Fusion 360's API exposes every entity (face, edge, vertex, body, ...) via
an opaque *entity token* (a UUID-like string). Tungsten CAD exposes the
same entities via positional names like ``Face1``, ``Edge3``, ``Vertex2``.
We cannot reverse-engineer Fusion's tokens, but we *can* use the
index we captured during extraction to produce a matching
sub-element name on the FreeCAD side.

The mapping is straightforward:

- The extractor walks ``body.faces`` in Fusion's order and assigns
  index ``0, 1, 2, ...``. The rebuilder does the same on the
  reconstructed body. ``Face<i>`` is then a stable name.
- The same logic applies to edges, vertices, axes, planes and
  bodies themselves.

This module also owns the per-feature lookups ("get the face that this
Fillet's edge list refers to") so the feature rebuilder can stay
focused on the high-level dispatch.
"""

from __future__ import annotations

from typing import Any


def face_name(index: int) -> str:
    """Return ``Face<index+1>`` for a 0-based face index."""
    return f"Face{index + 1}"


def edge_name(index: int) -> str:
    """Return ``Edge<index+1>`` for a 0-based edge index."""
    return f"Edge{index + 1}"


def vertex_name(index: int) -> str:
    """Return ``Vertex<index+1>`` for a 0-based vertex index."""
    return f"Vertex{index + 1}"


def build_body_index(body: Any) -> dict[str, dict[str, str]]:
    """Return a mapping ``{token: {"face": "Face1", ...}}`` for a body.

    The result can be looked up by token (Fusion's opaque ID) and gives
    the corresponding FreeCAD sub-element name on the body. If the body
    has no faces, edges or vertices, the corresponding dict is empty.
    """
    out: dict[str, dict[str, str]] = {}

    def _walk_face(faces, key):
        for i in range(faces.count):
            ent = faces.item(i)
            try:
                tok = ent.entityToken
            except Exception:
                tok = None
            if tok:
                out.setdefault(tok, {})[key] = face_name(i)

    def _walk_edge(edges, key):
        for i in range(edges.count):
            ent = edges.item(i)
            try:
                tok = ent.entityToken
            except Exception:
                tok = None
            if tok:
                out.setdefault(tok, {})[key] = edge_name(i)

    def _walk_vertex(vertices, key):
        for i in range(vertices.count):
            ent = vertices.item(i)
            try:
                tok = ent.entityToken
            except Exception:
                tok = None
            if tok:
                out.setdefault(tok, {})[key] = vertex_name(i)

    try:
        _walk_face(body.faces, "face")
    except Exception:
        pass
    try:
        _walk_edge(body.edges, "edge")
    except Exception:
        pass
    try:
        _walk_vertex(body.vertices, "vertex")
    except Exception:
        pass
    return out


def build_global_index(
    doc: Any,
) -> dict[str, dict[str, Any]]:
    """Return a mapping ``token -> {"object": <doc_object>, "kind": str}``.

    The rebuilder uses this to translate a Fusion token into the
    FreeCAD object it should link to. Bodies are indexed by their
    token, faces/edges/vertices by their body's index plus position.
    """
    out: dict[str, dict[str, Any]] = {}

    def _scan(obj: Any) -> None:
        try:
            token = obj.entityToken
        except Exception:
            token = None
        if token:
            out.setdefault(token, {"object": obj, "kind": obj.TypeId})
        # If it's a body, also index its sub-elements.
        try:
            sub = build_body_index(obj)
            for tok, mapping in sub.items():
                entry = out.setdefault(tok, {"object": obj, "kind": obj.TypeId})
                entry.update(mapping)
        except Exception:
            pass

    for obj in doc.Objects:
        _scan(obj)
    return out


def resolve_link_sub(
    fusion_ref: dict | None,
    global_index: dict[str, dict[str, Any]],
) -> tuple[Any | None, list[str]]:
    """Translate a Fusion reference (``{token, ...}``) into a
    ``(FreeCAD_object, sub_names)`` pair suitable for a
    ``PropertyLinkSub`` assignment.

    The fusion_ref is typically::

        {"index": 3, "token": "uuid-string"}

    We look up the token in ``global_index`` to find the parent
    object, then format the sub-element name based on the kind
    stored in the index entry.
    """
    if not fusion_ref:
        return None, []
    token = fusion_ref.get("token")
    if not token:
        return None, []
    entry = global_index.get(token)
    if not entry:
        return None, []
    obj = entry.get("object")
    if obj is None:
        return None, []

    # Try face / edge / vertex in priority order.
    for kind, formatter in (
        ("face", lambda i: face_name(i)),
        ("edge", lambda i: edge_name(i)),
        ("vertex", lambda i: vertex_name(i)),
    ):
        sub = entry.get(kind)
        if sub:
            return obj, [sub]
    return obj, [""]


def resolve_sub_list(
    fusion_refs: list[dict] | None,
    global_index: dict[str, dict[str, Any]],
) -> list[tuple[Any, list[str]]]:
    """Translate a list of Fusion refs into a list of (object, subnames)."""
    out: list[tuple[Any, list[str]]] = []
    for ref in fusion_refs or []:
        obj, subs = resolve_link_sub(ref, global_index)
        if obj is not None:
            out.append((obj, subs))
    return out


def resolve_token_to_subname(
    token: str,
    kind: str,
    index: int,
) -> str:
    """Convenience: format a sub-name for the given kind and index."""
    if kind == "face":
        return face_name(index)
    if kind == "edge":
        return edge_name(index)
    if kind == "vertex":
        return vertex_name(index)
    return f"{kind.capitalize()}{index + 1}"