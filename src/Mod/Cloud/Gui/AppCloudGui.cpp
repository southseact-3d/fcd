#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>

#include "Workbench.h"


// use a different name to CreateCommand()
void CreateCloudCommands(void);


namespace CloudGui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("CloudGui")
    {
        initialize("This module is the CloudGui module.");  // register with Python
    }

    virtual ~Module()
    {}

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace CloudGui


/* Python entry */
PyMOD_INIT_FUNC(CloudGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }

    // instantiating the commands
    CreateCloudCommands();
    CloudGui::Workbench::init();

    // ADD YOUR CODE HERE
    //
    //
    PyObject* mod = CloudGui::initModule();
    Base::Console().log("Loading GUI of Cloud module... done\n");
    PyMOD_Return(mod);
}
