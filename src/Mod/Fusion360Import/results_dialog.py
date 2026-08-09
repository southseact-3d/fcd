# SPDX-License-Identifier: LGPL-2.1-or-later
"""Modal dialog that summarises a finished import.

Shows:

- The reconstructed body's name
- How many features, sketches, datums, bodies and occurrences were
  created.
- Lists of warnings (e.g. "Fillet radius recovered as 2.5mm but edge
  selection could not be mapped")
- Lists of skipped items (e.g. "ReplaceFace - no Tungsten CAD
  equivalent")

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
        construction_count: int,
        body_count: int,
        occurrence_count: int,
        skipped: list[str],
        warnings: list[str],
        parent: QtWidgets.QWidget | None = None,
    ) -> None:
        super().__init__(parent)
        self.setWindowTitle("Fusion 360 Import Results")
        self.setModal(True)
        self.resize(560, 480)

        layout = QtWidgets.QVBoxLayout(self)

        summary = QtWidgets.QLabel(
            f"<b>Import complete</b><br>"
            f"Body: <code>{body_name}</code><br>"
            f"Features created: <b>{feature_count}</b><br>"
            f"Sketches created: <b>{sketch_count}</b><br>"
            f"Datums created: <b>{construction_count}</b><br>"
            f"Bodies created: <b>{body_count}</b> "
            f"(occurrences: <b>{occurrence_count}</b>)"
        )
        layout.addWidget(summary)

        tabs = QtWidgets.QTabWidget()
        layout.addWidget(tabs, 1)

        tabs.addTab(
            self._make_list_widget(warnings, "No warnings."),
            f"Warnings ({len(warnings)})",
        )
        tabs.addTab(
            self._make_list_widget(
                skipped,
                "Nothing was skipped - every Fusion feature was rebuilt.",
            ),
            f"Skipped ({len(skipped)})",
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
    construction_count: int,
    body_count: int,
    occurrence_count: int,
    skipped: list[str],
    warnings: list[str],
    parent: QtWidgets.QWidget | None = None,
) -> None:
    """Open the results dialog modally."""
    dlg = ResultsDialog(
        body_name,
        feature_count,
        sketch_count,
        construction_count,
        body_count,
        occurrence_count,
        skipped,
        warnings,
        parent,
    )
    dlg.exec_()