import FreeCAD as App
import FreeCADGui as Gui
from PySide6 import QtCore, QtGui, QtWidgets
import os


class TaskSceneSettings:
    def __init__(self):
        self.form = QtWidgets.QWidget()
        self.form.setWindowTitle("Scene Settings")
        layout = QtWidgets.QVBoxLayout(self.form)

        # Environment group
        envGroup = QtWidgets.QGroupBox("Environment")
        envLayout = QtWidgets.QFormLayout(envGroup)

        self.envPathEdit = QtWidgets.QLineEdit()
        self.envBrowseBtn = QtWidgets.QPushButton("Browse...")
        self.envBrowseBtn.clicked.connect(self._browseHDR)
        envPathLayout = QtWidgets.QHBoxLayout()
        envPathLayout.addWidget(self.envPathEdit)
        envPathLayout.addWidget(self.envBrowseBtn)
        envLayout.addRow("HDRI Map:", envPathLayout)

        self.rotationSpin = QtWidgets.QDoubleSpinBox()
        self.rotationSpin.setRange(0, 360)
        self.rotationSpin.setSuffix("\u00b0")
        envLayout.addRow("Rotation:", self.rotationSpin)

        self.brightnessSpin = QtWidgets.QDoubleSpinBox()
        self.brightnessSpin.setRange(0, 100)
        self.brightnessSpin.setValue(1.0)
        self.brightnessSpin.setSingleStep(0.1)
        envLayout.addRow("Brightness:", self.brightnessSpin)

        self.bgVisibleCheck = QtWidgets.QCheckBox("Show in background")
        self.bgVisibleCheck.setChecked(True)
        envLayout.addRow(self.bgVisibleCheck)

        layout.addWidget(envGroup)

        # Background group
        bgGroup = QtWidgets.QGroupBox("Background Color")
        bgLayout = QtWidgets.QFormLayout(bgGroup)

        self.bgColorBtn = QtWidgets.QPushButton()
        self.bgColorBtn.setStyleSheet("background-color: rgb(128, 128, 128)")
        self.bgColorBtn.clicked.connect(self._pickColor)
        bgLayout.addRow("Color:", self.bgColorBtn)

        layout.addWidget(bgGroup)

        # Ground Plane group
        groundGroup = QtWidgets.QGroupBox("Ground Plane")
        groundLayout = QtWidgets.QFormLayout(groundGroup)

        self.groundCheck = QtWidgets.QCheckBox("Enable ground plane")
        groundLayout.addRow(self.groundCheck)

        self.groundHeightSpin = QtWidgets.QDoubleSpinBox()
        self.groundHeightSpin.setRange(-1000, 1000)
        groundLayout.addRow("Height:", self.groundHeightSpin)

        self.groundColorBtn = QtWidgets.QPushButton()
        self.groundColorBtn.setStyleSheet("background-color: rgb(200, 200, 200)")
        self.groundColorBtn.clicked.connect(self._pickGroundColor)
        groundLayout.addRow("Color:", self.groundColorBtn)

        layout.addWidget(groundGroup)

        # Built-in presets
        presetGroup = QtWidgets.QGroupBox("Built-in Presets")
        presetLayout = QtWidgets.QHBoxLayout(presetGroup)

        for name in ["Studio", "Outdoor", "Neutral"]:
            btn = QtWidgets.QPushButton(name)
            btn.clicked.connect(lambda checked, n=name: self._loadPreset(n))
            presetLayout.addWidget(btn)

        layout.addWidget(presetGroup)
        layout.addStretch()

        self._bgColor = (128, 128, 128)
        self._groundColor = (200, 200, 200)

    def _browseHDR(self):
        from PySide6.QtWidgets import QFileDialog

        path, _ = QFileDialog.getOpenFileName(
            self.form, "Open HDRI", "", "HDR Images (*.hdr);;All Files (*)"
        )
        if path:
            self.envPathEdit.setText(path)

    def _pickColor(self):
        from PySide6.QtWidgets import QColorDialog

        color = QColorDialog.getColor()
        if color.isValid():
            self._bgColor = (color.red(), color.green(), color.blue())
            self.bgColorBtn.setStyleSheet(
                f"background-color: rgb({color.red()}, {color.green()}, {color.blue()})"
            )

    def _pickGroundColor(self):
        from PySide6.QtWidgets import QColorDialog

        color = QColorDialog.getColor()
        if color.isValid():
            self._groundColor = (color.red(), color.green(), color.blue())
            self.groundColorBtn.setStyleSheet(
                f"background-color: rgb({color.red()}, {color.green()}, {color.blue()})"
            )

    def _loadPreset(self, name):
        presets_dir = os.path.join(
            App.getResourceDir(),
            "Mod",
            "Render",
            "Resources",
            "Materials",
            "Environments",
        )
        preset_path = os.path.join(presets_dir, f"{name}.hdr")
        if os.path.exists(preset_path):
            self.envPathEdit.setText(preset_path)

    def accept(self):
        param = App.ParamGet("User parameter:BaseApp/Preferences/Render")
        param.SetString("EnvironmentPath", self.envPathEdit.text())
        param.SetFloat("EnvRotation", self.rotationSpin.value())
        param.SetFloat("EnvBrightness", self.brightnessSpin.value())
        param.SetBool("EnvBgVisible", self.bgVisibleCheck.isChecked())
        param.SetBool("GroundEnabled", self.groundCheck.isChecked())
        param.SetFloat("GroundHeight", self.groundHeightSpin.value())
        return True

    def reject(self):
        return True

    def getStandardButtons(self):
        return QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
