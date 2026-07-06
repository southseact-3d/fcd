"""Template for a PartDesign CLI test.

Copy this file to test_<command_name>.py and fill in COMMAND_ID and SCRIPT.

Each test file:
  1. Defines COMMAND_ID (e.g., "PartDesign_Pad")
  2. Defines SCRIPT — a Python script that runs INSIDE FreeCADCmd
  3. The script must:
       a. Create a fresh document
       b. Set up any prerequisites (sketch, body, etc.)
       c. Invoke the command being tested (via DocumentObject API, not GUI)
       d. Assert the expected result
       e. Print TEST_PASS or TEST_FAIL: <reason>
       f. Exit 0 on pass, non-zero on fail

The runner (tests/cli/runner.py) imports this file, extracts COMMAND_ID
and SCRIPT, writes SCRIPT to a temp file, runs `FreeCADCmd <temp.py>`,
and checks stdout for the markers.
"""

from __future__ import annotations

# The command ID as registered in Gui.addCommand() or Command("...")
COMMAND_ID = "PartDesign_<CommandName>"

# Timeout in seconds (default 60). Bump for slow tests.
TIMEOUT = 60

# The in-process FreeCAD Python script.
#
# Available modules: FreeCAD (as App), FreeCADGui (as Gui, but no GUI!),
# Part, PartDesign, Sketcher, etc. FreeCADGui has limited functionality
# in CLI mode — only the App-side (Document, DocumentObject, Property)
# is fully usable. For commands that need Gui side-effects, use
# App.ActiveDocument.addObject(...) directly with the correct TypeId.
#
# Pattern:
#   1. Create a doc
#   2. Set up prerequisites
#   3. Add the feature object with the right TypeId
#   4. Set its properties
#   5. doc.recompute()
#   6. Assert the result (Shape not null, expected volume, etc.)
#   7. Print TEST_PASS or TEST_FAIL: <reason>
SCRIPT = r"""
import sys
import json

import FreeCAD as App
import Part
import PartDesign
import Sketcher

# --- 1. Create a fresh document ---
doc = App.newDocument("test_template")
if doc is None:
    print("TEST_FAIL: could not create document")
    sys.exit(1)

errors = []

# --- 2. Set up prerequisites ---
# (e.g., create a Body, then a Sketch inside it)
body = doc.addObject("PartDesign::Body", "Body")
if body is None:
    errors.append("could not create Body")

# --- 3. Add the feature object ---
# Replace 'PartDesign::<Feature>' with the actual TypeId.
# feature = body.newObject("PartDesign::<Feature>", "Feature")

# --- 4. Set properties ---
# feature.Profile = sketch
# feature.Length = 10.0

# --- 5. Recompute ---
doc.recompute()

# --- 6. Assert the result ---
# if feature is None or feature.Shape.isNull():
#     errors.append("feature shape is null after recompute")
# expected_volume = 3141.59
# actual_volume = feature.Shape.Volume
# if abs(actual_volume - expected_volume) > 1.0:
#     errors.append(f"unexpected volume: {actual_volume} (expected ~{expected_volume})")

# --- 7. Report ---
if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
else:
    print("TEST_PASS")
    sys.exit(0)
"""
