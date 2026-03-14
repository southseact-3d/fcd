#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>
#include <Gui/Language/Translator.h>

#include "ViewProviderEdge2TracObject.h"
#include "ViewProviderRobotObject.h"
#include "ViewProviderTrajectory.h"
#include "ViewProviderTrajectoryCompound.h"
#include "ViewProviderTrajectoryDressUp.h"
#include "Workbench.h"


// use a different name to CreateCommand()
void CreateRobotCommands();
void CreateRobotCommandsExport();
void CreateRobotCommandsInsertRobots();
void CreateRobotCommandsTrajectory();

void loadRobotResource()
{
    // add resources and reloads the translators
    Q_INIT_RESOURCE(Robot);
    Q_INIT_RESOURCE(Robot_translation);
    Gui::Translator::instance()->refresh();
}

namespace RobotGui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("RobotGui")
    {
        initialize("This module is the RobotGui module.");  // register with Python
    }

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace RobotGui


/* Python entry */
PyMOD_INIT_FUNC(RobotGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }
    try {
        Base::Interpreter().runString("import PartGui");
        Base::Interpreter().runString("import Part");
        Base::Interpreter().runString("import Robot");
        // set some default values
        // default speed for trajectory is 1m/s
        Base::Interpreter().runString("_DefSpeed = '1 m/s'");
        // default Cintinuity is off
        Base::Interpreter().runString("_DefCont = False");
        // default Cintinuity is off
        Base::Interpreter().runString("_DefAcceleration = '1 m/s^2'");
        // default orientation of a waypoint if no other constraint
        Base::Interpreter().runString("_DefOrientation = FreeCAD.Rotation()");
        // default displacement while e.g. picking
        Base::Interpreter().runString("_DefDisplacement = FreeCAD.Vector(0,0,0)");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }
    PyObject* mod = RobotGui::initModule();
    Base::Console().log("Loading GUI of Robot module… done\n");

    // instantiating the commands
    CreateRobotCommands();
    CreateRobotCommandsExport();
    CreateRobotCommandsInsertRobots();
    CreateRobotCommandsTrajectory();

    // clang-format off
    // addition objects
    RobotGui::Workbench                      ::init();
    RobotGui::ViewProviderRobotObject        ::init();
    RobotGui::ViewProviderTrajectory         ::init();
    RobotGui::ViewProviderEdge2TracObject    ::init();
    RobotGui::ViewProviderTrajectoryCompound ::init();
    RobotGui::ViewProviderTrajectoryDressUp  ::init();
    // clang-format on

    // add resources and reloads the translators
    loadRobotResource();

    PyMOD_Return(mod);
}
