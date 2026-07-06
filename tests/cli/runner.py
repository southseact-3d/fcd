#!/usr/bin/env python3
"""CLI test runner for the FCD PartDesign workbench.

Walks tests/cli/partdesign/test_*.py, runs each as a subprocess via
FreeCADCmd, captures results, and writes CSV + JSON reports.

Usage:
  python3 tests/cli/runner.py [--filter PATTERN] [--fail-fast] [--timeout 60] [--freecadcmd PATH]
"""

from __future__ import annotations

import argparse
import csv
import glob
import importlib.util
import json
import os
import sys
import time
from datetime import datetime, timezone
from pathlib import Path

# Make conftest.py importable
SCRIPT_DIR = Path(__file__).resolve().parent
sys.path.insert(0, str(SCRIPT_DIR))
from conftest import TestResult, get_freecadcmd, run_freecad_script  # noqa: E402


# ANSI colors
GREEN = "\033[32m"
RED = "\033[31m"
YELLOW = "\033[33m"
BLUE = "\033[34m"
GREY = "\033[90m"
RESET = "\033[0m"
BOLD = "\033[1m"


def discover_tests(tests_dir: Path, filter_pattern: str | None = None) -> list[Path]:
    """Find all test_*.py files in tests_dir, optionally filtered by pattern."""
    pattern = "test_*.py"
    tests = sorted(tests_dir.glob(pattern))
    if filter_pattern:
        tests = [t for t in tests if filter_pattern.lower() in t.name.lower()]
    return tests


def load_test(test_path: Path) -> dict:
    """Dynamically import a test_*.py file and return its metadata.

    Each test file must define module-level:
      - COMMAND_ID: str  (e.g., "PartDesign_Pad")
      - SCRIPT: str      (the in-process FreeCAD Python code)
    Optional:
      - TIMEOUT: int = 60
    """
    spec = importlib.util.spec_from_file_location(test_path.stem, test_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return {
        "command_id": getattr(module, "COMMAND_ID"),
        "script": getattr(module, "SCRIPT"),
        "timeout": getattr(module, "TIMEOUT", 60),
        "test_file": test_path.name,
    }


def run_all(
    tests_dir: Path,
    results_dir: Path,
    filter_pattern: str | None = None,
    fail_fast: bool = False,
    timeout: int = 60,
    freecadcmd: str | None = None,
) -> int:
    """Run all tests, write reports, return exit code (0=all pass, 1=any fail)."""
    tests = discover_tests(tests_dir, filter_pattern)
    if not tests:
        print(f"{YELLOW}No tests found in {tests_dir}{RESET}")
        return 2

    print(f"{BOLD}=== FCD PartDesign CLI Test Runner ==={RESET}")
    print(f"FreeCADCmd: {freecadcmd or get_freecadcmd()}")
    print(f"Tests dir:  {tests_dir}")
    print(f"Filter:     {filter_pattern or '(none)'}")
    print(f"Timeout:    {timeout}s per test")
    print(f"Fail-fast:  {fail_fast}")
    print(f"Tests:      {len(tests)}")
    print()

    results: list[TestResult] = []
    pass_count = 0
    fail_count = 0
    skip_count = 0
    start = time.monotonic()

    for i, test_path in enumerate(tests, 1):
        try:
            meta = load_test(test_path)
        except Exception as e:
            print(f"  {RED}✗ LOAD_FAIL {test_path.name}: {e}{RESET}")
            skip_count += 1
            continue

        print(f"[{i}/{len(tests)}] {meta['command_id']}", end="", flush=True)

        result = run_freecad_script(
            command_id=meta["command_id"],
            test_file=meta["test_file"],
            script=meta["script"],
            timeout=timeout,
            freecadcmd=freecadcmd,
        )
        results.append(result)

        if result.passed:
            pass_count += 1
            print(
                f"  {GREEN}✓ PASS{RESET} ({result.duration_ms} ms)",
                flush=True,
            )
        else:
            fail_count += 1
            print(
                f"  {RED}✗ FAIL{RESET} ({result.duration_ms} ms)",
                flush=True,
            )
            if result.error_message:
                msg = result.error_message[:200]
                print(f"      {RED}{msg}{RESET}")
            if fail_fast:
                print(f"\n{RED}{BOLD}Stopping at first failure (--fail-fast).{RESET}")
                break

    elapsed = time.monotonic() - start
    total = pass_count + fail_count + skip_count
    print()
    print(f"{BOLD}=== Summary ==={RESET}")
    print(f"  {GREEN}PASS: {pass_count}{RESET}")
    print(f"  {RED}FAIL: {fail_count}{RESET}")
    print(f"  {YELLOW}SKIP: {skip_count}{RESET}")
    print(f"  TOTAL: {total}")
    print(f"  Elapsed: {elapsed:.1f}s")

    # Write reports
    results_dir.mkdir(parents=True, exist_ok=True)
    ts = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    csv_path = results_dir / f"{ts}.csv"
    json_path = results_dir / f"{ts}.json"

    with open(csv_path, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(
            ["command", "test_file", "passed", "returncode", "duration_ms", "error_message"]
        )
        for r in results:
            writer.writerow(
                [
                    r.command,
                    r.test_file,
                    r.passed,
                    r.returncode,
                    r.duration_ms,
                    r.error_message or "",
                ]
            )

    with open(json_path, "w") as f:
        json.dump([r.to_dict() for r in results], f, indent=2)

    print(f"\nReports written:")
    print(f"  CSV:  {csv_path}")
    print(f"  JSON: {json_path}")

    return 0 if fail_count == 0 else 1


def main():
    parser = argparse.ArgumentParser(description="FCD PartDesign CLI test runner")
    parser.add_argument(
        "--filter",
        type=str,
        default=None,
        help="Only run tests whose filename contains this substring (case-insensitive)",
    )
    parser.add_argument(
        "--fail-fast",
        action="store_true",
        help="Stop at the first failure",
    )
    parser.add_argument(
        "--timeout",
        type=int,
        default=60,
        help="Timeout per test in seconds (default: 60)",
    )
    parser.add_argument(
        "--freecadcmd",
        type=str,
        default=None,
        help="Path to FreeCADCmd (overrides $FREECADCMD)",
    )
    parser.add_argument(
        "--tests-dir",
        type=str,
        default=str(SCRIPT_DIR / "partdesign"),
        help="Directory containing test_*.py files",
    )
    parser.add_argument(
        "--results-dir",
        type=str,
        default=str(SCRIPT_DIR / "results"),
        help="Directory to write CSV/JSON reports",
    )
    args = parser.parse_args()

    tests_dir = Path(args.tests_dir)
    results_dir = Path(args.results_dir)

    exit_code = run_all(
        tests_dir=tests_dir,
        results_dir=results_dir,
        filter_pattern=args.filter,
        fail_fast=args.fail_fast,
        timeout=args.timeout,
        freecadcmd=args.freecadcmd,
    )
    sys.exit(exit_code)


if __name__ == "__main__":
    main()
