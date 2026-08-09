# SPDX-License-Identifier: LGPL-2.1-or-later

# Fusion360Import init script (headless).
# Registers the workbench-only file importers (none yet - F3D is parsed
# at a higher level) and prints a one-line startup message.

import FreeCAD

FreeCAD.Console.PrintLog("[Fusion360Import] headless init loaded\n")