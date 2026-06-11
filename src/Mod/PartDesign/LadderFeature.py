# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2026 Tungsten CAD Contributors                           *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU Lesser General Public License (LGPL)    *
# *   as published by the Free Software Foundation; either version 2 of     *
# *   the License, or (at your option) any later version.                   *
# *   for detail see the LICENCE text file.                                 *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU Library General Public License for more details.                  *
# *                                                                         *
# *   You should have received a copy of the GNU Library General Public     *
# *   License along with this program; if not, write to the Free Software   *
# *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  *
# *   USA                                                                   *
# *                                                                         *
# ***************************************************************************

import pathlib
import math
import FreeCAD, Part
from FreeCAD import Base

if FreeCAD.GuiUp:
    import FreeCADGui
    from PySide import QtCore, QtGui

__title__ = "PartDesign Ladder Object management"
__author__ = "Tungsten CAD Contributors"
__url__ = "https://www.tungsten3d.com"


def makeLadder(name="Ladder"):
    """makeLadder(name): makes a Ladder feature"""
    obj = FreeCAD.ActiveDocument.addObject("Part::FeaturePython", name)
    _Ladder(obj)
    if FreeCAD.GuiUp:
        _ViewProviderLadder(obj.ViewObject)
    if FreeCAD.GuiUp:
        body = FreeCADGui.ActiveDocument.ActiveView.getActiveObject("pdbody")
        part = FreeCADGui.ActiveDocument.ActiveView.getActiveObject("part")
        if body:
            body.Group = body.Group + [obj]
        elif part:
            part.Group = part.Group + [obj]
    return obj


class CommandLadder:
    "GUI command to create a Ladder"

    def GetResources(self):
        return {
            "Pixmap": "PartDesign_Ladder",
            "MenuText": QtCore.QT_TRANSLATE_NOOP("PartDesign_Ladder", "Ladder"),
            "Accel": "",
            "ToolTip": QtCore.QT_TRANSLATE_NOOP(
                "PartDesign_Ladder", "Creates or edits a parametric ladder"
            ),
        }

    def Activated(self):
        FreeCAD.ActiveDocument.openTransaction("Create ladder")
        FreeCADGui.addModule("LadderFeature")
        FreeCADGui.doCommand("LadderFeature.makeLadder('Ladder')")
        FreeCADGui.doCommand(
            "Gui.activeDocument().setEdit(App.ActiveDocument.ActiveObject.Name,0)"
        )

    def IsActive(self):
        if FreeCAD.ActiveDocument:
            return True
        else:
            return False


