# SPDX-License-Identifier: LGPL-2.1-or-later

"""Simple dialog to display cavity detection results and suggested drain holes."""

from PySide6 import QtCore, QtWidgets

try:
    import FreeCADGui
except ImportError:
    FreeCADGui = None  # type: ignore[assignment]


class CavityResultDialog(QtWidgets.QDialog):
    """Dialog summarising cavities found in a mesh and suggested drain holes."""

    def __init__(self, obj, result, parent=None):
        super().__init__(parent)
        self.obj = obj
        self.result = result
        self.setWindowTitle("Cavity Detection Results")
        self.setMinimumSize(400, 300)
        self._build_ui()

    def _build_ui(self):
        layout = QtWidgets.QVBoxLayout(self)

        cavities = self.result.get("cavities", [])
        drain_holes = self.result.get("suggested_drain_holes", [])

        layout.addWidget(QtWidgets.QLabel(f"Object: <b>{getattr(self.obj, 'Name', 'Unknown')}</b>"))
        layout.addWidget(QtWidgets.QLabel(f"Cavities found: {len(cavities)}"))
        layout.addWidget(QtWidgets.QLabel(f"Suggested drain holes: {len(drain_holes)}"))

        list_widget = QtWidgets.QListWidget(self)
        if cavities:
            for i, c in enumerate(cavities, 1):
                vol = c.get("volume", 0.0)
                centre = c.get("center", (0, 0, 0))
                list_widget.addItem(
                    f"Cavity {i}: volume={vol:.2f} mm\u00b3, "
                    f"centre=({centre[0]:.2f}, {centre[1]:.2f}, {centre[2]:.2f})"
                )
        else:
            list_widget.addItem("No cavities detected.")
        layout.addWidget(list_widget)

        hole_list = QtWidgets.QListWidget(self)
        if drain_holes:
            for i, hole in enumerate(drain_holes, 1):
                pos = hole.get("position", (0, 0, 0))
                hole_list.addItem(
                    f"Hole {i}: ({pos[0]:.2f}, {pos[1]:.2f}, {pos[2]:.2f})"
                )
        else:
            hole_list.addItem("No drain holes suggested.")
        layout.addWidget(hole_list)

        btn_layout = QtWidgets.QHBoxLayout()
        btn_layout.addStretch()
        close_btn = QtWidgets.QPushButton("Close")
        close_btn.clicked.connect(self.close)
        btn_layout.addWidget(close_btn)
        layout.addLayout(btn_layout)

    def exec_(self):
        if hasattr(super(), "exec_"):
            return super().exec_()
        return self.exec()  # Fallback for newer PySide6
