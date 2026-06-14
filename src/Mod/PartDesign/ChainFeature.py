# SPDX-License-Identifier: LGPL-2.1-or-later

import FreeCAD, Part
from fchain import chain

if FreeCAD.GuiUp:
    import FreeCADGui
    from PySide import QtCore, QtGui
    from FreeCADGui import PySideUic as uic

__title__ = "PartDesign ChainObject management"
__author__ = "Tungsten CAD Contributors"
__url__ = "https://www.freecad.org"


def makeChain(name):
    """makeChain(name): makes a Chain"""
    obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython", name)
    _Chain(obj)
    if FreeCAD.GuiUp:
        _ViewProviderChain(obj.ViewObject)
    if FreeCAD.GuiUp:
        body = FreeCADGui.ActiveDocument.ActiveView.getActiveObject("pdbody")
        part = FreeCADGui.ActiveDocument.ActiveView.getActiveObject("part")
        if body:
            body.Group = body.Group + [obj]
        elif part:
            part.Group = part.Group + [obj]
    return obj


class CommandChain:
    """GUI command to create a Chain"""

    def GetResources(self):
        return {
            "Pixmap": "PartDesign_Sprocket",
            "MenuText": QtCore.QT_TRANSLATE_NOOP("PartDesign_Chain", "Chain"),
            "Accel": "",
            "ToolTip": QtCore.QT_TRANSLATE_NOOP(
                "PartDesign_Chain", "Creates or edits a chain definition."
            ),
        }

    def Activated(self):
        FreeCAD.ActiveDocument.openTransaction("Create Chain")
        FreeCADGui.addModule("ChainFeature")
        FreeCADGui.doCommand("ChainFeature.makeChain('Chain')")
        FreeCADGui.doCommand(
            "Gui.activeDocument().setEdit(App.ActiveDocument.ActiveObject.Name,0)"
        )

    def IsActive(self):
        if FreeCAD.ActiveDocument:
            return True
        else:
            return False


class _Chain:
    """The Chain object"""

    def __init__(self, obj):
        self.Type = "Chain"
        self._ensure_properties(obj, is_restore=False)
        obj.Proxy = self

    def onDocumentRestored(self, obj):
        """hook used to migrate older versions of this object"""
        self._ensure_properties(obj, is_restore=True)

    def _ensure_properties(self, obj, is_restore):
        def ensure_property(type_, name, doc, default):
            if not hasattr(obj, name):
                obj.addProperty(type_, name, "Chain", doc, locked=True)
                if callable(default):
                    setattr(obj, name, default())
                else:
                    setattr(obj, name, default)

        ensure_property(
            "App::PropertyLength",
            "LinkLength",
            "Overall length of each link (long axis)",
            "20 mm",
        )
        ensure_property(
            "App::PropertyLength",
            "LinkWidth",
            "Overall width of each link (short axis)",
            "10 mm",
        )
        ensure_property(
            "App::PropertyLength",
            "WireDiameter",
            "Diameter of the circular cross-section forming each link",
            "3 mm",
        )
        ensure_property(
            "App::PropertyInteger",
            "NumberOfLinks",
            "Total number of links in the chain",
            10,
        )
        ensure_property(
            "App::PropertyLength",
            "Gap",
            "Clearance between adjacent links",
            "0.5 mm",
        )
        ensure_property(
            "App::PropertyBool",
            "AlternateRotation",
            "Rotate every other link 90 degrees for physical interlocking",
            True,
        )
        ensure_property(
            "App::PropertyLink",
            "PathObject",
            "Optional path for the chain to follow. If empty, chain is straight along X axis.",
            None,
        )

    def execute(self, obj):
        path_shape = None
        if obj.PathObject is not None:
            try:
                path_shape = obj.PathObject.Shape
            except Exception:
                path_shape = None

        links = chain.createChain(
            link_length=obj.LinkLength.Value,
            link_width=obj.LinkWidth.Value,
            wire_diameter=obj.WireDiameter.Value,
            num_links=obj.NumberOfLinks,
            gap=obj.Gap.Value,
            path_shape=path_shape,
            alternate_rotation=obj.AlternateRotation,
        )

        if not links:
            obj.Shape = Part.Shape()
            return

        compound = Part.makeCompound(links)
        obj.Shape = compound


