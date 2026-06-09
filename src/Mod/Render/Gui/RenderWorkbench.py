import FreeCAD as App
import FreeCADGui as Gui
import os


class RenderWorkbench(Workbench):
    "Render workbench for photorealistic rendering"

    def __init__(self):
        self.__class__.Icon = (
            App.getResourceDir() + "Mod/Render/Resources/Icons/RenderWorkbench.svg"
        )
        self.__class__.MenuText = "Render"
        self.__class__.ToolTip = "Render workbench for photorealistic rendering"

    def Initialize(self):
        import Render.Gui.RenderCommands

        self.appendToolbar(
            "Render",
            [
                "Render_Render",
                "Render_Stop",
                "Separator",
                "Render_Settings",
                "Render_Scene",
                "Render_Camera",
                "Render_Material",
                "Separator",
                "Render_Queue",
                "Render_Preview",
            ],
        )
        self.appendMenu(
            "Render",
            [
                "Render_Render",
                "Render_Stop",
                "Separator",
                "Render_Settings",
                "Render_Scene",
                "Render_Camera",
                "Render_Material",
                "Separator",
                "Render_Queue",
                "Render_Preview",
            ],
        )

    def Activated(self):
        pass

    def Deactivated(self):
        pass

    def ContextMenu(self, recipient):
        self.appendContextMenu("Render Context", ["Render_Material", "Render_Settings"])

    def GetClassName(self):
        return "Gui::PythonWorkbench"


Gui.addWorkbench(RenderWorkbench())
