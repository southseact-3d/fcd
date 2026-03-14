import FreeCAD, FreeCADGui


class CmdHelloWorld:
    def Activated(self):
        FreeCAD.Console.PrintMessage("Hello, World!\n")

    def IsActive(self):
        return True

    def GetResources(self):
        return {
            "Pixmap": "freecad",
            "MenuText": "Hello World",
            "ToolTip": "Print Hello World",
        }


FreeCADGui.addCommand("_TEMPLATEPY__HelloWorld", CmdHelloWorld())
