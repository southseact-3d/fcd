# SPDX-License-Identifier: LGPL-2.1-or-later

# ***************************************************************************
# *   Copyright (c) 2025 sliptonic <shopinthewoods@gmail.com>               *
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

"""QWizardPage subclasses for the CAM Walkthrough wizard."""

import FreeCAD
import FreeCADGui
import Path
import Path.Preferences as PathPreferences
from PySide import QtCore, QtGui, QtWidgets

translate = FreeCAD.Qt.translate

if False:
    Path.Log.setLevel(Path.Log.Level.DEBUG, Path.Log.thisModule())
    Path.Log.trackModule(Path.Log.thisModule())
else:
    Path.Log.setLevel(Path.Log.Level.INFO, Path.Log.thisModule())


# ---------------------------------------------------------------------------
# Operation type metadata
# ---------------------------------------------------------------------------

OPERATION_TYPES = [
    {
        "id": "Profile",
        "label": "Profile",
        "description": "Cuts along the outer boundary (or selected edges) of the model.",
        "module": "Path.Op.Profile",
    },
    {
        "id": "Pocket",
        "label": "Pocket",
        "description": "Clears material inside a closed region.",
        "module": "Path.Op.PocketShape",
    },
    {
        "id": "FaceMill",
        "label": "Face Mill",
        "description": "Mills the top face of the model flat.",
        "module": "Path.Op.MillFace",
    },
    {
        "id": "Adaptive",
        "label": "Adaptive",
        "description": "High-speed adaptive clearing of large amounts of material.",
        "module": "Path.Op.Adaptive",
    },
    {
        "id": "Drilling",
        "label": "Drilling",
        "description": "Drills holes at selected circular features.",
        "module": "Path.Op.Drilling",
    },
    {
        "id": "Engrave",
        "label": "Engrave",
        "description": "Engraves along selected edges or wires.",
        "module": "Path.Op.Engrave",
    },
]


# ===================================================================
#  Step 1 - Select Body
# ===================================================================


class SelectBodyPage(QtWidgets.QWizardPage):
    """Page 1: Select one or more solid bodies from the 3D view."""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setTitle(translate("CAM_Walkthrough", "Step 1: Select Body"))
        self.setSubTitle(
            translate(
                "CAM_Walkthrough",
                "Select one or more solid bodies in the 3D view that you want to machine.",
            )
        )
        self._selected = []

        layout = QtWidgets.QVBoxLayout(self)

        self._info_label = QtWidgets.QLabel(
            translate(
                "CAM_Walkthrough",
                "Select solids in the 3D view, then click Refresh below.",
            )
        )
        self._info_label.setWordWrap(True)
        layout.addWidget(self._info_label)

        self._selected_label = QtWidgets.QLabel()
        self._selected_label.setWordWrap(True)
        layout.addWidget(self._selected_label)

        self._refresh_btn = QtWidgets.QPushButton(
            translate("CAM_Walkthrough", "Refresh Selection")
        )
        self._refresh_btn.clicked.connect(self._refresh)
        layout.addWidget(self._refresh_btn)

        self._clear_btn = QtWidgets.QPushButton(translate("CAM_Walkthrough", "Clear"))
        self._clear_btn.clicked.connect(self._clear)
        layout.addWidget(self._clear_btn)

        layout.addStretch()

    # -- helpers --

    def _refresh(self):
        sel = FreeCADGui.Selection.getSelection()
        solids = [o for o in sel if hasattr(o, "Shape") and o.Shape.Solids]
        self._selected = solids
        if solids:
            names = ", ".join(o.Label for o in solids)
            self._selected_label.setText(
                translate("CAM_Walkthrough", "Selected: <b>%1</b>").replace("%1", names)
            )
        else:
            self._selected_label.setText(
                translate("CAM_Walkthrough", "<i>No solids selected.</i>")
            )
        self.completeChanged.emit()

    def _clear(self):
        self._selected = []
        self._selected_label.setText("")
        self.completeChanged.emit()

    def getSelectedBodies(self):
        return list(self._selected)

    def isComplete(self):
        return len(self._selected) > 0


# ===================================================================
#  Step 2 - Job Setup
# ===================================================================