class _Ladder:
    "The Ladder object"

    def __init__(self, obj):
        self.Type = "Ladder"
        self._ensure_properties(obj, is_restore=False)
        obj.Proxy = self

    def onDocumentRestored(self, obj):
        self._ensure_properties(obj, is_restore=True)

    def _ensure_properties(self, obj, is_restore):
        def ensure_property(type_, name, group, doc, default):
            if not hasattr(obj, name):
                obj.addProperty(type_, name, group, doc, locked=True)
                if callable(default):
                    setattr(obj, name, default())
                else:
                    setattr(obj, name, default)

        ensure_property(
            "App::PropertyLength",
            "LadderHeight",
            "Ladder",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Overall height of the ladder"),
            default="500 mm",
        )
        ensure_property(
            "App::PropertyLength",
            "LadderWidth",
            "Ladder",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Width between outer edges of rails"),
            default="200 mm",
        )
        ensure_property(
            "App::PropertyInteger",
            "NumberOfRungs",
            "Ladder",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Number of rungs"),
            default=10,
        )
        ensure_property(
            "App::PropertyLength",
            "RungSpacing",
            "Ladder",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Distance between rungs"),
            default="45 mm",
        )
        ensure_property(
            "App::PropertyEnumeration",
            "RungShape",
            "Rungs",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Cross-section shape of the rungs"),
            default=["Round", "Flat"],
        )
        ensure_property(
            "App::PropertyLength",
            "RungDiameter",
            "Rungs",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Diameter of round rungs"),
            default="8 mm",
        )
        ensure_property(
            "App::PropertyLength",
            "RungWidth",
            "Rungs",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Width of flat rungs"),
            default="12 mm",
        )
        ensure_property(
            "App::PropertyLength",
            "RungThickness",
            "Rungs",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Thickness of flat rungs"),
            default="3 mm",
        )
        ensure_property(
            "App::PropertyEnumeration",
            "RailShape",
            "Rails",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Cross-section shape of the rails"),
            default=["Round", "Square", "C-Channel"],
        )
        ensure_property(
            "App::PropertyLength",
            "RailWidth",
            "Rails",
            QtCore.QT_TRANSLATE_NOOP("App::Property", "Width or diameter of the rails"),
            default="10 mm",
        )
        ensure_property(
            "App::PropertyLength",
            "RailThickness",
            "Rails",
            QtCore.QT_TRANSLATE_NOOP(
                "App::Property", "Wall thickness of C-channel rails (ignored for Round/Square)"
            ),
            default="2 mm",
        )
        ensure_property(
            "App::PropertyLink",
            "PathSketch",
            "Path",
            QtCore.QT_TRANSLATE_NOOP(
                "App::Property",
                "Optional sketch path for curved ladders. Leave empty for a straight ladder.",
            ),
            default=None,
        )

    def execute(self, obj):
        path_sketch = obj.PathSketch

        if path_sketch is not None:
            shape = self._build_curved_ladder(obj, path_sketch)
        else:
            shape = self._build_straight_ladder(obj)

        if shape is not None:
            obj.Shape = shape

    def _build_straight_ladder(self, obj):
        height = obj.LadderHeight.Value
        width = obj.LadderWidth.Value
        num_rungs = obj.NumberOfRungs
        rung_spacing = obj.RungSpacing.Value
        rung_shape = obj.RungShape
        rail_shape = obj.RailShape
        rail_w = obj.RailWidth.Value
        rail_t = obj.RailThickness.Value

        if height <= 0 or width <= 0 or num_rungs < 1 or rung_spacing <= 0:
            return None

        shapes = []

        rail_profiles = self._make_rail_profiles(
            rail_shape, rail_w, rail_t, width, height
        )
        for profile in rail_profiles:
            shapes.append(profile)

        rung_profiles = self._make_rung_profiles(
            rung_shape, obj.RungDiameter.Value, obj.RungWidth.Value, obj.RungThickness.Value
        )

        rung_y_positions = []
        if num_rungs == 1:
            rung_y_positions.append(height / 2.0)
        else:
            total_span = rung_spacing * (num_rungs - 1)
            start_y = (height - total_span) / 2.0
            for i in range(num_rungs):
                rung_y_positions.append(start_y + i * rung_spacing)

        for rung_y in rung_y_positions:
            for profile in rung_profiles:
                moved = profile.copy()
                moved.translate(Base.Vector(0, rung_y, 0))
                shapes.append(moved)

        if not shapes:
            return None

        result = shapes[0]
        for s in shapes[1:]:
            result = result.fuse(s)

        return result

    def _build_curved_ladder(self, obj, path_sketch):
        width = obj.LadderWidth.Value
        num_rungs = obj.NumberOfRungs
        rung_shape = obj.RungShape
        rail_shape = obj.RailShape
        rail_w = obj.RailWidth.Value
        rail_t = obj.RailThickness.Value

        try:
            path_wire = path_sketch.Shape.Edges[0]
        except (IndexError, AttributeError):
            return None

        path_length = path_wire.Length()
        if path_length <= 0:
            return None

        shapes = []

        rail_profiles = self._make_rail_profiles(
            rail_shape, rail_w, rail_t, width, path_length
        )

        for profile in rail_profiles:
            try:
                pipe = profile.copy().makePipeShell(
                    [path_wire], makeSolid=True, isFrenet=True
                )
                shapes.append(pipe)
            except Exception:
                pass

        rung_profiles = self._make_rung_profiles(
            rung_shape, obj.RungDiameter.Value, obj.RungWidth.Value, obj.RungThickness.Value
        )

        rung_positions = []
        if num_rungs == 1:
            rung_positions.append(0.5)
        else:
            for i in range(num_rungs):
                rung_positions.append(i / (num_rungs - 1) if num_rungs > 1 else 0.5)

        for t in rung_positions:
            param = t * path_length
            try:
                edge_point = path_wire.valueAt(param)
                edge_tangent = path_wire.tangentAt(param)
            except Exception:
                continue

            for profile in rung_profiles:
                moved = profile.copy()
                moved.translate(edge_point)
                try:
                    pipe = moved.makePipeShell(
                        [path_wire], makeSolid=True, isFrenet=True
                    )
                    shapes.append(pipe)
                except Exception:
                    pass

        if not shapes:
            return None

        result = shapes[0]
        for s in shapes[1:]:
            result = result.fuse(s)

        return result

    def _make_rail_profiles(self, rail_shape, rail_w, rail_t, width, height):
        profiles = []
        half_w = width / 2.0
        z_offset = 0.0

        if rail_shape == "Round":
            r = rail_w / 2.0
            left = Part.makeCircle(r, Base.Vector(-half_w, 0, z_offset), Base.Vector(0, 0, 1))
            right = Part.makeCircle(r, Base.Vector(half_w, 0, z_offset), Base.Vector(0, 0, 1))
            left_face = Part.Face(Part.Wire(left))
            right_face = Part.Face(Part.Wire(right))
            left_solid = left_face.extrude(Base.Vector(0, height, 0))
            right_solid = right_face.extrude(Base.Vector(0, height, 0))
            profiles.append(left_solid)
            profiles.append(right_solid)

        elif rail_shape == "Square":
            half = rail_w / 2.0
            left_profile = Part.makeBox(
                rail_w, height, rail_w,
                Base.Vector(-half_w - half, 0, z_offset - half),
            )
            right_profile = Part.makeBox(
                rail_w, height, rail_w,
                Base.Vector(half_w - half, 0, z_offset - half),
            )
            profiles.append(left_profile)
            profiles.append(right_profile)

        elif rail_shape == "C-Channel":
            outer_w = rail_w
            outer_h = rail_w
            t = rail_t
            left_x = -half_w - outer_w / 2.0
            right_x = half_w - outer_w / 2.0

            for x_base in [left_x, right_x]:
                pts = [
                    Base.Vector(x_base, 0, z_offset - outer_h / 2.0),
                    Base.Vector(x_base + outer_w, 0, z_offset - outer_h / 2.0),
                    Base.Vector(x_base + outer_w, 0, z_offset + outer_h / 2.0),
                    Base.Vector(x_base, 0, z_offset + outer_h / 2.0),
                    Base.Vector(x_base, 0, z_offset - outer_h / 2.0 + t),
                    Base.Vector(x_base + outer_w - t, 0, z_offset - outer_h / 2.0 + t),
                    Base.Vector(x_base + outer_w - t, 0, z_offset + outer_h / 2.0 - t),
                    Base.Vector(x_base, 0, z_offset + outer_h / 2.0 - t),
                    Base.Vector(x_base, 0, z_offset - outer_h / 2.0),
                ]
                wire = Part.makePolygon(pts)
                face = Part.Face(wire)
                solid = face.extrude(Base.Vector(0, height, 0))
                profiles.append(solid)

        return profiles

    def _make_rung_profiles(self, rung_shape, rung_diameter, rung_width, rung_thickness):
        profiles = []

        if rung_shape == "Round":
            r = rung_diameter / 2.0
            circ = Part.makeCircle(r, Base.Vector(0, 0, 0), Base.Vector(0, 0, 1))
            face = Part.Face(Part.Wire(circ))
            profiles.append(face)

        elif rung_shape == "Flat":
            half_w = rung_width / 2.0
            half_t = rung_thickness / 2.0
            box = Part.makeBox(
                rung_width, 1.0, rung_thickness,
                Base.Vector(-half_w, 0, -half_t),
            )
            profiles.append(box)

        return profiles


