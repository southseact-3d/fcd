# SPDX-License-Identifier: LGPL-2.1-or-later
"""High-level wrapper that fetches Fusion 360 design history.

This module is the public entry point used by the GUI commands. It:

1. Opens a connection to the Fusion 360 MCP add-in via
   :class:`fusion360_mcp_client.FusionMCPClient`.
2. Sends :data:`EXTRACT_HISTORY_SCRIPT` (from
   :mod:`fusion360_extractor`) for execution inside Fusion 360.
3. Parses the JSON response.
4. Copies the exported STEP file into a stable location on the
   client machine.
5. Returns a :class:`FusionDesignHistory` dataclass that the feature
   rebuilder can consume.

The class also exposes a :meth:`progress` callback so the GUI can show
a progress dialog while the script runs.
"""

from __future__ import annotations

import json
import os
import shutil
import tempfile
from dataclasses import dataclass, field
from typing import Any, Callable

from . import fusion360_extractor
from .fusion360_mcp_client import (
    DEFAULT_HOST,
    DEFAULT_PORT,
    FusionMCPClient,
    FusionMCPConnectionError,
    FusionMCPError,
)


ProgressCallback = Callable[[str, float], None]
"""Called with (status_message, fraction_in_0_to_1)."""


@dataclass
class FusionDesignHistory:
    """A complete snapshot of a Fusion 360 design's history."""

    document: str
    design_name: str
    design_type: str
    units: str
    root_component: dict
    parameters: dict
    timeline: list
    step_path: str | None = None
    raw: dict = field(default_factory=dict)

    @property
    def user_parameters(self) -> list[dict]:
        return list(self.parameters.get("user", []))

    @property
    def model_parameters(self) -> list[dict]:
        return list(self.parameters.get("model", []))

    def features(self) -> list[dict]:
        """All timeline entries that represent parametric features.

        Sketches, construction geometry, components and groups are
        excluded; callers that need them should inspect ``timeline``
        directly.
        """
        skip = {"sketch", "construction_plane", "construction_axis",
                "construction_point", "occurrence", "group"}
        return [
            e for e in self.timeline
            if e.get("feature_kind") not in skip
        ]


class FusionExtractionError(RuntimeError):
    """Raised when the extract script returns an error object."""


class FusionExtractor:
    """Orchestrates an end-to-end extraction round-trip."""

    def __init__(
        self,
        host: str = DEFAULT_HOST,
        port: int = DEFAULT_PORT,
        timeout: float = 60.0,
    ) -> None:
        self.client = FusionMCPClient(host=host, port=port, timeout=timeout)
        self._last_step_path: str | None = None

    def is_connected(self) -> bool:
        """Return True if the add-in can be reached and responds to ping."""
        try:
            with self.client as c:
                return c.ping()
        except (FusionMCPConnectionError, FusionMCPError):
            return False

    def extract(
        self,
        progress: ProgressCallback | None = None,
    ) -> FusionDesignHistory:
        """Connect, extract history, copy STEP, return parsed result."""
        report = (lambda msg, frac: progress(msg, frac)) if progress else (lambda *_a, **_kw: None)

        report("Connecting to Fusion 360 MCP add-in...", 0.05)
        self.client.connect()
        report("Sending extraction script...", 0.15)

        try:
            raw_output = self.client.execute_script(
                fusion360_extractor.EXTRACT_HISTORY_SCRIPT
            )
        except (FusionMCPConnectionError, FusionMCPError) as exc:
            raise FusionExtractionError(str(exc)) from exc

        report("Parsing extraction output...", 0.7)
        data = self._parse_output(raw_output)

        if "error" in data:
            raise FusionExtractionError(
                f"Fusion 360 extraction failed: {data.get('message', data['error'])}\n"
                f"{data.get('traceback', '')}"
            )

        report("Copying STEP file...", 0.85)
        step_path = self._materialise_step(data)

        report("Done", 1.0)

        return FusionDesignHistory(
            document=str(data.get("document", "")),
            design_name=str(data.get("design_name", "")),
            design_type=str(data.get("design_type", "unknown")),
            units=str(data.get("units", "mm")),
            root_component=dict(data.get("root_component", {})),
            parameters=dict(data.get("parameters", {"user": [], "model": []})),
            timeline=list(data.get("timeline", [])),
            step_path=step_path,
            raw=data,
        )

    @staticmethod
    def _parse_output(output: str) -> dict:
        """The script emits a fenced JSON block delimited by markers."""
        begin = "__FUSION_EXTRACT_BEGIN__"
        end = "__FUSION_EXTRACT_END__"
        idx_begin = output.find(begin)
        idx_end = output.find(end)
        if idx_begin != -1 and idx_end != -1 and idx_end > idx_begin:
            payload = output[idx_begin + len(begin):idx_end].strip()
        else:
            # Fall back to the last JSON object on stdout.
            payload = output.strip().splitlines()[-1] if output.strip() else "{}"
        try:
            return json.loads(payload)
        except json.JSONDecodeError as exc:
            raise FusionExtractionError(
                f"Fusion 360 returned malformed JSON: {exc}\n"
                f"Payload: {payload[:500]}"
            ) from exc

    def _materialise_step(self, data: dict) -> str | None:
        """Move the STEP file Fusion exported to a stable location."""
        export_info = data.get("step_export") or {}
        src = export_info.get("path")
        if not src or not os.path.exists(src):
            return None
        # Use a deterministic-ish filename based on the document name.
        doc_name = data.get("document") or "fusion_export"
        safe = "".join(c for c in doc_name if c.isalnum() or c in "-_.")
        if not safe:
            safe = "fusion_export"
        dest_dir = os.path.join(tempfile.gettempdir(), "Fusion360Import")
        os.makedirs(dest_dir, exist_ok=True)
        dest = os.path.join(dest_dir, f"{safe}.step")
        try:
            shutil.copy2(src, dest)
            self._last_step_path = dest
            return dest
        except OSError:
            # If we can't copy, just return the original path so the
            # caller can still try to read it.
            return src