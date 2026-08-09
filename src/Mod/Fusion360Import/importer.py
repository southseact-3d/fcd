# SPDX-License-Identifier: LGPL-2.1-or-later
"""Top-level orchestrator used by the GUI commands.

The flow is:

    FusionExtractor.extract()     -> FusionDesignHistory
    FeatureRebuilder.rebuild()    -> PartDesign::Body
    RebuildResult                 -> user-facing summary

This module is intentionally thin so the GUI code can swap any of
the three layers for a mock during testing.
"""

from __future__ import annotations

from typing import Any, Callable

from .extractor import FusionDesignHistory, FusionExtractor, FusionExtractionError
from .feature_rebuilder import FeatureRebuilder, RebuildResult


ProgressCallback = Callable[[str, float], None]


def import_design_from_fusion(
    doc: Any,
    host: str = "127.0.0.1",
    port: int = 9876,
    body_name: str | None = None,
    progress: ProgressCallback | None = None,
) -> tuple[Any, FusionDesignHistory, RebuildResult]:
    """Run the full extract-then-rebuild pipeline.

    Parameters
    ----------
    doc:
        An active ``FreeCAD.Document`` to populate.
    host, port:
        Network endpoint of the Fusion 360 MCP add-in.
    body_name:
        Optional name for the resulting ``PartDesign::Body``.
    progress:
        Optional ``callable(message: str, fraction: float)``.

    Returns
    -------
    (body, history, result):
        ``body`` is the new PartDesign::Body, ``history`` is the raw
        extracted data, ``result`` summarises what was reconstructed.
    """
    extractor = FusionExtractor(host=host, port=port)
    history = extractor.extract(progress=progress)

    rebuilder = FeatureRebuilder()
    body = rebuilder.rebuild(doc, history, body_name=body_name, progress=progress)

    return body, history, rebuilder.result


def import_design_from_history(
    doc: Any,
    history: FusionDesignHistory,
    body_name: str | None = None,
    progress: ProgressCallback | None = None,
) -> tuple[Any, RebuildResult]:
    """Rebuild from a previously-extracted history (no Fusion needed)."""
    rebuilder = FeatureRebuilder()
    body = rebuilder.rebuild(doc, history, body_name=body_name, progress=progress)
    return body, rebuilder.result


__all__ = [
    "FusionExtractor",
    "FusionDesignHistory",
    "FusionExtractionError",
    "FeatureRebuilder",
    "RebuildResult",
    "import_design_from_fusion",
    "import_design_from_history",
]