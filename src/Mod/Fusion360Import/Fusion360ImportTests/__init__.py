# SPDX-License-Identifier: LGPL-2.1-or-later
"""Tests for the Fusion360Import workbench.

The tests are deliberately tiny - they don't actually open a Fusion
360 connection (which would require a live Fusion instance) but they
verify the wiring between the modules: serialisation of Fusion JSON,
sketch geometry building, feature tree walking.

Run with ``python -m unittest`` from this directory.
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
        import re
        client = self._client()
        fake = mock.MagicMock()
        # The test must echo the request id so the client accepts it.
        fake.recv.side_effect = [b"", b"", b""]  # force _recv_line loop termination

        sent_payload = {}

        def capture_sendall(data):
            sent_payload["data"] = data
            # build a matching response
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
        # It should at least parse as Python - so compile() will work.
        compile(script, "<extraction_script>", "exec")

    def test_script_emits_markers(self):
        from Fusion360Import import fusion360_extractor
        script = fusion360_extractor.EXTRACT_HISTORY_SCRIPT
        self.assertIn("__FUSION_EXTRACT_BEGIN__", script)
        self.assertIn("__FUSION_EXTRACT_END__", script)
        self.assertIn("json.dumps", script)


class TestRebuilder(unittest.TestCase):
    """Test that the rebuilder walks the timeline correctly with a stub."""

    def test_unknown_features_are_skipped(self):
        from Fusion360Import.feature_rebuilder import FeatureRebuilder

        history = mock.MagicMock()
        history.timeline = [
            {"feature_kind": "rect_pattern", "name": "P1",
             "entity_token": "tok1"},
            {"feature_kind": "mirror", "name": "M1",
             "entity_token": "tok2"},
        ]
        doc = mock.MagicMock()

        rebuilder = FeatureRebuilder()
        body = rebuilder.rebuild(doc, history, body_name="TestBody")

        self.assertEqual(len(rebuilder.result.skipped), 2)
        doc.openTransaction.assert_called_once()
        doc.commitTransaction.assert_called_once()
        doc.removeObject.assert_not_called()


class TestParseOutput(unittest.TestCase):
    """Test that the orchestrator parses the Fusion response correctly."""

    def test_parses_fenced_payload(self):
        from Fusion360Import.extractor import FusionExtractor
        output = (
            "some noise\n"
            "__FUSION_EXTRACT_BEGIN__\n"
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


if __name__ == "__main__":
    unittest.main()