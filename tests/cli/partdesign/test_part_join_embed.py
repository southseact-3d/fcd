"""CLI test for PartDesign_PartJoinEmbed command.

Bridge to Part command (dialog: Join embed)

NOTE: Many of these commands are GUI dialogs (e.g., preferences, dropdowns,
file dialogs) that can't be fully exercised in CLI mode. For those, this
test verifies that:
  1. The command is registered in the FreeCADGui command manager.
  2. The command's GetResources() returns a non-empty dict.

This is a smoke test, not a functional test. Full functional verification
happens in Phase 4 (interactive GUI testing).
"""
from __future__ import annotations

COMMAND_ID = "PartDesign_PartJoinEmbed"
TIMEOUT = 30

SCRIPT = r"""
import sys, json

# In CLI mode, FreeCADGui may not be fully available, but the command
# manager should still be reachable via the FreeCADGui module.
errors = []
try:
    import FreeCADGui as Gui
    cm = Gui.commandManager()
    cmd = cm.getCommandByName("PartDesign_PartJoinEmbed")
    if cmd is None:
        errors.append("command 'PartDesign_PartJoinEmbed' is not registered in the command manager")
    else:
        # Verify GetResources returns something
        try:
            res = cmd.GetResources()
            if not isinstance(res, dict):
                errors.append(f"GetResources() returned {type(res)}, expected dict")
            elif not res:
                errors.append("GetResources() returned empty dict")
        except Exception as e:
            errors.append(f"GetResources() raised: {e}")
except ImportError:
    errors.append("FreeCADGui not available in CLI mode")
except Exception as e:
    errors.append(f"unexpected error: {e}")

if errors:
    print("TEST_FAIL:", json.dumps(errors))
    sys.exit(1)
print("TEST_PASS")
sys.exit(0)
"""
