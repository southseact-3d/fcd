# SPDX-License-Identifier: LGPL-2.1-or-later
"""Tests for the Fusion360Import workbench.

Tests that need FreeCAD are guarded; the rest can run with plain
Python. Run with ``python -m unittest`` from this directory or
``pytest`` if available.
"""

import json
import unittest
from unittest import mock


class TestFusionMCPClient(unittest.TestCase):
    """Test the protocol formatting of the TCP client."""

    def _client(self):
        from Fusion360Import import fusion360_mcp_client
        return fusion360_mcp_client.FusionMCPClient(host="127.0.0.1", port=9876)

    def test_request_format(self):
        client = self._client()
        fake = mock.MagicMock()
        sent_payload = {}

        def capture_sendall(data):
            sent_payload["data"] = data
            import json as _json
            req = _json.loads(data.decode("utf-8").strip())
            fake.recv.side_effect = None
            fake.recv.return_value = (
                _json.dumps({"id": req["id"], "result": "pong"}) + "\n"
            ).encode("utf-8")

        fake.sendall.side_effect = capture_sendall
        client._sock = fake

        result = client.call_tool("ping", {})

        self.assertTrue(sent_payload["data"].endswith(b"\n"))
        payload = json.loads(sent_payload["data"].decode("utf-8").strip())
        self.assertEqual(payload["method"], "ping")
        self.assertIn("id", payload)
        self.assertEqual(result, "pong")

    def test_error_propagates(self):
        client = self._client()
        fake = mock.MagicMock()
        fake.recv.return_value = (
            b'{"id": "abc", "error": {"message": "boom"}}\n'
        )
        client._sock = fake

        from Fusion360Import.fusion360_mcp_client import FusionMCPError
        with self.assertRaises(FusionMCPError):
            client.call_tool("ping", {})

    def test_connection_failure(self):
        from Fusion360Import.fusion360_mcp_client import (
            FusionMCPClient,
            FusionMCPConnectionError,
        )
        client = FusionMCPClient(host="255.255.255.255", port=1)
        with self.assertRaises(FusionMCPConnectionError):
            client.connect()


class TestExtractionScript(unittest.TestCase):
    """The extractor is a Python string we ship to Fusion. Verify it's valid."""

    def test_script_is_importable(self):
        from Fusion360Import import fusion360_extractor
        script = fusion360_extractor.EXTRACT_HISTORY_SCRIPT
        compile(script, "<extraction_script>", "exec")

    def test_script_emits_markers(self):
        from Fusion360Import import fusion360_extractor
        script = fusion360_extractor.EXTRACT_HISTORY_SCRIPT
        self.assertIn("__FUSION_EXTRACT_BEGIN__", script)
        self.assertIn("__FUSION_EXTRACT_END__", script)
        self.assertIn("json.dumps", script)

    def test_script_covers_all_features(self):
        """Every Fusion 360 feature class name should appear at least once."""
        from Fusion360Import import fusion360_extractor
        script = fusion360_extractor.EXTRACT_HISTORY_SCRIPT
        must_have = [
            "ExtrudeFeature",
            "RevolveFeature",
            "LoftFeature",
            "SweepFeature",
            "CoilFeature",
            "FilletFeature",
            "ChamferFeature",
            "DraftFeature",
            "ShellFeature",
            "OffsetFacesFeature",
            "HoleFeature",
            "ThreadFeature",
            "RectangularPatternFeature",
            "CircularPatternFeature",
            "PathPatternFeature",
            "MirrorFeature",
            "BooleanFeature",
            "BoxFeature",
            "CylinderFeature",
            "SphereFeature",
            "TorusFeature",
            "ConeFeature",
            "ScaleFeature",
            "ReplaceFaceFeature",
            "ConstructionPlane",
            "ConstructionAxis",
            "ConstructionPoint",
            "Occurrence",
        ]
        for cls in must_have:
            self.assertIn(cls, script, f"Missing handler for {cls}")

    def test_schema_constant_is_string(self):
        from Fusion360Import import fusion360_extractor
        self.assertIsInstance(fusion360_extractor.HISTORY_SCHEMA, str)
        self.assertIn("schema", fusion360_extractor.HISTORY_SCHEMA)


class TestParseOutput(unittest.TestCase):
    """Test that the orchestrator parses the Fusion response correctly."""

    def test_parses_fenced_payload(self):
        from Fusion360Import.extractor import FusionExtractor
        output = (
            "some noise\n"
            + "__FUSION_EXTRACT_BEGIN__\n"
            + json.dumps({"document": "X", "timeline": []})
            + "\n__FUSION_EXTRACT_END__\n"
        )
        parsed = FusionExtractor._parse_output(output)
        self.assertEqual(parsed["document"], "X")
        self.assertEqual(parsed["timeline"], [])

    def test_falls_back_to_last_json_object(self):
        from Fusion360Import.extractor import FusionExtractor
        output = (
            "noise\n" + json.dumps({"hello": "world"}) + "\n"
        )
        parsed = FusionExtractor._parse_output(output)
        self.assertEqual(parsed, {"hello": "world"})


