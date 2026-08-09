# SPDX-License-Identifier: LGPL-2.1-or-later
"""A lightweight progress dialog shown during extraction.

We don't want a heavy modal blocking the GUI thread, so this dialog
is frameless and just shows the current step + a progress bar. The
extraction itself runs in a worker thread; this dialog is owned by
the GUI thread and updates via Qt signals.
"""

from __future__ import annotations

from PySide import QtCore, QtWidgets


class ProgressDialog(QtWidgets.QDialog):
    """Borderless progress dialog driven by ``ProgressSignal``."""

    def __init__(self, parent: QtWidgets.QWidget | None = None) -> None:
        super().__init__(parent)
        self.setWindowTitle("Importing from Fusion 360")
        self.setModal(True)
        self.setMinimumWidth(420)
        self.setWindowFlag(QtCore.Qt.FramelessWindowHint, False)

        layout = QtWidgets.QVBoxLayout(self)

        self.heading = QtWidgets.QLabel(
            "<b>Importing design from Fusion 360...</b>"
        )
        layout.addWidget(self.heading)

        self.status = QtWidgets.QLabel(" ")
        self.status.setWordWrap(True)
        layout.addWidget(self.status)

        self.bar = QtWidgets.QProgressBar()
        self.bar.setRange(0, 100)
        self.bar.setValue(0)
        layout.addWidget(self.bar)

        self.cancel_button = QtWidgets.QPushButton("Cancel")
        self.cancel_button.clicked.connect(self.reject)
        layout.addWidget(self.cancel_button, alignment=QtCore.Qt.AlignRight)

    def update_progress(self, message: str, fraction: float) -> None:
        self.status.setText(message)
        self.bar.setValue(int(max(0.0, min(1.0, fraction)) * 100))
        QtWidgets.QApplication.processEvents()


def make_callback(dialog: ProgressDialog):
    """Return a ``progress(message, fraction)`` closure bound to ``dialog``."""
    def _callback(message: str, fraction: float) -> None:
        dialog.update_progress(message, fraction)
    return _callback