class JobSetupPage(QtWidgets.QWizardPage):
    """Page 2: Stock size and post processor selection.

    Args:
        bodies_callable: A callable returning the list of selected body objects.
    """

    def __init__(self, bodies_callable=None, parent=None):
        super().__init__(parent)
        self._bodies_callable = bodies_callable
        self.setTitle(translate("CAM_Walkthrough", "Step 2: Job Setup"))
        self.setSubTitle(
            translate(
                "CAM_Walkthrough",
                "Configure stock size and post processor.",
            )
        )

        layout = QtWidgets.QFormLayout(self)

        # -- Stock padding --
        self._padding_x = QtWidgets.QDoubleSpinBox()
        self._padding_x.setSuffix(" mm")
        self._padding_x.setRange(0, 1000)
        self._padding_x.setValue(5.0)
        self._padding_x.valueChanged.connect(lambda: self.completeChanged.emit())
        layout.addRow(
            translate("CAM_Walkthrough", "Stock padding (X/Y):"), self._padding_x
        )

        self._padding_z_top = QtWidgets.QDoubleSpinBox()
        self._padding_z_top.setSuffix(" mm")
        self._padding_z_top.setRange(0, 1000)
        self._padding_z_top.setValue(1.0)
        self._padding_z_top.valueChanged.connect(lambda: self.completeChanged.emit())
        layout.addRow(
            translate("CAM_Walkthrough", "Stock padding (Z top):"), self._padding_z_top
        )

        self._padding_z_neg = QtWidgets.QDoubleSpinBox()
        self._padding_z_neg.setSuffix(" mm")
        self._padding_z_neg.setRange(0, 1000)
        self._padding_z_neg.setValue(0.5)
        self._padding_z_neg.valueChanged.connect(lambda: self.completeChanged.emit())
        layout.addRow(
            translate("CAM_Walkthrough", "Stock padding (Z bottom):"),
            self._padding_z_neg,
        )

        # -- Post processor --
        self._post_combo = QtWidgets.QComboBox()
        posts = PathPreferences.allEnabledPostProcessors()
        default = PathPreferences.defaultPostProcessor()
        default_idx = 0
        for i, p in enumerate(posts):
            self._post_combo.addItem(p)
            if p == default:
                default_idx = i
        if posts:
            self._post_combo.setCurrentIndex(default_idx)
        self._post_combo.currentIndexChanged.connect(
            lambda: self.completeChanged.emit()
        )
        layout.addRow(translate("CAM_Walkthrough", "Post Processor:"), self._post_combo)

        # -- Read-only stock preview --
        self._stock_preview = QtWidgets.QLabel()
        self._stock_preview.setWordWrap(True)
        layout.addRow(
            translate("CAM_Walkthrough", "Stock dimensions:"), self._stock_preview
        )

        self._update_preview()

    def _update_preview(self):
        bodies = self._get_bodies()
        if not bodies:
            self._stock_preview.setText(translate("CAM_Walkthrough", "N/A"))
            return
        bb = self._compute_stock_bounds()
        if bb is None:
            self._stock_preview.setText(translate("CAM_Walkthrough", "N/A"))
            return
        x_len = bb.XLength + 2 * self._padding_x.value()
        y_len = bb.YLength + 2 * self._padding_x.value()
        z_len = bb.ZLength + self._padding_z_top.value() + self._padding_z_neg.value()
        self._stock_preview.setText(
            translate("CAM_Walkthrough", "%1 x %2 x %3 mm")
            .replace("%1", f"{x_len:.1f}")
            .replace("%2", f"{y_len:.1f}")
            .replace("%3", f"{z_len:.1f}")
        )

    def _get_bodies(self):
        if self._bodies_callable is not None:
            return self._bodies_callable()
        return []

    def _compute_stock_bounds(self):
        import Part

        bodies = self._get_bodies()
        shapes = []
        for body in bodies:
            if hasattr(body, "Shape"):
                shapes.append(body.Shape)
        if not shapes:
            return None
        compound = Part.makeCompound(shapes)
        return compound.BoundBox

    def getPaddingXY(self):
        return self._padding_x.value()

    def getPaddingZTop(self):
        return self._padding_z_top.value()

    def getPaddingZBottom(self):
        return self._padding_z_neg.value()

    def getPostProcessor(self):
        return self._post_combo.currentText()

    def getStockBounds(self):
        return self._compute_stock_bounds()

    def isComplete(self):
        return self._post_combo.count() > 0


# ===================================================================
#  Step 3 - Operation Type
# ===================================================================