class TestRebuilderDispatch(unittest.TestCase):
    """Verify the dispatcher routes feature kinds to the right builder."""

    def setUp(self):
        # These tests need FreeCAD; skip if not available.
        try:
            import FreeCAD  # noqa: F401
        except ImportError:
            self.skipTest("FreeCAD not available")

    def test_dispatch_unknown_skipped(self):
        from Fusion360Import.feature_rebuilder import FeatureRebuilder
        rb = FeatureRebuilder()
        # We need an active body before processing entries. Stub it.
        rb._active_body = mock.MagicMock()
        rb._doc = mock.MagicMock()
        rb._process_entry(
            {"feature_kind": "weird_thing", "name": "X", "entity_token": "tok"},
            mock.MagicMock(),
            "X",
        )
        self.assertTrue(any("weird_thing" in s for s in rb.result.skipped))

    def test_dispatch_construction_plane_creates_datum(self):
        from Fusion360Import.feature_rebuilder import FeatureRebuilder
        rb = FeatureRebuilder()
        rb._active_body = mock.MagicMock()
        rb._active_body.Origin = "OriginObj"
        rb._doc = mock.MagicMock()
        rb._doc.addObject.return_value = mock.MagicMock(TypeId="Part::DatumPlane")
        entry = {
            "feature_kind": "construction_plane",
            "name": "Plane1",
            "entity_token": "tok1",
            "construction": {
                "origin": [0, 0, 0],
                "normal": [0, 0, 1],
                "u_axis": [1, 0, 0],
            },
        }
        rb._process_entry(entry, mock.MagicMock(), "Plane1")
        rb._doc.addObject.assert_called_once()
        # Construction count was incremented.
        self.assertEqual(rb.result.construction_count, 1)


class TestReferenceResolver(unittest.TestCase):
    """Test sub-name formatting and token -> FreeCAD lookup."""

    def test_face_name_is_one_indexed(self):
        from Fusion360Import.reference_resolver import face_name, edge_name, vertex_name
        self.assertEqual(face_name(0), "Face1")
        self.assertEqual(face_name(2), "Face3")
        self.assertEqual(edge_name(0), "Edge1")
        self.assertEqual(vertex_name(5), "Vertex6")

    def test_resolve_returns_empty_for_no_token(self):
        from Fusion360Import.reference_resolver import resolve_link_sub
        obj, subs = resolve_link_sub(None, {})
        self.assertIsNone(obj)
        self.assertEqual(subs, [])

    def test_resolve_uses_global_index(self):
        from Fusion360Import.reference_resolver import resolve_link_sub
        index = {
            "tok1": {"object": "fake_obj", "kind": "body", "face": "Face3"}
        }
        obj, subs = resolve_link_sub({"token": "tok1"}, index)
        self.assertEqual(obj, "fake_obj")
        self.assertEqual(subs, ["Face3"])

    def test_resolve_falls_back_to_object_only(self):
        from Fusion360Import.reference_resolver import resolve_link_sub
        index = {"tok1": {"object": "fake_obj", "kind": "body"}}
        obj, subs = resolve_link_sub({"token": "tok1"}, index)
        self.assertEqual(obj, "fake_obj")
        self.assertEqual(subs, [""])


class TestExtractorHistory(unittest.TestCase):
    """Test FusionDesignHistory helper methods."""

    def test_features_filters_out_non_features(self):
        from Fusion360Import.extractor import FusionDesignHistory
        h = FusionDesignHistory(
            document="x",
            design_name="x",
            design_type="parametric",
            units="mm",
            root_component={},
            parameters={"user": [], "model": []},
            timeline=[
                {"feature_kind": "sketch"},
                {"feature_kind": "extrude"},
                {"feature_kind": "construction_plane"},
                {"feature_kind": "fillet"},
                {"feature_kind": "occurrence"},
            ],
        )
        feats = h.features()
        kinds = [f["feature_kind"] for f in feats]
        self.assertEqual(kinds, ["extrude", "fillet"])

    def test_user_and_model_params_properties(self):
        from Fusion360Import.extractor import FusionDesignHistory
        h = FusionDesignHistory(
            document="x",
            design_name="x",
            design_type="parametric",
            units="mm",
            root_component={},
            parameters={
                "user": [{"name": "length"}],
                "model": [{"name": "width"}],
            },
            timeline=[],
        )
        self.assertEqual(len(h.user_parameters), 1)
        self.assertEqual(len(h.model_parameters), 1)


if __name__ == "__main__":
    unittest.main()