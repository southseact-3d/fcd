# SPDX-License-Identifier: LGPL-2.1-or-later
"""Fusion 360 Import workbench for Tungsten CAD.

This package reconstructs Fusion 360 parametric design history as
native Tungsten CAD features. The extraction is driven by the
Fusion360MCP add-in (faust-machines/fusion360-mcp-server) running
inside Fusion 360; the rebuilder uses the standard PartDesign/Sketcher
Python API to create sketches, parameters and feature objects.

Public modules
--------------

- :mod:`fusion360_mcp_client` - TCP JSON-RPC client for the add-in.
- :mod:`fusion360_extractor` - Python script executed inside Fusion.
- :mod:`extractor`            - High-level wrapper that fetches
                                the design history.
- :mod:`sketch_rebuilder`     - Reconstructs Sketcher sketches.
- :mod:`feature_rebuilder`    - Reconstructs PartDesign features.
- :mod:`importer`             - Top-level orchestrator.
- :mod:`commands`             - GUI commands (registered via
                                :func:`commands.register_commands`).
"""

__all__ = [
    "fusion360_mcp_client",
    "fusion360_extractor",
    "extractor",
    "sketch_rebuilder",
    "feature_rebuilder",
    "reference_resolver",
    "importer",
    "commands",
    "connection_dialog",
    "results_dialog",
    "progress_dialog",
    "help_dialog",
]