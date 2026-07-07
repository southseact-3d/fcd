"""Shared helpers for the PartDesign CLI test harness.

This module is imported by both runner.py and individual test_*.py files.
It provides:
  - The path to the FreeCADCmd binary (from $FREECADCMD or default)
  - A helper to run an in-process FreeCAD Python script and check the result
  - Standard pass/fail markers used by the in-process scripts
  - Result dataclass for structured output

This module is NOT imported by the in-process FreeCAD scripts themselves
(those run inside FreeCADCmd, which has its own Python with FreeCAD modules).
The in-process scripts communicate with the runner via stdout markers.
"""

from __future__ import annotations

import os
import subprocess
import json
import time
import dataclasses
from typing import Optional


# --- Environment ---------------------------------------------------------

DEFAULT_FREECADCMD = "FreeCADCmd"


def get_freecadcmd() -> str:
    """Return the path to FreeCADCmd, from $FREECADCMD or the default."""
    return os.environ.get("FREECADCMD", DEFAULT_FREECADCMD)


# --- In-process markers --------------------------------------------------

# The in-process script prints these to stdout to signal pass/fail.
# The runner greps for them — they MUST appear on a line by themselves.
PASS_MARKER = "TEST_PASS"
FAIL_MARKER = "TEST_FAIL"


# --- Result dataclass ----------------------------------------------------


@dataclasses.dataclass
class TestResult:
    """Structured result of a single test run."""

    command: str
    test_file: str
    passed: bool
    returncode: int
    duration_ms: int
    stdout: str
    stderr: str
    error_message: Optional[str] = None

    def to_dict(self) -> dict:
        return dataclasses.asdict(self)

    def summary(self) -> str:
        icon = "✓" if self.passed else "✗"
        status = "PASS" if self.passed else "FAIL"
        return f"  {icon} {status} {self.command}  ({self.duration_ms} ms)"


# --- Runner helper -------------------------------------------------------


def run_freecad_script(
    command_id: str,
    test_file: str,
    script: str,
    timeout: int = 60,
    freecadcmd: Optional[str] = None,
) -> TestResult:
    """Run a Python script inside FreeCADCmd and check the result.

    The script should print TEST_PASS or TEST_FAIL to stdout.
    """
    fc = freecadcmd or get_freecadcmd()

    # Write the script to a temp file (avoids shell-escaping nightmares)
    import tempfile

    with tempfile.NamedTemporaryFile(
        suffix=".py", mode="w", delete=False, prefix=f"fcd_test_{command_id}_"
    ) as f:
        # FreeCAD captures Python's print() and sys.stdout, so we need to
        # replace print() with os.write(1, ...) which writes directly to the
        # OS-level stdout, bypassing FreeCAD's capture.
        import re as _re
        # Replace: print("TEST_PASS") -> os.write(1, b"TEST_PASS\n")
        # Use a function replacement to avoid backslash escaping issues
        _patched = _re.sub(
            r'print\("TEST_PASS"\)',
            lambda m: 'import os; os.write(1, b"TEST_PASS" + b"\\n")',
            script
        )
        # Replace: print("TEST_FAIL:", json.dumps(errors)) -> os.write(1, b"TEST_FAIL: " + json.dumps(errors).encode() + b"\n")
        _patched = _re.sub(
            r'print\("TEST_FAIL:", json\.dumps\(errors\)\)',
            lambda m: 'import os; os.write(1, b"TEST_FAIL: " + json.dumps(errors).encode() + b"\\n")',
            _patched
        )
        f.write(_patched)
        script_path = f.name

    start = time.monotonic()
    try:
        result = subprocess.run(
            [fc, script_path],
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        duration_ms = int((time.monotonic() - start) * 1000)
        returncode = result.returncode
        stdout = result.stdout or ""
        stderr = result.stderr or ""
    except subprocess.TimeoutExpired as e:
        duration_ms = int((time.monotonic() - start) * 1000)
        returncode = -1
        stdout = (e.stdout or b"").decode("utf-8", errors="replace") if isinstance(e.stdout, bytes) else (e.stdout or "")
        stderr = f"TIMEOUT after {timeout}s\n" + ((e.stderr or b"").decode("utf-8", errors="replace") if isinstance(e.stderr, bytes) else (e.stderr or ""))
    except FileNotFoundError as e:
        duration_ms = 0
        returncode = -2
        stdout = ""
        stderr = f"FreeCADCmd not found: {e}\nSet $FREECADCMD to the FreeCADCmd binary path."
    finally:
        try:
            os.unlink(script_path)
        except OSError:
            pass

    # Strict pass criteria:
    # 1. exit code 0
    # 2. TEST_PASS in stdout
    # 3. no 'Error' or 'Exception' in stdout (case-sensitive)
    passed = (
        returncode == 0
        and PASS_MARKER in stdout
        and FAIL_MARKER not in stdout
        and "Error" not in stdout
        and "Exception" not in stdout
    )

    # Extract error message if TEST_FAIL was printed
    error_message = None
    if FAIL_MARKER in stdout:
        # Try to extract the JSON-encoded error list after TEST_FAIL:
        for line in stdout.splitlines():
            if line.startswith(FAIL_MARKER + ":"):
                error_message = line[len(FAIL_MARKER) + 1 :].strip()
                break
        if not error_message:
            error_message = "TEST_FAIL marker present but no reason given"

    if returncode != 0 and not error_message:
        error_message = f"FreeCADCmd exited with code {returncode}"

    if "Error" in stdout and not error_message:
        # Find the first line containing 'Error' for context
        for line in stdout.splitlines():
            if "Error" in line:
                error_message = f"stdout contains 'Error': {line.strip()[:200]}"
                break

    if "Exception" in stdout and not error_message:
        for line in stdout.splitlines():
            if "Exception" in line:
                error_message = f"stdout contains 'Exception': {line.strip()[:200]}"
                break

    # Truncate stdout/stderr for storage (keep last 4KB for diagnosis)
    return TestResult(
        command=command_id,
        test_file=test_file,
        passed=passed,
        returncode=returncode,
        duration_ms=duration_ms,
        stdout=stdout[-4096:] if stdout else "",
        stderr=stderr[-4096:] if stderr else "",
        error_message=error_message,
    )
