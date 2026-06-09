import FreeCAD as App
import FreeCADGui as Gui
from PySide6 import QtCore, QtGui, QtWidgets


class TaskCameraSettings:
    def __init__(self):
        self.form = QtWidgets.QWidget()
        self.form.setWindowTitle("Camera Settings")
        layout = QtWidgets.QVBoxLayout(self.form)

        # Projection group
        projGroup = QtWidgets.QGroupBox("Projection")
        projLayout = QtWidgets.QFormLayout(projGroup)

        self.perspectiveRadio = QtWidgets.QRadioButton("Perspective")
        self.orthoRadio = QtWidgets.QRadioButton("Orthographic")
        self.perspectiveRadio.setChecked(True)
        projLayout.addRow(self.perspectiveRadio)
        projLayout.addRow(self.orthoRadio)

        self.fovSpin = QtWidgets.QDoubleSpinBox()
        self.fovSpin.setRange(1, 179)
        self.fovSpin.setValue(45)
        self.fovSpin.setSuffix("\u00b0")
        projLayout.addRow("FOV:", self.fovSpin)

        layout.addWidget(projGroup)

        # DOF group
        dofGroup = QtWidgets.QGroupBox("Depth of Field")
        dofLayout = QtWidgets.QFormLayout(dofGroup)

        self.dofCheck = QtWidgets.QCheckBox("Enable DOF")
        dofLayout.addRow(self.dofCheck)

        self.apertureSpin = QtWidgets.QDoubleSpinBox()
        self.apertureSpin.setRange(0, 100)
        self.apertureSpin.setSingleStep(0.1)
        dofLayout.addRow("Aperture:", self.apertureSpin)

        self.focalDistSpin = QtWidgets.QDoubleSpinBox()
        self.focalDistSpin.setRange(0.01, 10000)
        self.focalDistSpin.setValue(10)
        dofLayout.addRow("Focal Distance:", self.focalDistSpin)

        layout.addWidget(dofGroup)

        # Exposure group
        expGroup = QtWidgets.QGroupBox("Exposure")
        expLayout = QtWidgets.QFormLayout(expGroup)

        self.exposureSpin = QtWidgets.QDoubleSpinBox()
        self.exposureSpin.setRange(-10, 20)
        self.exposureSpin.setValue(0)
        self.exposureSpin.setSingleStep(0.5)
        expLayout.addRow("EV100:", self.exposureSpin)

        layout.addWidget(expGroup)

        # Position group
        posGroup = QtWidgets.QGroupBox("Position")
        posLayout = QtWidgets.QFormLayout(posGroup)

        self.posX = QtWidgets.QDoubleSpinBox()
        self.posY = QtWidgets.QDoubleSpinBox()
        self.posZ = QtWidgets.QDoubleSpinBox()
        self.posX.setRange(-10000, 10000)
        self.posY.setRange(-10000, 10000)
        self.posZ.setRange(-10000, 10000)
        posLayout.addRow("X:", self.posX)
        posLayout.addRow("Y:", self.posY)
        posLayout.addRow("Z:", self.posZ)

        self.targetX = QtWidgets.QDoubleSpinBox()
        self.targetY = QtWidgets.QDoubleSpinBox()
        self.targetZ = QtWidgets.QDoubleSpinBox()
        self.targetX.setRange(-10000, 10000)
        self.targetY.setRange(-10000, 10000)
        self.targetZ.setRange(-10000, 10000)
        posLayout.addRow("Target X:", self.targetX)
        posLayout.addRow("Target Y:", self.targetY)
        posLayout.addRow("Target Z:", self.targetZ)

        layout.addWidget(posGroup)
        layout.addStretch()

        # Load current viewer camera
        self._loadFromViewer()

    def _loadFromViewer(self):
        try:
            viewer = Gui.ActiveDocument.ActiveView.getViewer()
            cam = viewer.getCamera()
            if cam:
                self.posX.setValue(cam.position.getValue().getX())
                self.posY.setValue(cam.position.getValue().getY())
                self.posZ.setValue(cam.position.getValue().getZ())
        except Exception:
            pass

    def accept(self):
        param = App.ParamGet("User parameter:BaseApp/Preferences/Render")
        param.SetBool("Orthographic", self.orthoRadio.isChecked())
        param.SetFloat("FOV", self.fovSpin.value())
        param.SetBool("DOFEnabled", self.dofCheck.isChecked())
        param.SetFloat("Aperture", self.apertureSpin.value())
        param.SetFloat("FocalDistance", self.focalDistSpin.value())
        param.SetFloat("Exposure", self.exposureSpin.value())
        return True

    def reject(self):
        return True

    def getStandardButtons(self):
        return QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
