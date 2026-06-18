import FreeCAD as App
import FreeCADGui as Gui
from PySide6 import QtCore, QtGui, QtWidgets


class SceneBrowserPanel(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QtWidgets.QVBoxLayout(self)

        label = QtWidgets.QLabel("Render Scene Objects")
        label.setStyleSheet("font-weight: bold;")
        layout.addWidget(label)

        self.tree = QtWidgets.QTreeWidget()
        self.tree.setHeaderLabels(["Object", "Visible", "Material"])
        self.tree.setColumnCount(3)
        layout.addWidget(self.tree)

        refreshBtn = QtWidgets.QPushButton("Refresh")
        refreshBtn.clicked.connect(self._refresh)
        layout.addWidget(refreshBtn)

        self._dock = None

    def _refresh(self):
        self.tree.clear()
        if not App.ActiveDocument:
            return
        for obj in App.ActiveDocument.Objects:
            if hasattr(obj, "Shape"):
                item = QtWidgets.QTreeWidgetItem(self.tree)
                item.setText(0, obj.Label)
                item.setCheckState(1, QtCore.Qt.Checked)
                item.setText(2, "Default")


def showSceneBrowser():
    """Show or create the scene browser dock widget."""
    mw = Gui.getMainWindow()
    dock = mw.findChild(QtWidgets.QDockWidget, "RenderSceneBrowser")
    if dock:
        dock.show()
        dock.raise_()
        return

    dock = QtWidgets.QDockWidget("Scene Browser", mw)
    dock.setObjectName("RenderSceneBrowser")
    panel = SceneBrowserPanel(dock)
    dock.setWidget(panel)
    mw.addDockWidget(QtCore.Qt.LeftDockWidgetArea, dock)
    panel._dock = dock


def toggleSceneBrowser():
    mw = Gui.getMainWindow()
    dock = mw.findChild(QtWidgets.QDockWidget, "RenderSceneBrowser")
    if dock:
        if dock.isVisible():
            dock.hide()
        else:
            dock.show()
            dock.raise_()
    else:
        showSceneBrowser()
