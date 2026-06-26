# SPDX-License-Identifier: LGPL-2.1-or-later
"""TCP client for the Fusion 360 MCP add-in.

This client speaks the lightweight JSON-RPC protocol used by the
Fusion360MCP add-in (faust-machines/fusion360-mcp-server). The add-in
runs inside Fusion 360 and exposes a TCP socket on localhost:9876 by
default. Every request is a single line of JSON; every response is a
single line of JSON terminated by a newline.

Protocol overview
-----------------

Request:
    {"id": "<opaque>", "method": "<tool_name>", "params": { ... }}

Response (success):
    {"id": "<same>", "result": <tool_specific>}

Response (error):
    {"id": "<same>", "error": {"message": "...", "code": -1}}

The single method that matters for full-history extraction is
``execute_script``: it runs an arbitrary Python script inside the
Fusion 360 main thread and returns whatever the script writes to a
captured buffer as JSON text. All the other tools (``get_scene_info``,
``list_components``, ``get_object_info``, ...) are thin convenience
wrappers around this same capability.

A typical extraction script looks like:

    import adsk.core, adsk.fusion, json
    app = adsk.core.Application.get()
    design = adsk.fusion.Design.cast(app.activeProduct)
    # ... walk design.timeline ...
    print(json.dumps(result))

The client captures everything printed by the script and returns it
as a string. The caller is responsible for JSON-decoding.
"""

from __future__ import annotations

import json
import socket
import time
import uuid
from dataclasses import dataclass, field
from typing import Any


DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 9876
CONNECT_TIMEOUT = 5.0
RECV_TIMEOUT = 60.0
RECV_CHUNK = 65536


class FusionMCPError(RuntimeError):
    """Raised when the Fusion 360 MCP add-in returns an error response."""


class FusionMCPConnectionError(ConnectionError):
    """Raised when the TCP connection to the add-in cannot be established."""


@dataclass
class FusionMCPClient:
    """Thin synchronous client for the Fusion 360 MCP add-in.

    Parameters
    ----------
    host, port:
        Network endpoint of the Fusion 360 MCP add-in. Defaults match
        the faust-machines add-in.
    timeout:
        Socket timeout for individual recv() calls. Extraction scripts
        can take several seconds on complex designs; bump this when
        importing assemblies.
    """

    host: str = DEFAULT_HOST
    port: int = DEFAULT_PORT
    timeout: float = RECV_TIMEOUT
    _sock: socket.socket | None = field(default=None, init=False, repr=False)

    def connect(self) -> None:
        """Open the TCP connection. Raises FusionMCPConnectionError."""
        if self._sock is not None:
            return
        try:
            sock = socket.create_connection(
                (self.host, self.port), timeout=CONNECT_TIMEOUT
            )
            sock.settimeout(self.timeout)
            self._sock = sock
        except OSError as exc:
            raise FusionMCPConnectionError(
                f"Cannot connect to Fusion 360 MCP add-in at "
                f"{self.host}:{self.port}: {exc}. "
                f"Make sure Fusion 360 is running with the Fusion360MCP "
                f"add-in enabled."
            ) from exc

    def close(self) -> None:
        if self._sock is not None:
            try:
                self._sock.close()
            finally:
                self._sock = None

    def __enter__(self) -> "FusionMCPClient":
        self.connect()
        return self

    def __exit__(self, *exc: Any) -> None:
        self.close()

    def ping(self) -> bool:
        """Return True if the add-in responds to a ``ping`` call."""
        try:
            self.call_tool("ping", {})
            return True
        except FusionMCPError:
            return False

    def list_tools(self) -> list[str]:
        """Return the names of all tools exposed by the add-in."""
        result = self.call_tool("list_tools", {})
        if isinstance(result, list):
            return [str(t) for t in result]
        if isinstance(result, dict) and "tools" in result:
            return [str(t.get("name", t)) for t in result["tools"]]
        return []

    def call_tool(self, name: str, params: dict | None = None) -> Any:
        """Invoke a tool by name and return its ``result`` value.

        Raises FusionMCPError if the add-in returns an error response.
        Raises FusionMCPConnectionError on socket failure.
        """
        if self._sock is None:
            self.connect()
        assert self._sock is not None

        request_id = uuid.uuid4().hex
        request = {
            "id": request_id,
            "method": name,
            "params": params or {},
        }
        line = (json.dumps(request) + "\n").encode("utf-8")

        try:
            self._sock.sendall(line)
            data = self._recv_line()
        except (OSError, ConnectionError) as exc:
            self.close()
            raise FusionMCPConnectionError(
                f"Communication with Fusion 360 MCP add-in failed: {exc}"
            ) from exc

        try:
            response = json.loads(data)
        except json.JSONDecodeError as exc:
            raise FusionMCPError(
                f"Invalid JSON from Fusion 360 MCP add-in: {exc}\n"
                f"Raw: {data!r}"
            ) from exc

        if response.get("id") != request_id:
            raise FusionMCPError(
                f"Response id mismatch (sent {request_id!r}, "
                f"got {response.get('id')!r})"
            )

        if "error" in response:
            err = response["error"] or {}
            raise FusionMCPError(
                f"Fusion 360 MCP error: "
                f"{err.get('message', 'unknown error')}"
            )

        return response.get("result")

    def execute_script(self, script: str) -> str:
        """Run ``script`` inside Fusion 360 and return its stdout.

        The add-in captures ``print()`` output from the script and
        returns it as a single string. The caller is responsible for
        JSON-decoding (most extraction scripts emit one JSON object).
        """
        result = self.call_tool("execute_script", {"script": script})
        if isinstance(result, dict) and "output" in result:
            return str(result["output"])
        if isinstance(result, str):
            return result
        return json.dumps(result)

    def _recv_line(self) -> bytes:
        """Read bytes from the socket until a newline is found.

        The faust-machines add-in uses newline-delimited JSON, so a
        single recv() usually returns the entire response. We still
        loop in case of fragmentation or large payloads.
        """
        assert self._sock is not None
        buf = bytearray()
        deadline = time.monotonic() + self.timeout
        while True:
            chunk = self._sock.recv(RECV_CHUNK)
            if not chunk:
                raise FusionMCPConnectionError(
                    "Fusion 360 MCP add-in closed the connection"
                )
            buf.extend(chunk)
            if buf.endswith(b"\n"):
                break
            if time.monotonic() > deadline:
                raise FusionMCPConnectionError(
                    "Timeout waiting for response from Fusion 360 MCP"
                )
        return bytes(buf.rstrip(b"\r\n"))


def open_default() -> FusionMCPClient:
    """Convenience constructor: a client connected to localhost:9876."""
    client = FusionMCPClient()
    client.connect()
    return client