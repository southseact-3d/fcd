# PartDesign CLI Test Harness

Per-command CLI tests for the FCD PartDesign workbench. Each test invokes
`FreeCADCmd` headlessly, exercises a single command, and verifies the
result with strict pass criteria.

## Layout

```
tests/cli/
├── README.md          # this file
├── conftest.py        # shared helpers (FreeCAD path, assertion utils)
├── runner.py          # runs all tests, aggregates results
├── results/           # CSV + JSON output per run (gitignored)
└── partdesign/
    ├── test_body.py
    ├── test_pad.py
    ├── test_pocket.py
    └── ... (one per command in docs/PARITY_MATRIX.md)
```

## Running

### Prerequisites

Set `FREECADCMD` to point at your `FreeCADCmd` binary:

```bash
export FREECADCMD=/path/to/FreeCADCmd
# or, if installed system-wide:
export FREECADCMD=FreeCADCmd
```

If you're running against the build dir from the Phase 0 AppImage extract:

```bash
export FREECADCMD=/home/runner/work/fcd/fcd/build/install/bin/FreeCADCmd
```

### Run a single test

```bash
python3 tests/cli/partdesign/test_pad.py
```

Prints JSON with `passed`, `stdout`, `stderr`, `returncode`, `duration_ms`.

Exits 0 on pass, 1 on fail.

### Run all tests

```bash
python3 tests/cli/runner.py
```

Walks `tests/cli/partdesign/test_*.py`, runs each as a subprocess with
60-second timeout, captures results, writes:

- `tests/cli/results/<timestamp>.csv` — one row per test
- `tests/cli/results/<timestamp>.json` — full structured output
- stdout summary table: `PASS 45 / FAIL 3 / SKIP 0 / TOTAL 48`

Exits non-zero if any test failed.

### Run with options

```bash
# Only run tests matching a pattern
python3 tests/cli/runner.py --filter pad

# Stop on first failure
python3 tests/cli/runner.py --fail-fast

# Longer timeout (some commands need >60s on slow CI)
python3 tests/cli/runner.py --timeout 120
```

## Strict pass criteria

A test passes if and only if ALL of the following are true:

1. `FreeCADCmd` exits with code 0
2. stdout contains `TEST_PASS` (printed by the in-process test script)
3. stdout does NOT contain `Error` or `Exception` (case-sensitive)
4. The expected geometry object exists in the document tree (asserted
   inside the in-process script)

Any failure of (1)–(4) fails the test. The runner captures stdout and
stderr for diagnosis.

## Adding a new test

1. Look up the command in `docs/PARITY_MATRIX.md` to find its test name
   (e.g., `test_pad.py` for `PartDesign_Pad`).
2. Copy `tests/cli/partdesign/_template.py` to `test_<name>.py`.
3. Fill in the `COMMAND_ID`, `SCRIPT` (the in-process FreeCAD Python),
   and any custom assertions.
4. Run `python3 tests/cli/partdesign/test_<name>.py` to verify it works.

## GitHub Actions

The workflow at `.github/workflows/sub_cli_tests.yml` runs the full suite
on every PR that touches `src/Mod/PartDesign/` or `tests/cli/`. It
downloads the Ubuntu build artifact from `sub_buildUbuntu.yml`, extracts
`FreeCADCmd`, sets `FREECADCMD`, and invokes `runner.py`. Results are
uploaded as a workflow artifact for review.