class _ViewProviderLadder:
    "A View Provider for the Ladder object"

    def __init__(self, vobj):
        vobj.Proxy = self

    def getIcon(self):
        return ":/icons/PartDesign_Ladder.svg"

    def attach(self, vobj):
        self.ViewObject = vobj
        self.Object = vobj.Object

    def setEdit(self, vobj, mode):
        taskd = _LadderTaskPanel(self.Object, mode)
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


class _LadderTaskPanel:
    """The editmode TaskPanel for Ladder objects"""

    def __init__(self, obj, mode):
        self.obj = obj

        self.form = FreeCADGui.PySideUic.loadUi(str(pathlib.Path(__file__).with_suffix(".ui")))
        self.form.setWindowIcon(QtGui.QIcon(":/icons/PartDesign_Ladder.svg"))
        self._setup_connections()
        self.update()

        if mode == 0:
            self.obj.Proxy.execute(self.obj)
            FreeCADGui.SendMsgToActiveView("ViewFit")

    def _setup_connections(self):
        def assign_value(prop_name, fit_view=False):
            def setter(value):
                setattr(self.obj, prop_name, value)
                self.obj.Proxy.execute(self.obj)
                if fit_view:
                    FreeCADGui.SendMsgToActiveView("ViewFit")
            return setter

        def assign_index(prop_name):
            setter = assign_value(prop_name)
            def transform(value):
                setter(value)
            return transform

        self.form.spinBox_NumberOfRungs.valueChanged.connect(assign_value("NumberOfRungs", fit_view=True))
        self.form.Quantity_RungSpacing.valueChanged.connect(assign_value("RungSpacing", fit_view=True))
        self.form.Quantity_LadderHeight.valueChanged.connect(assign_value("LadderHeight", fit_view=True))
        self.form.Quantity_LadderWidth.valueChanged.connect(assign_value("LadderWidth", fit_view=True))

        self.form.comboBox_RungShape.currentIndexChanged.connect(assign_index("RungShape"))
        self.form.Quantity_RungDiameter.valueChanged.connect(assign_value("RungDiameter"))
        self.form.Quantity_RungWidth.valueChanged.connect(assign_value("RungWidth"))
        self.form.Quantity_RungThickness.valueChanged.connect(assign_value("RungThickness"))

        self.form.comboBox_RailShape.currentIndexChanged.connect(assign_index("RailShape"))
        self.form.Quantity_RailWidth.valueChanged.connect(assign_value("RailWidth"))
        self.form.Quantity_RailThickness.valueChanged.connect(assign_value("RailThickness"))

    def transferTo(self):
        "Transfer from the dialog to the object"
        self.obj.LadderHeight = self.form.Quantity_LadderHeight.text()
        self.obj.LadderWidth = self.form.Quantity_LadderWidth.text()
        self.obj.NumberOfRungs = self.form.spinBox_NumberOfRungs.value()
        self.obj.RungSpacing = self.form.Quantity_RungSpacing.text()

        self.obj.RungShape = self.form.comboBox_RungShape.currentText()
        self.obj.RungDiameter = self.form.Quantity_RungDiameter.text()
        self.obj.RungWidth = self.form.Quantity_RungWidth.text()
        self.obj.RungThickness = self.form.Quantity_RungThickness.text()

        self.obj.RailShape = self.form.comboBox_RailShape.currentText()
        self.obj.RailWidth = self.form.Quantity_RailWidth.text()
        self.obj.RailThickness = self.form.Quantity_RailThickness.text()

    def transferFrom(self):
        "Transfer from the object to the dialog"
        self.form.Quantity_LadderHeight.setText(self.obj.LadderHeight.UserString)
        self.form.Quantity_LadderWidth.setText(self.obj.LadderWidth.UserString)
        self.form.spinBox_NumberOfRungs.setValue(self.obj.NumberOfRungs)
        self.form.Quantity_RungSpacing.setText(self.obj.RungSpacing.UserString)

        idx = self.form.comboBox_RungShape.findText(self.obj.RungShape)
        if idx >= 0:
            self.form.comboBox_RungShape.setCurrentIndex(idx)
        self.form.Quantity_RungDiameter.setText(self.obj.RungDiameter.UserString)
        self.form.Quantity_RungWidth.setText(self.obj.RungWidth.UserString)
        self.form.Quantity_RungThickness.setText(self.obj.RungThickness.UserString)

        idx = self.form.comboBox_RailShape.findText(self.obj.RailShape)
        if idx >= 0:
            self.form.comboBox_RailShape.setCurrentIndex(idx)
        self.form.Quantity_RailWidth.setText(self.obj.RailWidth.UserString)
        self.form.Quantity_RailThickness.setText(self.obj.RailThickness.UserString)

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
        FreeCADGui.ActiveDocument.resetEdit()
        FreeCAD.ActiveDocument.abortTransaction()
