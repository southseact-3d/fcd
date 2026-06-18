import FreeCAD as App
import FreeCADGui as Gui
from PySide6 import QtCore, QtGui, QtWidgets


class RenderQueuePanel(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QtWidgets.QVBoxLayout(self)

        # Queue list
        self.queueList = QtWidgets.QTreeWidget()
        self.queueList.setHeaderLabels(["Name", "Status", "Progress", "Output"])
        self.queueList.setColumnCount(4)
        layout.addWidget(self.queueList)

        # Buttons
        btnLayout = QtWidgets.QHBoxLayout()
        self.startBtn = QtWidgets.QPushButton("Start Selected")
        self.startBtn.clicked.connect(self._startSelected)
        self.cancelBtn = QtWidgets.QPushButton("Cancel Selected")
        self.cancelBtn.clicked.connect(self._cancelSelected)
        self.clearBtn = QtWidgets.QPushButton("Clear Completed")
        self.clearBtn.clicked.connect(self._clearCompleted)
        btnLayout.addWidget(self.startBtn)
        btnLayout.addWidget(self.cancelBtn)
        btnLayout.addWidget(self.clearBtn)
        layout.addLayout(btnLayout)

        self._dock = None

    def _startSelected(self):
        item = self.queueList.currentItem()
        if item:
            App.Console.PrintMessage(f"Starting render: {item.text(0)}\n")

    def _cancelSelected(self):
        item = self.queueList.currentItem()
        if item:
            App.Console.PrintMessage(f"Cancelling render: {item.text(0)}\n")

    def _clearCompleted(self):
        for i in range(self.queueList.topLevelItemCount() - 1, -1, -1):
            item = self.queueList.topLevelItem(i)
            if item.text(1) == "Completed":
                self.queueList.takeTopLevelItem(i)


def showQueuePanel():
    """Show or create the render queue dock widget."""
    mw = Gui.getMainWindow()
    dock = mw.findChild(QtWidgets.QDockWidget, "RenderQueue")
    if dock:
        dock.show()
        dock.raise_()
        return

    dock = QtWidgets.QDockWidget("Render Queue", mw)
    dock.setObjectName("RenderQueue")
    panel = RenderQueuePanel(dock)
    dock.setWidget(panel)
    mw.addDockWidget(QtCore.Qt.RightDockWidgetArea, dock)
    panel._dock = dock


def toggleQueuePanel():
    """Toggle the render queue dock widget."""
    mw = Gui.getMainWindow()
    dock = mw.findChild(QtWidgets.QDockWidget, "RenderQueue")
    if dock:
        if dock.isVisible():
            dock.hide()
        else:
            dock.show()
            dock.raise_()
    else:
        showQueuePanel()
