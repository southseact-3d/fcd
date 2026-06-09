import FreeCAD as App
import FreeCADGui as Gui
import os


class _CommandRender:
    def GetResources(self):
        return {
            "Pixmap": os.path.join(
                App.getResourceDir(), "Mod/Render/Resources/Icons", "render.svg"
            ),
            "MenuText": "Render",
            "ToolTip": "Start rendering the current scene",
        }

    def IsActive(self):
        return App.ActiveDocument is not None

    def Activated(self):
        try:
            import Render.Gui.RenderGui as RenderGui

            RenderGui.startRender()
        except ImportError:
            App.Console.PrintError("Render module not available\n")


class _CommandStop:
    def GetResources(self):
        return {
            "Pixmap": os.path.join(
                App.getResourceDir(), "Mod/Render/Resources/Icons", "stop.svg"
            ),
            "MenuText": "Stop Render",
            "ToolTip": "Stop the current render",
        }

    def IsActive(self):
        return True

    def Activated(self):
        try:
            import Render.Gui.RenderGui as RenderGui

            RenderGui.stopRender()
        except ImportError:
            pass


class _CommandSettings:
    def GetResources(self):
        return {
            "Pixmap": os.path.join(
                App.getResourceDir(), "Mod/Render/Resources/Icons", "settings.svg"
            ),
            "MenuText": "Render Settings",
            "ToolTip": "Configure render output settings",
        }

    def IsActive(self):
        return True

    def Activated(self):
        from PySide6 import QtWidgets
        from Render.Gui.TaskRenderSettings import TaskRenderSettings

        panel = TaskRenderSettings()
        Gui.Control.showDialog(panel)


class _CommandScene:
    def GetResources(self):
        return {
            "Pixmap": os.path.join(
                App.getResourceDir(), "Mod/Render/Resources/Icons", "scene.svg"
            ),
            "MenuText": "Scene Settings",
            "ToolTip": "Configure scene and environment settings",
        }

    def IsActive(self):
        return True

    def Activated(self):
        from Render.Gui.TaskSceneSettings import TaskSceneSettings

        panel = TaskSceneSettings()
        Gui.Control.showDialog(panel)


class _CommandCamera:
    def GetResources(self):
        return {
            "Pixmap": os.path.join(
                App.getResourceDir(), "Mod/Render/Resources/Icons", "camera.svg"
            ),
            "MenuText": "Camera Settings",
            "ToolTip": "Configure render camera settings",
        }

    def IsActive(self):
        return True

    def Activated(self):
        from Render.Gui.TaskCameraSettings import TaskCameraSettings

        panel = TaskCameraSettings()
        Gui.Control.showDialog(panel)


class _CommandMaterial:
    def GetResources(self):
        return {
            "Pixmap": os.path.join(
                App.getResourceDir(), "Mod/Render/Resources/Icons", "material.svg"
            ),
            "MenuText": "Material Editor",
            "ToolTip": "Edit material for selected object",
        }

    def IsActive(self):
        return App.ActiveDocument is not None and len(Gui.Selection.getSelection()) > 0

    def Activated(self):
        from Render.Gui.TaskMaterialEditor import TaskMaterialEditor

        panel = TaskMaterialEditor()
        Gui.Control.showDialog(panel)


class _CommandQueue:
    def GetResources(self):
        return {
            "Pixmap": os.path.join(
                App.getResourceDir(), "Mod/Render/Resources/Icons", "queue.svg"
            ),
            "MenuText": "Render Queue",
            "ToolTip": "Show/hide the render queue panel",
        }

    def IsActive(self):
        return True

    def Activated(self):
        try:
            from Render.Gui.PanelRenderQueue import toggleQueuePanel

            toggleQueuePanel()
        except ImportError:
            pass


class _CommandPreview:
    def GetResources(self):
        return {
            "Pixmap": os.path.join(
                App.getResourceDir(), "Mod/Render/Resources/Icons", "preview.svg"
            ),
            "MenuText": "Preview Render",
            "ToolTip": "Start a quick preview render",
        }

    def IsActive(self):
        return App.ActiveDocument is not None

    def Activated(self):
        try:
            import Render.Gui.RenderGui as RenderGui

            RenderGui.startPreview()
        except ImportError:
            App.Console.PrintError("Render module not available\n")


Gui.addCommand("Render_Render", _CommandRender())
Gui.addCommand("Render_Stop", _CommandStop())
Gui.addCommand("Render_Settings", _CommandSettings())
Gui.addCommand("Render_Scene", _CommandScene())
Gui.addCommand("Render_Camera", _CommandCamera())
Gui.addCommand("Render_Material", _CommandMaterial())
Gui.addCommand("Render_Queue", _CommandQueue())
Gui.addCommand("Render_Preview", _CommandPreview())