class OperationTypePage(QtWidgets.QWizardPage):
    """Page 3: Choose which type of CAM operation to perform."""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setTitle(translate("CAM_Walkthrough", "Step 3: Operation Type"))
        self.setSubTitle(
            translate(
                "CAM_Walkthrough",
                "Choose the type of machining operation.",
            )
        )

        layout = QtWidgets.QVBoxLayout(self)

        self._button_group = QtWidgets.QButtonGroup(self)
        first = True
        for op in OPERATION_TYPES:
            rb = QtWidgets.QRadioButton(op["label"])
            rb.setToolTip(op["description"])
            self._button_group.addButton(rb)
            self._button_group.setId(rb, OPERATION_TYPES.index(op))
            if first:
                rb.setChecked(True)
                first = False
            row_layout = QtWidgets.QHBoxLayout()
            row_layout.addWidget(rb)
            desc = QtWidgets.QLabel(f"<i>{op['description']}</i>")
            desc.setWordWrap(True)
            row_layout.addWidget(desc, 1)
            layout.addLayout(row_layout)

        self._button_group.idClicked.connect(lambda: self.completeChanged.emit())
        layout.addStretch()

    def getOperationType(self):
        idx = self._button_group.checkedId()
        if 0 <= idx < len(OPERATION_TYPES):
            return OPERATION_TYPES[idx]
        return OPERATION_TYPES[0]

    def isComplete(self):
        return self._button_group.checkedId() >= 0


# ===================================================================
#  Step 4 - Tool Selection
# ===================================================================


class ToolSelectionPage(QtWidgets.QWizardPage):
    """Page 4: Select a cutting tool from the library."""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setTitle(translate("CAM_Walkthrough", "Step 4: Tool Selection"))
        self.setSubTitle(
            translate(
                "CAM_Walkthrough",
                "Select a cutting tool from the library.",
            )
        )

        layout = QtWidgets.QVBoxLayout(self)

        self._tool_label = QtWidgets.QLabel(
            translate("CAM_Walkthrough", "<i>No tool selected.</i>")
        )
        self._tool_label.setWordWrap(True)
        layout.addWidget(self._tool_label)

        self._select_btn = QtWidgets.QPushButton(
            translate("CAM_Walkthrough", "Browse Tool Library...")
        )
        self._select_btn.clicked.connect(self._browse_tool)
        layout.addWidget(self._select_btn)

        # -- Feeds / Speeds (pre-filled from tool defaults) --
        form = QtWidgets.QFormLayout()

        self._spindle_speed = QtWidgets.QSpinBox()
        self._spindle_speed.setRange(0, 100000)
        self._spindle_speed.setValue(1000)
        self._spindle_speed.setSuffix(" RPM")
        self._spindle_speed.valueChanged.connect(lambda: self.completeChanged.emit())
        form.addRow(translate("CAM_Walkthrough", "Spindle Speed:"), self._spindle_speed)

        self._feed_rate = QtWidgets.QDoubleSpinBox()
        self._feed_rate.setRange(0, 100000)
        self._feed_rate.setValue(300.0)
        self._feed_rate.setSuffix(" mm/min")
        self._feed_rate.valueChanged.connect(lambda: self.completeChanged.emit())
        form.addRow(translate("CAM_Walkthrough", "Feed Rate:"), self._feed_rate)

        self._plunge_rate = QtWidgets.QDoubleSpinBox()
        self._plunge_rate.setRange(0, 100000)
        self._plunge_rate.setValue(150.0)
        self._plunge_rate.setSuffix(" mm/min")
        self._plunge_rate.valueChanged.connect(lambda: self.completeChanged.emit())
        form.addRow(translate("CAM_Walkthrough", "Plunge Rate:"), self._plunge_rate)

        # -- Coolant --
        self._coolant_combo = QtWidgets.QComboBox()
        self._coolant_combo.addItems(["None", "Flood", "Mist"])
        form.addRow(translate("CAM_Walkthrough", "Coolant:"), self._coolant_combo)

        layout.addLayout(form)
        layout.addStretch()

        self._tool = None

    def _browse_tool(self):
        from Path.Tool.toolbit.ui.selector import ToolBitSelector

        selector = ToolBitSelector(parent=self, compact=True)
        if selector.exec_() == QtWidgets.QDialog.Accepted:
            toolbit = selector.get_selected_tool()
            if toolbit:
                self._tool = toolbit
                self._tool_label.setText(
                    translate("CAM_Walkthrough", "Tool: <b>%1</b>").replace(
                        "%1", toolbit.Label
                    )
                )
                self._fill_from_tool(toolbit)
                self.completeChanged.emit()

    def _fill_from_tool(self, toolbit):
        if hasattr(toolbit, "SpindleSpeed") and toolbit.SpindleSpeed > 0:
            self._spindle_speed.setValue(int(toolbit.SpindleSpeed))
        if hasattr(toolbit, "HorizontalFeed") and toolbit.HorizontalFeed > 0:
            self._feed_rate.setValue(float(toolbit.HorizontalFeed.getValueAs("mm/min")))
        if hasattr(toolbit, "VerticalFeed") and toolbit.VerticalFeed > 0:
            self._plunge_rate.setValue(float(toolbit.VerticalFeed.getValueAs("mm/min")))

    def getTool(self):
        return self._tool

    def getSpindleSpeed(self):
        return self._spindle_speed.value()

    def getFeedRate(self):
        return self._feed_rate.value()

    def getPlungeRate(self):
        return self._plunge_rate.value()

    def getCoolant(self):
        return self._coolant_combo.currentText()

    def isComplete(self):
        return self._tool is not None


# ===================================================================
#  Step 5 - Cutting Parameters (depths, step-down)
# ===================================================================


class CuttingParamsPage(QtWidgets.QWizardPage):
    """Page 5: Cutting depths and step-down."""

    def __init__(self, stock_bounds, parent=None):
        super().__init__(parent)
        self._stock_bounds = stock_bounds
        self.setTitle(translate("CAM_Walkthrough", "Step 5: Cutting Parameters"))
        self.setSubTitle(
            translate(
                "CAM_Walkthrough",
                "Set step-down and additional cutting parameters.",
            )
        )

        layout = QtWidgets.QFormLayout(self)

        self._step_down = QtWidgets.QDoubleSpinBox()
        self._step_down.setSuffix(" mm")
        self._step_down.setRange(0.01, 1000)
        self._step_down.setValue(2.0)
        self._step_down.setSingleStep(0.5)
        self._step_down.valueChanged.connect(lambda: self.completeChanged.emit())
        layout.addRow(
            translate("CAM_Walkthrough", "Step Down (depth per pass):"), self._step_down
        )

        self._step_over = QtWidgets.QDoubleSpinBox()
        self._step_over.setRange(1, 100)
        self._step_over.setValue(50.0)
        self._step_over.setSuffix(" %")
        self._step_over.valueChanged.connect(lambda: self.completeChanged.emit())
        layout.addRow(
            translate("CAM_Walkthrough", "Step Over (% of tool diameter):"),
            self._step_over,
        )

        self._extra_offset = QtWidgets.QDoubleSpinBox()
        self._extra_offset.setSuffix(" mm")
        self._extra_offset.setRange(-100, 100)
        self._extra_offset.setValue(0.0)
        layout.addRow(translate("CAM_Walkthrough", "Extra Offset:"), self._extra_offset)

        layout.addStretch()

    def getStepDown(self):
        return self._step_down.value()

    def getStepOver(self):
        return self._step_over.value()

    def getExtraOffset(self):
        return self._extra_offset.value()

    def isComplete(self):
        return self._step_down.value() > 0


# ===================================================================
#  Step 6 - Heights & Depths
# ===================================================================


class HeightsDepthsPage(QtWidgets.QWizardPage):
    """Page 6: Clearance height, safe height, start depth, final depth."""

    def __init__(self, stock_bounds, parent=None):
        super().__init__(parent)
        self._stock_bounds = stock_bounds
        self.setTitle(translate("CAM_Walkthrough", "Step 6: Heights & Depths"))
        self.setSubTitle(
            translate(
                "CAM_Walkthrough",
                "Configure safe travel heights and cutting depths.",
            )
        )

        layout = QtWidgets.QFormLayout(self)

        z_max = stock_bounds.ZMax if stock_bounds else 0.0
        z_min = stock_bounds.ZMin if stock_bounds else -10.0

        self._clearance_height = QtWidgets.QDoubleSpinBox()
        self._clearance_height.setSuffix(" mm")
        self._clearance_height.setRange(-10000, 10000)
        self._clearance_height.setValue(round(z_max + 5.0, 2))
        layout.addRow(
            translate("CAM_Walkthrough", "Clearance Height:"), self._clearance_height
        )

        self._safe_height = QtWidgets.QDoubleSpinBox()
        self._safe_height.setSuffix(" mm")
        self._safe_height.setRange(-10000, 10000)
        self._safe_height.setValue(round(z_max + 3.0, 2))
        layout.addRow(translate("CAM_Walkthrough", "Safe Height:"), self._safe_height)

        self._start_depth = QtWidgets.QDoubleSpinBox()
        self._start_depth.setSuffix(" mm")
        self._start_depth.setRange(-10000, 10000)
        self._start_depth.setValue(round(z_max, 2))
        layout.addRow(translate("CAM_Walkthrough", "Start Depth:"), self._start_depth)

        self._final_depth = QtWidgets.QDoubleSpinBox()
        self._final_depth.setSuffix(" mm")
        self._final_depth.setRange(-10000, 10000)
        self._final_depth.setValue(round(z_min, 2))
        layout.addRow(translate("CAM_Walkthrough", "Final Depth:"), self._final_depth)

        layout.addStretch()

    def getClearanceHeight(self):
        return self._clearance_height.value()

    def getSafeHeight(self):
        return self._safe_height.value()

    def getStartDepth(self):
        return self._start_depth.value()

    def getFinalDepth(self):
        return self._final_depth.value()

    def isComplete(self):
        return True


# ===================================================================
#  Step 7 - Review & Generate
# ===================================================================


class ReviewPage(QtWidgets.QWizardPage):
    """Page 7: Summary of all choices and generate GCode."""

    def __init__(self, parent=None):
        super().__init__(parent)
        self.setTitle(translate("CAM_Walkthrough", "Step 7: Review & Generate"))
        self.setSubTitle(
            translate(
                "CAM_Walkthrough",
                "Review your settings and click Generate to create the CAM Job and export GCode.",
            )
        )

        layout = QtWidgets.QVBoxLayout(self)

        self._summary = QtWidgets.QTextEdit()
        self._summary.setReadOnly(True)
        layout.addWidget(self._summary)

        self._status_label = QtWidgets.QLabel()
        self._status_label.setWordWrap(True)
        layout.addWidget(self._status_label)

    def initializePage(self):
        wizard = self.wizard()
        body_page = wizard.page(0)
        job_page = wizard.page(1)
        op_page = wizard.page(2)
        tool_page = wizard.page(3)
        cut_page = wizard.page(4)
        height_page = wizard.page(5)

        bodies = body_page.getSelectedBodies()
        body_names = ", ".join(b.Label for b in bodies) if bodies else "None"
        stock_bounds = job_page.getStockBounds()
        stock_str = "N/A"
        if stock_bounds:
            stock_str = "%.1f x %.1f x %.1f mm" % (
                stock_bounds.XLength + 2 * job_page.getPaddingXY(),
                stock_bounds.YLength + 2 * job_page.getPaddingXY(),
                stock_bounds.ZLength
                + job_page.getPaddingZTop()
                + job_page.getPaddingZBottom(),
            )

        op_info = op_page.getOperationType()
        tool = tool_page.getTool()
        tool_name = tool.Label if tool else "None"

        html = f"""
        <table style="font-size: 12pt; line-height: 1.6;">
        <tr><td><b>Body:</b></td><td>{body_names}</td></tr>
        <tr><td><b>Stock:</b></td><td>{stock_str}</td></tr>
        <tr><td><b>Operation:</b></td><td>{op_info["label"]}</td></tr>
        <tr><td><b>Tool:</b></td><td>{tool_name}</td></tr>
        <tr><td><b>Spindle:</b></td><td>{tool_page.getSpindleSpeed()} RPM</td></tr>
        <tr><td><b>Feed Rate:</b></td><td>{tool_page.getFeedRate()} mm/min</td></tr>
        <tr><td><b>Plunge Rate:</b></td><td>{tool_page.getPlungeRate()} mm/min</td></tr>
        <tr><td><b>Coolant:</b></td><td>{tool_page.getCoolant()}</td></tr>
        <tr><td><b>Step Down:</b></td><td>{cut_page.getStepDown()} mm</td></tr>
        <tr><td><b>Step Over:</b></td><td>{cut_page.getStepOver()}%</td></tr>
        <tr><td><b>Clearance:</b></td><td>{height_page.getClearanceHeight()} mm</td></tr>
        <tr><td><b>Safe Height:</b></td><td>{height_page.getSafeHeight()} mm</td></tr>
        <tr><td><b>Start Depth:</b></td><td>{height_page.getStartDepth()} mm</td></tr>
        <tr><td><b>Final Depth:</b></td><td>{height_page.getFinalDepth()} mm</td></tr>
        <tr><td><b>Post Processor:</b></td><td>{job_page.getPostProcessor()}</td></tr>
        </table>
        """
        self._summary.setHtml(html)
        self._status_label.setText("")

    def setGenerationStatus(self, success, message=""):
        if success:
            self._status_label.setText(f"<b style='color: green;'>{message}</b>")
        else:
            self._status_label.setText(f"<b style='color: red;'>{message}</b>")

    def isComplete(self):
        return True
