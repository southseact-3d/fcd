import FreeCAD as App
import FreeCADGui as Gui
from PySide6 import QtCore, QtGui, QtWidgets


class TaskRenderSettings:
    def __init__(self):
        self.form = QtWidgets.QWidget()
        self.form.setWindowTitle("Render Settings")
        layout = QtWidgets.QVBoxLayout(self.form)

        # Resolution group
        resGroup = QtWidgets.QGroupBox("Resolution")
        resLayout = QtWidgets.QFormLayout(resGroup)
        self.widthSpin = QtWidgets.QSpinBox()
        self.widthSpin.setRange(64, 7680)
        self.widthSpin.setValue(1920)
        self.heightSpin = QtWidgets.QSpinBox()
        self.heightSpin.setRange(64, 4320)
        self.heightSpin.setValue(1080)
        resLayout.addRow("Width:", self.widthSpin)
        resLayout.addRow("Height:", self.heightSpin)

        # Aspect ratio presets
        presetLayout = QtWidgets.QHBoxLayout()
        for label, w, h in [
            ("16:9", 1920, 1080),
            ("4:3", 1600, 1200),
            ("1:1", 1080, 1080),
            ("21:9", 2560, 1080),
        ]:
            btn = QtWidgets.QPushButton(label)
            btn.clicked.connect(lambda checked, w=w, h=h: self._setResolution(w, h))
            presetLayout.addWidget(btn)
        resLayout.addRow("Presets:", presetLayout)
        layout.addWidget(resGroup)

        # Quality group
        qualGroup = QtWidgets.QGroupBox("Quality")
        qualLayout = QtWidgets.QFormLayout(qualGroup)
        self.samplesSpin = QtWidgets.QSpinBox()
        self.samplesSpin.setRange(1, 10000)
        self.samplesSpin.setValue(256)
        self.bouncesSpin = QtWidgets.QSpinBox()
        self.bouncesSpin.setRange(1, 64)
        self.bouncesSpin.setValue(8)
        qualLayout.addRow("Max Samples:", self.samplesSpin)
        qualLayout.addRow("Max Bounces:", self.bouncesSpin)
        layout.addWidget(qualGroup)

        # Output group
        outGroup = QtWidgets.QGroupBox("Output")
        outLayout = QtWidgets.QFormLayout(outGroup)
        self.formatCombo = QtWidgets.QComboBox()
        self.formatCombo.addItems(["PNG", "JPEG", "EXR"])
        self.qualitySpin = QtWidgets.QSpinBox()
        self.qualitySpin.setRange(1, 100)
        self.qualitySpin.setValue(95)
        self.pathEdit = QtWidgets.QLineEdit()
        self.pathBtn = QtWidgets.QPushButton("Browse...")
        self.pathBtn.clicked.connect(self._browsePath)
        outLayout.addRow("Format:", self.formatCombo)
        outLayout.addRow("JPEG Quality:", self.qualitySpin)
        pathLayout = QtWidgets.QHBoxLayout()
        pathLayout.addWidget(self.pathEdit)
        pathLayout.addWidget(self.pathBtn)
        outLayout.addRow("Output Path:", pathLayout)
        layout.addWidget(outGroup)

        # Tone mapping group
        tmGroup = QtWidgets.QGroupBox("Tone Mapping")
        tmLayout = QtWidgets.QFormLayout(tmGroup)
        self.toneCombo = QtWidgets.QComboBox()
        self.toneCombo.addItems(["ACES", "Reinhard", "Filmic", "None"])
        tmLayout.addRow("Operator:", self.toneCombo)
        layout.addWidget(tmGroup)

        layout.addStretch()

    def _setResolution(self, w, h):
        self.widthSpin.setValue(w)
        self.heightSpin.setValue(h)

    def _browsePath(self):
        from PySide6.QtWidgets import QFileDialog

        path, _ = QFileDialog.getSaveFileName(
            self.form, "Save Render", "", "PNG (*.png);;JPEG (*.jpg);;EXR (*.exr)"
        )
        if path:
            self.pathEdit.setText(path)

    def accept(self):
        param = App.ParamGet("User parameter:BaseApp/Preferences/Render")
        param.SetInt("Width", self.widthSpin.value())
        param.SetInt("Height", self.heightSpin.value())
        param.SetInt("MaxSamples", self.samplesSpin.value())
        param.SetInt("MaxBounces", self.bouncesSpin.value())
        param.SetInt("Format", self.formatCombo.currentIndex())
        param.SetInt("JPEGQuality", self.qualitySpin.value())
        param.SetString("OutputPath", self.pathEdit.text())
        param.SetInt("ToneMapping", self.toneCombo.currentIndex())
        return True

    def reject(self):
        return True

    def getStandardButtons(self):
        return QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