class _ViewProviderChain:
    """A View Provider for the Chain object"""

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/PartDesign_Sprocket.svg"

    def attach(self, vobj):
        self.ViewObject = vobj
        self.Object = vobj.Object

    def setEdit(self, vobj, mode):
        taskd = _ChainTaskPanel(self.Object, mode)
        taskd.obj = vobj.Object
        taskd.update()
        FreeCADGui.Control.showDialog(taskd)
        return True

    def unsetEdit(self, vobj, mode):
        FreeCADGui.Control.closeDialog()
        return

    def dumps(self):
        return None

    def loads(self, state):
        return None


class _ChainTaskPanel:
    """The editmode TaskPanel for Chain objects"""

    def __init__(self, obj, mode):
        self.obj = obj

        self.form = FreeCADGui.PySideUic.loadUi(
            FreeCAD.getHomePath() + "Mod/PartDesign/ChainFeature.ui"
        )
        self.form.setWindowIcon(QtGui.QIcon(":/icons/PartDesign_Sprocket.svg"))

        def assignValue(property_name, fitView=False):
            """Returns a function that assigns a value to the given property"""

            def assigner(value):
                setattr(self.obj, property_name, value)
                self.obj.Proxy.execute(self.obj)
                if fitView:
                    FreeCAD.Gui.SendMsgToActiveView("ViewFit")

            return assigner

        def assignIndexAsBool(property_name):
            """Variant that transforms a Yes/No combobox index to a bool"""
            assigner = assignValue(property_name)

            def transformingAssigner(value):
                assigner(True if value == 0 else False)

            return transformingAssigner

        self.form.Quantity_LinkLength.valueChanged.connect(
            assignValue("LinkLength", fitView=True)
        )
        self.form.Quantity_LinkWidth.valueChanged.connect(
            assignValue("LinkWidth", fitView=True)
        )
        self.form.Quantity_WireDiameter.valueChanged.connect(
            assignValue("WireDiameter", fitView=True)
        )
        self.form.spinBox_NumberOfLinks.valueChanged.connect(
            assignValue("NumberOfLinks", fitView=True)
        )
        self.form.Quantity_Gap.valueChanged.connect(assignValue("Gap"))
        self.form.comboBox_AlternateRotation.currentIndexChanged.connect(
            assignIndexAsBool("AlternateRotation")
        )

        self.update()

        if mode == 0:  # fresh created
            self.obj.Proxy.execute(self.obj)  # calculate once
            FreeCAD.Gui.SendMsgToActiveView("ViewFit")

    def transferTo(self):
        """Transfer from the dialog to the object"""
        self.obj.LinkLength = self.form.Quantity_LinkLength.text()
        self.obj.LinkWidth = self.form.Quantity_LinkWidth.text()
        self.obj.WireDiameter = self.form.Quantity_WireDiameter.text()
        self.obj.NumberOfLinks = self.form.spinBox_NumberOfLinks.value()
        self.obj.Gap = self.form.Quantity_Gap.text()
        self.obj.AlternateRotation = (
            True if self.form.comboBox_AlternateRotation.currentIndex() == 0 else False
        )

    def transferFrom(self):
        """Transfer from the object to the dialog"""
        self.form.Quantity_LinkLength.setText(self.obj.LinkLength.UserString)
        self.form.Quantity_LinkWidth.setText(self.obj.LinkWidth.UserString)
        self.form.Quantity_WireDiameter.setText(self.obj.WireDiameter.UserString)
        self.form.spinBox_NumberOfLinks.setValue(self.obj.NumberOfLinks)
        self.form.Quantity_Gap.setText(self.obj.Gap.UserString)
        self.form.comboBox_AlternateRotation.setCurrentIndex(
            0 if self.obj.AlternateRotation else 1
        )

    def getStandardButtons(self):
        return (
            QtGui.QDialogButtonBox.Ok
            | QtGui.QDialogButtonBox.Cancel
            | QtGui.QDialogButtonBox.Apply
        )

    def clicked(self, button):
        if button == QtGui.QDialogButtonBox.Apply:
            self.transferTo()
            self.obj.Proxy.execute(self.obj)

    def update(self):
        self.transferFrom()

    def accept(self):
        self.transferTo()
        FreeCAD.ActiveDocument.recompute()
        FreeCADGui.ActiveDocument.resetEdit()

    def reject(self):
        FreeCAD.ActiveDocument.removeObject(self.obj.Name)
        FreeCAD.ActiveDocument.recompute()
        FreeCADGui.ActiveDocument.resetEdit()
        FreeCAD.ActiveDocument.abortTransaction()
