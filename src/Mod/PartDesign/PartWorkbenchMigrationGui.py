# SPDX-License-Identifier: LGPL-2.1-or-later

"""GUI command for migrating legacy Part command names in macros."""

import FreeCAD as App  # type: ignore
import FreeCADGui as Gui  # type: ignore
from PySide import QtCore, QtGui  # type: ignore

from PartDesign.PartWorkbenchMigration import migrate_macro_file


class CommandMigratePartMacros:
    def GetResources(self):
        return {
            "MenuText": QtCore.QT_TRANSLATE_NOOP(
                "PartDesign_MigratePartMacros", "Migrate Part Macros..."
            ),
            "ToolTip": QtCore.QT_TRANSLATE_NOOP(
                "PartDesign_MigratePartMacros",
                "Rewrite legacy Part_* command names to PartDesign bridge commands in selected macro files.",
            ),
            "Pixmap": "PartDesignWorkbench",
        }

    def Activated(self):
        parent = Gui.getMainWindow()
        files, _selected_filter = QtGui.QFileDialog.getOpenFileNames(
            parent,
            App.Qt.translate("PartDesign_MigratePartMacros", "Select macros to migrate"),
            "",
            "FreeCAD Macros (*.FCMacro);;Python Files (*.py);;All Files (*)",
        )

        if not files:
            return

        migrated_count = 0
        failed = []
        for path in files:
            try:
                migrate_macro_file(path, in_place=True)
                migrated_count += 1
            except Exception as err:
                failed.append((path, str(err)))

        if not failed:
            QtGui.QMessageBox.information(
                parent,
                App.Qt.translate("PartDesign_MigratePartMacros", "Migration Complete"),
                App.Qt.translate(
                    "PartDesign_MigratePartMacros",
                    "Migrated {count} file(s).".format(count=migrated_count),
                ),
            )
            return

        details = "\n".join(["{}: {}".format(path, err) for path, err in failed])
        QtGui.QMessageBox.warning(
            parent,
            App.Qt.translate("PartDesign_MigratePartMacros", "Migration Completed with Errors"),
            App.Qt.translate(
                "PartDesign_MigratePartMacros",
                "Migrated {ok} file(s). Failed: {failed}.".format(
                    ok=migrated_count,
                    failed=len(failed),
                ),
            )
            + "\n\n"
            + details,
        )

    def IsActive(self):
        return True


def register_gui_commands():
    Gui.addCommand("PartDesign_MigratePartMacros", CommandMigratePartMacros())
