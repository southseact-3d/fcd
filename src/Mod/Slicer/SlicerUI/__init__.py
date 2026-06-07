# SPDX-License-Identifier: LGPL-2.1-or-later

"""SlicerWorkbench UI widgets."""

from .material_selector_widget import MaterialSelectorWidget
from .printer_selector_widget import PrinterSelectorWidget
from .slicer_output_widget import SlicerOutputWidget
from .slicer_preferences import SlicerPreferencesDialog
from .support_paint_widget import SupportPaintWidget

__all__ = [
    "MaterialSelectorWidget",
    "PrinterSelectorWidget",
    "SlicerOutputWidget",
    "SlicerPreferencesDialog",
    "SupportPaintWidget",
]
