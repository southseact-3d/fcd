# SPDX-License-Identifier: LGPL-2.1-or-later
"""Modal dialog that summarises a finished import.

Shows:

- The reconstructed body's name
- How many features and sketches were created
- Lists of warnings (e.g. "Fillet radius recovered as 2.5mm but edge
  selection could not be mapped")
- Lists of skipped items (e.g. "RectangularPattern - deep reference
  mapping not yet implemented")

The dialog is read-only - the user just clicks "OK" to dismiss.
"""

from __future__ import annotations

from PySide import QtWidgets


class ResultsDialog(QtWidgets.QDialog):
    """Show the user what was (and wasn't) rebuilt from Fusion 360."""

    def __init__(
        self,
        body_name: str,
        feature_count: int,
        sketch_count: int,
        skipped: list[str],
        warnings: list[str],
        parent: QtWidgets.QWidget | None = None,
    ) -> None:
        super().__init__(parent)
        self.setWindowTitle("Fusion 360 Import Results")
        self.setModal(True)
        self.resize(520, 400)

        layout = QtWidgets.QVBoxLayout(self)

        summary = QtWidgets.QLabel(
            f"<b>Import complete</b><br>"
            f"Body: <code>{body_name}</code><br>"
            f"Features created: <b>{feature_count}</b><br>"
            f"Sketches created: <b>{sketch_count}</b>"
        )
        layout.addWidget(summary)

        tabs = QtWidgets.QTabWidget()
        layout.addWidget(tabs, 1)

        tabs.addTab(self._make_list_widget(warnings, "No warnings."), "Warnings")
        tabs.addTab(
            self._make_list_widget(skipped, "Nothing was skipped - all features reconstructed."),
            "Skipped",
        )

        button_box = QtWidgets.QDialogButtonBox(QtWidgets.QDialogButtonBox.Ok)
        button_box.accepted.connect(self.accept)
        layout.addWidget(button_box)

    def _make_list_widget(self, items: list[str], empty_msg: str) -> QtWidgets.QWidget:
        widget = QtWidgets.QListWidget()
        if not items:
            widget.addItem(empty_msg)
            return widget
        for entry in items:
            widget.addItem(entry)
        return widget


def show(
    body_name: str,
    feature_count: int,
    sketch_count: int,
    skipped: list[str],
    warnings: list[str],
    parent: QtWidgets.QWidget | None = None,
) -> None:
    """Open the results dialog modally."""
    dlg = ResultsDialog(
        body_name, feature_count, sketch_count, skipped, warnings, parent
    )
    dlg.exec_()