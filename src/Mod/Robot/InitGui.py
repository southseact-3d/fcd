class RobotWorkbench(Workbench):
    "Robot workbench object"

    def __init__(self):
        self.__class__.Icon = (
            FreeCAD.getResourceDir() + "Mod/Robot/Resources/icons/RobotWorkbench.svg"
        )
        self.__class__.MenuText = "Robot"
        self.__class__.ToolTip = "Robot workbench"

    def Initialize(self):
        # load the module
        import RobotGui
        import Robot

    def GetClassName(self):
        return "RobotGui::Workbench"


Gui.addWorkbench(RobotWorkbench())
