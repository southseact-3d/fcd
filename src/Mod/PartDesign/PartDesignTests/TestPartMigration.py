# SPDX-License-Identifier: LGPL-2.1-or-later

import tempfile
import unittest
from pathlib import Path

from PartDesign.PartWorkbenchMigration import (
    migrate_macro_file,
    remap_command_name,
    rewrite_macro_content,
)


class TestPartMigration(unittest.TestCase):
    def testRemapCommand(self):
        self.assertEqual(remap_command_name("Part_Box"), "PartDesign_PartBox")
        self.assertEqual(remap_command_name("Part_Unknown"), "Part_Unknown")

    def testRewriteMacroContent(self):
        source = """
import FreeCADGui as Gui
Gui.runCommand('Part_Box', 0)
Gui.runCommand(\"Part_Cut\", 0)
Gui.runCommand('Part_CheckGeometry', 0)
"""
        migrated = rewrite_macro_content(source)
        self.assertIn("PartDesign_PartBox", migrated)
        self.assertIn("PartDesign_PartCut", migrated)
        self.assertIn("PartDesign_PartCheckGeometry", migrated)
        self.assertNotIn("'Part_Box'", migrated)

    def testMigrateMacroFile(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            macro_path = Path(tmpdir) / "legacy_macro.FCMacro"
            macro_path.write_text("Gui.runCommand('Part_Sweep', 0)\n", encoding="utf-8")

            migrated = migrate_macro_file(str(macro_path), in_place=False)
            self.assertIn("PartDesign_PartSweep", migrated)
            self.assertIn("Part_Sweep", macro_path.read_text(encoding="utf-8"))

            migrate_macro_file(str(macro_path), in_place=True)
            self.assertIn("PartDesign_PartSweep", macro_path.read_text(encoding="utf-8"))


if __name__ == "__main__":
    unittest.main()
