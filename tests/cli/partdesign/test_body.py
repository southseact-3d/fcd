"""CLI test for PartDesign_Body command.

Verifies that a PartDesign Body can be created and is correctly typed.
"""
from __future__ import annotations

COMMAND_ID = "PartDesign_Body"
TIMEOUT = 30

SCRIPT = r"""
import sys, json
import FreeCAD as App
import PartDesign

doc = App.newDocument("test_body")
errors = []

# Create a Body
body = doc.addObject("PartDesign::Body", "Body")
if body is None:
    errors.append("Body object was not created")
else:
    if not body.TypeId.startswith("PartDesign::Body"):
        errors.append(f"wrong TypeId: {body.TypeId}")
    if body.Tip is not None:
        errors.append(f"new Body should have None Tip, got {body.Tip}")
    # Body should have an empty Shape until features are added
    try:
        s = body.Shape
        # Empty shape is OK at this stage
    except Exception as e:
        errors.append(f"Body.Shape access raised: {e}")

doc.recompute()

# Verify it appears in the document
if "Body" not in [o.Name for o in doc.Objects]:
    errors.append("Body not found in doc.Objects after recompute")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
else:
    print("TEST_PASS")
    sys.exit(0)
"""
