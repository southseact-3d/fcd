#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>

#include "ViewProviderInspection.h"
#include "Workbench.h"


// use a different name to CreateCommand()
void CreateInspectionCommands();


namespace InspectionGui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("InspectionGui")
    {
        initialize("This module is the InspectionGui module.");  // register with Python
    }

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace InspectionGui


/* Python entry */
PyMOD_INIT_FUNC(InspectionGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }

    // instantiating the commands
    // clang-format off
    CreateInspectionCommands();
    InspectionGui::ViewProviderInspection       ::init();
    InspectionGui::ViewProviderInspectionGroup  ::init();
    InspectionGui::Workbench                    ::init();
    // clang-format on

    // ADD YOUR CODE HERE
    //
    //

    PyObject* mod = InspectionGui::initModule();
    Base::Console().log("Loading GUI of Inspection module… done\n");
    PyMOD_Return(mod);
}
