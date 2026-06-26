# SPDX-License-Identifier: LGPL-2.1-or-later
"""Qt dialog for configuring the Fusion 360 MCP connection.

The dialog persists its settings in the user's FreeCAD preferences so
they survive across sessions. It also offers a "Test connection"
button that performs a TCP handshake against the add-in without
running any extraction - this is the friendly way to discover
connectivity problems before the user kicks off a full import.
"""

from __future__ import annotations

import FreeCAD

from PySide import QtCore, QtWidgets


PREF_PATH = "User parameter:BaseApp/Mod/Fusion360Import"


def _pref_group():
    return FreeCAD.ParamGet(PREF_PATH)


def load_settings() -> dict:
    grp = _pref_group()
    return {
        "host": grp.GetString("Host", "127.0.0.1"),
        "port": grp.GetInt("Port", 9876),
        "auto_connect": grp.GetBool("AutoConnect", True),
    }


def save_settings(host: str, port: int, auto_connect: bool) -> None:
    grp = _pref_group()
    grp.SetString("Host", host)
    grp.SetInt("Port", int(port))
    grp.SetBool("AutoConnect", bool(auto_connect))


class ConnectionDialog(QtWidgets.QDialog):
    """Modal dialog for editing the MCP connection settings."""

    def __init__(self, parent: QtWidgets.QWidget | None = None) -> None:
        super().__init__(parent)
        self.setWindowTitle("Connect to Fusion 360")
        self.setModal(True)
        self.resize(420, 240)

        layout = QtWidgets.QVBoxLayout(self)

        intro = QtWidgets.QLabel(
            "<b>Fusion 360 Connection</b><br>"
            "Enter the host and port of the Fusion 360 MCP add-in "
            "(default localhost:9876). The add-in must be running in "
            "Fusion 360 before importing."
        )
        intro.setWordWrap(True)
        layout.addWidget(intro)

        form = QtWidgets.QFormLayout()
        self.host_edit = QtWidgets.QLineEdit()
        self.port_edit = QtWidgets.QSpinBox()
        self.port_edit.setRange(1, 65535)
        settings = load_settings()
        self.host_edit.setText(settings["host"])
        self.port_edit.setValue(int(settings["port"]))
        form.addRow("Host:", self.host_edit)
        form.addRow("Port:", self.port_edit)
        layout.addLayout(form)

        self.auto_check = QtWidgets.QCheckBox(
            "Connect automatically when importing"
        )
        self.auto_check.setChecked(bool(settings["auto_connect"]))
        layout.addWidget(self.auto_check)

        self.status_label = QtWidgets.QLabel("")
        self.status_label.setWordWrap(True)
        layout.addWidget(self.status_label)

        button_row = QtWidgets.QHBoxLayout()
        self.test_button = QtWidgets.QPushButton("Test connection")
        self.test_button.clicked.connect(self._on_test)
        self.save_button = QtWidgets.QPushButton("Save")
        self.save_button.setDefault(True)
        self.save_button.clicked.connect(self._on_save)
        self.cancel_button = QtWidgets.QPushButton("Cancel")
        self.cancel_button.clicked.connect(self.reject)
        button_row.addWidget(self.test_button)
        button_row.addStretch(1)
        button_row.addWidget(self.cancel_button)
        button_row.addWidget(self.save_button)
        layout.addLayout(button_row)

    # ------------------------------------------------------------------
    # Slots
    # ------------------------------------------------------------------

    def _on_test(self) -> None:
        host = self.host_edit.text().strip() or "127.0.0.1"
        port = int(self.port_edit.value())
        self.status_label.setText(f"Connecting to {host}:{port}...")
        QtWidgets.QApplication.processEvents()

        # Import lazily so the dialog opens even if the MCP module is
        # broken for any reason.
        try:
            from . import fusion360_mcp_client
            client = fusion360_mcp_client.FusionMCPClient(host=host, port=port, timeout=5.0)
            client.connect()
            ok = client.ping()
            client.close()
        except Exception as exc:
            ok = False
            self.status_label.setText(
                f"<span style='color:#c0392b'>Failed: {exc}</span>"
            )
            return

        if ok:
            self.status_label.setText(
                "<span style='color:#27ae60'>Connection OK - "
                "Fusion 360 MCP add-in is responding.</span>"
            )
        else:
            self.status_label.setText(
                "<span style='color:#c0392b'>Connected, but the add-in "
                "did not respond to ping.</span>"
            )

    def _on_save(self) -> None:
        host = self.host_edit.text().strip() or "127.0.0.1"
        port = int(self.port_edit.value())
        save_settings(host, port, self.auto_check.isChecked())
        self.accept()

    # ------------------------------------------------------------------
    # Convenience accessors
    # ------------------------------------------------------------------

    def settings(self) -> tuple[str, int, bool]:
        return (
            self.host_edit.text().strip() or "127.0.0.1",
            int(self.port_edit.value()),
            bool(self.auto_check.isChecked()),
        )


def show(parent: QtWidgets.QWidget | None = None) -> tuple[str, int, bool] | None:
    """Open the dialog and return ``(host, port, auto_connect)`` if saved."""
    dlg = ConnectionDialog(parent)
    if dlg.exec_() == QtWidgets.QDialog.Accepted:
        return dlg.settings()
    return None