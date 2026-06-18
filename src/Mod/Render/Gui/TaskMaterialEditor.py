import FreeCAD as App
import FreeCADGui as Gui
from PySide6 import QtCore, QtGui, QtWidgets


class TaskMaterialEditor:
    def __init__(self):
        self.form = QtWidgets.QWidget()
        self.form.setWindowTitle("Material Editor")
        layout = QtWidgets.QVBoxLayout(self.form)

        self._obj = None
        self._sel = Gui.Selection.getSelection()
        if self._sel:
            self._obj = self._sel[0].Object

        if self._obj:
            label = QtWidgets.QLabel(f"Editing: {self._obj.Label}")
            label.setStyleSheet("font-weight: bold;")
            layout.addWidget(label)

        # PBR Parameters group
        pbrGroup = QtWidgets.QGroupBox("PBR Parameters")
        pbrLayout = QtWidgets.QFormLayout(pbrGroup)

        self.baseColorBtn = QtWidgets.QPushButton()
        self.baseColorBtn.setStyleSheet("background-color: rgb(200, 200, 200)")
        self.baseColorBtn.clicked.connect(self._pickBaseColor)
        pbrLayout.addRow("Base Color:", self.baseColorBtn)

        self.metallicSpin = QtWidgets.QDoubleSpinBox()
        self.metallicSpin.setRange(0, 1)
        self.metallicSpin.setValue(0)
        self.metallicSpin.setSingleStep(0.05)
        pbrLayout.addRow("Metallic:", self.metallicSpin)

        self.roughnessSpin = QtWidgets.QDoubleSpinBox()
        self.roughnessSpin.setRange(0, 1)
        self.roughnessSpin.setValue(0.5)
        self.roughnessSpin.setSingleStep(0.05)
        pbrLayout.addRow("Roughness:", self.roughnessSpin)

        self.specularSpin = QtWidgets.QDoubleSpinBox()
        self.specularSpin.setRange(0, 1)
        self.specularSpin.setValue(0.5)
        self.specularSpin.setSingleStep(0.05)
        pbrLayout.addRow("Specular:", self.specularSpin)

        self.emissiveBtn = QtWidgets.QPushButton()
        self.emissiveBtn.setStyleSheet("background-color: rgb(0, 0, 0)")
        self.emissiveBtn.clicked.connect(self._pickEmissive)
        pbrLayout.addRow("Emissive:", self.emissiveBtn)

        self.opacitySpin = QtWidgets.QDoubleSpinBox()
        self.opacitySpin.setRange(0, 1)
        self.opacitySpin.setValue(1)
        self.opacitySpin.setSingleStep(0.05)
        pbrLayout.addRow("Opacity:", self.opacitySpin)

        layout.addWidget(pbrGroup)

        # Texture group
        texGroup = QtWidgets.QGroupBox("Textures")
        texLayout = QtWidgets.QFormLayout(texGroup)

        self.baseTexEdit = QtWidgets.QLineEdit()
        self.baseTexBtn = QtWidgets.QPushButton("Browse...")
        self.baseTexBtn.clicked.connect(lambda: self._browseTexture(self.baseTexEdit))
        baseTexLayout = QtWidgets.QHBoxLayout()
        baseTexLayout.addWidget(self.baseTexEdit)
        baseTexLayout.addWidget(self.baseTexBtn)
        texLayout.addRow("Base Color:", baseTexLayout)

        self.normalTexEdit = QtWidgets.QLineEdit()
        self.normalTexBtn = QtWidgets.QPushButton("Browse...")
        self.normalTexBtn.clicked.connect(
            lambda: self._browseTexture(self.normalTexEdit)
        )
        normalTexLayout = QtWidgets.QHBoxLayout()
        normalTexLayout.addWidget(self.normalTexEdit)
        normalTexLayout.addWidget(self.normalTexBtn)
        texLayout.addRow("Normal:", normalTexLayout)

        layout.addWidget(texGroup)

        # Material Presets
        presetGroup = QtWidgets.QGroupBox("Presets")
        presetLayout = QtWidgets.QHBoxLayout(presetGroup)

        for name in ["Steel", "Gold", "Plastic", "Glass", "Wood"]:
            btn = QtWidgets.QPushButton(name)
            btn.clicked.connect(lambda checked, n=name: self._applyPreset(n))
            presetLayout.addWidget(btn)

        layout.addWidget(presetGroup)
        layout.addStretch()

        self._baseColor = (200, 200, 200)
        self._emissive = (0, 0, 0)

    def _pickBaseColor(self):
        from PySide6.QtWidgets import QColorDialog

        color = QColorDialog.getColor()
        if color.isValid():
            self._baseColor = (color.red(), color.green(), color.blue())
            self.baseColorBtn.setStyleSheet(
                f"background-color: rgb({color.red()}, {color.green()}, {color.blue()})"
            )

    def _pickEmissive(self):
        from PySide6.QtWidgets import QColorDialog

        color = QColorDialog.getColor()
        if color.isValid():
            self._emissive = (color.red(), color.green(), color.blue())
            self.emissiveBtn.setStyleSheet(
                f"background-color: rgb({color.red()}, {color.green()}, {color.blue()})"
            )

    def _browseTexture(self, lineEdit):
        from PySide6.QtWidgets import QFileDialog

        path, _ = QFileDialog.getOpenFileName(
            self.form,
            "Open Texture",
            "",
            "Images (*.png *.jpg *.jpeg *.exr *.hdr);;All Files (*)",
        )
        if path:
            lineEdit.setText(path)

    def _applyPreset(self, name):
        presets = {
            "Steel": {"color": (180, 180, 180), "metallic": 1.0, "roughness": 0.3},
            "Gold": {"color": (255, 215, 0), "metallic": 1.0, "roughness": 0.2},
            "Plastic": {"color": (200, 50, 50), "metallic": 0.0, "roughness": 0.4},
            "Glass": {"color": (220, 220, 255), "metallic": 0.0, "roughness": 0.05},
            "Wood": {"color": (139, 90, 43), "metallic": 0.0, "roughness": 0.7},
        }
        if name in presets:
            p = presets[name]
            self._baseColor = p["color"]
            self.baseColorBtn.setStyleSheet(
                f"background-color: rgb({p['color'][0]}, {p['color'][1]}, {p['color'][2]})"
            )
            self.metallicSpin.setValue(p["metallic"])
            self.roughnessSpin.setValue(p["roughness"])

    def accept(self):
        if self._obj:
            r, g, b = self._baseColor
            er, eg, eb = self._emissive
            try:
                import Materials

                mat = Materials.Material()
                mat.setProperty(
                    "DiffuseColor", f"({r / 255.0}, {g / 255.0}, {b / 255.0})"
                )
                mat.setProperty(
                    "Shininess", str((1 - self.roughnessSpin.value()) * 100)
                )
                self._obj.ViewObject.ShapeAppearance = mat
            except Exception:
                mat = App.Material()
                mat.diffuseColor = App.Color(r / 255.0, g / 255.0, b / 255.0)
                mat.shininess = (1 - self.roughnessSpin.value()) * 100
                mat.emissiveColor = App.Color(er / 255.0, eg / 255.0, eb / 255.0)
                self._obj.ViewObject.ShapeAppearance = [mat]
        return True

    def reject(self):
        return True

    def getStandardButtons(self):
        return QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel
