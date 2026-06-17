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

"""CAM Walkthrough wizard: guided body-to-GCode pipeline."""

import FreeCAD
import FreeCADGui
import Path
import Path.Main.Gui.WalkthroughPages as WalkthroughPages
import Path.Main.Job as PathJob
import Path.Op.Profile as PathProfile
import Path.Op.PocketShape as PathPocketShape
import Path.Op.MillFace as PathMillFace
import Path.Op.Adaptive as PathAdaptive
import Path.Op.Drilling as PathDrilling
import Path.Op.Engrave as PathEngrave
import Path.Post.Processor as PostProcessor
import Path.Tool.Controller as PathToolController
import Path.Post.Utils as PostUtils
from Path.Post.Utils import FilenameGenerator
from PySide import QtCore, QtGui, QtWidgets
from PySide.QtCore import QT_TRANSLATE_NOOP

translate = FreeCAD.Qt.translate

if False:
    Path.Log.setLevel(Path.Log.Level.DEBUG, Path.Log.thisModule())
    Path.Log.trackModule(Path.Log.thisModule())
else:
    Path.Log.setLevel(Path.Log.Level.INFO, Path.Log.thisModule())


# ---------------------------------------------------------------------------
#  Operation factory mapping
# ---------------------------------------------------------------------------

OP_FACTORIES = {
    "Profile": PathProfile.Create,
    "Pocket": PathPocketShape.Create,
    "FaceMill": PathMillFace.Create,
    "Adaptive": PathAdaptive.Create,
    "Drilling": PathDrilling.Create,
    "Engrave": PathEngrave.Create,
}


# ---------------------------------------------------------------------------
#  Wizard dialog
# ---------------------------------------------------------------------------


class WalkthroughWizard(QtWidgets.QWizard):
    """A step-by-step wizard that walks the user through body-to-GCode."""

    def __init__(self, parent=None):
        super().__init__(parent or QtGui.QApplication.activeWindow())
        self.setWindowTitle(translate("CAM_Walkthrough", "CAM Walkthrough"))
        self.setMinimumSize(600, 500)
        self.setOption(QtWidgets.QWizard.HaveHelpButton, False)

        # Pages are added lazily in _build_pages()
        self._pages_added = False

    def showEvent(self, event):
        if not self._pages_added:
            self._build_pages()
            self._pages_added = True
        super().showEvent(event)

    def _build_pages(self):
        # Step 1
        self._select_body = WalkthroughPages.SelectBodyPage()
        self.addPage(self._select_body)

        # Step 2
        self._job_setup = WalkthroughPages.JobSetupPage(
            bodies=self._select_body.getSelectedBodies
        )
        self.addPage(self._job_setup)

        # Step 3
        self._op_type = WalkthroughPages.OperationTypePage()
        self.addPage(self._op_type)

        # Step 4
        self._tool_select = WalkthroughPages.ToolSelectionPage()
        self.addPage(self._tool_select)

        # Step 5
        self._cutting = WalkthroughPages.CuttingParamsPage(stock_bounds=None)
        self.addPage(self._cutting)

        # Step 6
        self._heights = WalkthroughPages.HeightsDepthsPage(stock_bounds=None)
        self.addPage(self._heights)

        # Step 7
        self._review = WalkthroughPages.ReviewPage()
        self.addPage(self._review)

    def _current_stock_bounds(self):
        return self._job_setup.getStockBounds()

    def nextId(self):
        return super().nextId()

    # ---- Override page navigation to update stock bounds ----

    def currentIdChanged(self, pageId):
        if pageId >= 0:
            # Update the stock bounds in pages that depend on it
            bounds = self._current_stock_bounds()
            self._cutting._stock_bounds = bounds
            self._heights._stock_bounds = bounds

    def done(self, result):
        super().done(result)


# ---------------------------------------------------------------------------
#  Job creation & post-processing logic
# ---------------------------------------------------------------------------


def _create_job(wizard):
    """Create a FreeCAD Job from the wizard's collected settings."""
    body_page = wizard.page(0)
    job_page = wizard.page(1)
    op_page = wizard.page(2)
    tool_page = wizard.page(3)
    cut_page = wizard.page(4)
    height_page = wizard.page(5)
    review_page = wizard.page(6)

    bodies = body_page.getSelectedBodies()
    if not bodies:
        review_page.setGenerationStatus(
            False, translate("CAM_Walkthrough", "No bodies selected.")
        )
        return None

    # --- Create Job ---
    FreeCAD.ActiveDocument.openTransaction("CAM Walkthrough Job")
    try:
        job_name = "Job"
        job = PathJob.Create(job_name, bodies)
        FreeCAD.ActiveDocument.recompute()

        # Set post processor
        post_name = job_page.getPostProcessor()
        job.PostProcessor = post_name

        # --- Create Stock with padding ---
        stock = job.Stock
        if stock:
            stock_type = getattr(stock, "StockType", None)
            # Set to "Create box" with extensions
            if hasattr(stock, "StockType"):
                stock.StockType = "Create box"
            stock.Shape = None  # Force recreation
            bounds = job_page.getStockBounds()
            if bounds:
                pad_xy = job_page.getPaddingXY()
                pad_zt = job_page.getPaddingZTop()
                pad_zn = job_page.getPaddingZBottom()
                if hasattr(stock, "ExtXneg"):
                    stock.ExtXneg = pad_xy
                    stock.ExtXpos = pad_xy
                    stock.ExtYneg = pad_xy
                    stock.ExtYpos = pad_xy
                    stock.ExtZneg = pad_zn
                    stock.ExtZpos = pad_zt

        # --- Create ToolController ---
        tool = tool_page.getTool()
        tc_name = "TC: %s" % (tool.Label if tool else "Default")
        tc = PathToolController.Create(
            name=tc_name,
            tool=tool,
            toolNumber=1,
            assignViewProvider=True,
            assignTool=True,
        )
        tc.SpindleSpeed = tool_page.getSpindleSpeed()
        tc.HorizFeed = tool_page.getFeedRate()
        tc.VertFeed = tool_page.getPlungeRate()

        job.Proxy.addToolController(tc)
        FreeCAD.ActiveDocument.recompute()

        # --- Create Operation ---
        op_info = op_page.getOperationType()
        op_id = op_info["id"]
        factory = OP_FACTORIES.get(op_id)
        if factory is None:
            review_page.setGenerationStatus(
                False,
                translate("CAM_Walkthrough", "Unknown operation type: %1").replace(
                    "%1", op_id
                ),
            )
            FreeCAD.ActiveDocument.abortTransaction()
            return None

        op_name = "%s_op" % op_id.lower()
        op_obj = factory(op_name)

        # Set tool controller on the operation
        if hasattr(op_obj, "ToolController"):
            op_obj.ToolController = tc

        # Set cutting parameters
        if hasattr(op_obj, "StepDown"):
            op_obj.StepDown = cut_page.getStepDown()
        if hasattr(op_obj, "StepOver"):
            op_obj.StepOver = cut_page.getStepOver()
        if hasattr(op_obj, "OffsetExtra"):
            op_obj.OffsetExtra = cut_page.getExtraOffset()

        # Set heights
        if hasattr(op_obj, "ClearanceHeight"):
            op_obj.ClearanceHeight = height_page.getClearanceHeight()
        if hasattr(op_obj, "SafeHeight"):
            op_obj.SafeHeight = height_page.getSafeHeight()
        if hasattr(op_obj, "StartDepth"):
            op_obj.StartDepth = height_page.getStartDepth()
        if hasattr(op_obj, "FinalDepth"):
            op_obj.FinalDepth = height_page.getFinalDepth()

        # Set coolant on tool controller
        coolant = tool_page.getCoolant()
        if hasattr(tc, "CoolantMode"):
            tc.CoolantMode = coolant

        # Add operation to job
        job.Proxy.addOperation(op_obj)
        FreeCAD.ActiveDocument.recompute()

        FreeCAD.ActiveDocument.commitTransaction()
        FreeCAD.ActiveDocument.recompute()

        return job

    except Exception as e:
        FreeCAD.ActiveDocument.abortTransaction()
        review_page.setGenerationStatus(False, str(e))
        Path.Log.error("CAM Walkthrough: %s" % str(e))
        return None


def _post_process(job):
    """Run the post processor on the job and return (success, gcode)."""
    if not job:
        return False, "No job created."

    post_name = job.PostProcessor
    if not post_name:
        post_name = Path.Preferences.defaultPostProcessor()
    if not post_name:
        return False, "No post processor selected."

    post_processor = PostProcessor.PostProcessorFactory.get_post_processor(
        job, post_name
    )
    if post_processor is None:
        return False, "Post processor '%s' not found." % post_name

    post_data = post_processor.export()
    if not post_data:
        return False, "Post processor returned no output."

    # Build GCode string
    gcode_parts = []
    for _subpart, gcode in post_data:
        if gcode is not None:
            gcode_parts.append(gcode)
    gcode = "\n".join(gcode_parts)

    return True, gcode


def _save_gcode(gcode, parent=None):
    """Prompt the user for a save location and write the GCode file."""
    dialog = QtGui.QFileDialog(
        parent or QtGui.QApplication.activeWindow(),
        translate("CAM_Walkthrough", "Save GCode"),
        "",
        translate("CAM_Walkthrough", "GCode Files (*.nc *.tap *.gcode)"),
    )
    dialog.setAcceptMode(QtGui.QFileDialog.AcceptMode.AcceptSave)
    if dialog.exec_():
        filepath = dialog.selectedFiles()[0]
        with open(filepath, "w", encoding="utf-8") as f:
            f.write(gcode)
        FreeCAD.Console.PrintMessage(
            translate("CAM_Walkthrough", "GCode saved to %1\n").replace("%1", filepath)
        )
        return True
    return False


# ---------------------------------------------------------------------------
#  Walkthrough execution (called when wizard finishes)
# ---------------------------------------------------------------------------


def execute_walkthrough():
    """Launch the wizard and, on completion, create the job and post-process."""
    wizard = WalkthroughWizard()
    result = wizard.exec_()
    if result != QtWidgets.QDialog.Accepted:
        return

    # Collect settings from wizard pages
    body_page = wizard.page(0)
    job_page = wizard.page(1)
    op_page = wizard.page(2)
    tool_page = wizard.page(3)
    cut_page = wizard.page(4)
    height_page = wizard.page(5)
    review_page = wizard.page(6)

    # Create the job
    job = _create_job(wizard)
    if job is None:
        return

    # Post-process
    success, gcode = _post_process(job)
    if success:
        review_page.setGenerationStatus(
            True,
            translate("CAM_Walkthrough", "Job created successfully!"),
        )
        # Save to file
        _save_gcode(gcode, parent=wizard)
    else:
        review_page.setGenerationStatus(False, gcode)


# ---------------------------------------------------------------------------
#  FreeCAD Command
# ---------------------------------------------------------------------------


class CommandWalkthrough:
    """FreeCAD command that launches the CAM Walkthrough wizard."""

    def GetResources(self):
        return {
            "Pixmap": "CAM_Walkthrough",
            "MenuText": QT_TRANSLATE_NOOP("CAM_Walkthrough", "Walkthrough"),
            "ToolTip": QT_TRANSLATE_NOOP(
                "CAM_Walkthrough",
                "Guided step-by-step wizard to create a CAM job and generate GCode",
            ),
        }

    def IsActive(self):
        return FreeCAD.ActiveDocument is not None

    def Activated(self):
        execute_walkthrough()


# ---------------------------------------------------------------------------
#  Command registration
# ---------------------------------------------------------------------------

if FreeCAD.GuiUp:
    FreeCADGui.addCommand("CAM_Walkthrough", CommandWalkthrough())

FreeCAD.Console.PrintLog("Loading CAM Walkthrough... done\n")
