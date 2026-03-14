#include <Python.h>


#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>

#include "Workbench.h"

#include <CXX/Extensions.hxx>
#include <CXX/Objects.hxx>

// use a different name to CreateCommand()
void Create_TEMPLATE_Commands(void);


namespace _TEMPLATE_Gui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("_TEMPLATE_Gui")
    {
        initialize("This module is the _TEMPLATE_Gui module.");  // register with Python
    }

    virtual ~Module()
    {}

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace _TEMPLATE_Gui


/* Python entry */
PyMOD_INIT_FUNC(_TEMPLATE_Gui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(0);
    }

    // instantiating the commands
    Create_TEMPLATE_Commands();
    _TEMPLATE_Gui::Workbench::init();

    // ADD YOUR CODE HERE
    //
    //
    PyObject* mod = _TEMPLATE_Gui::initModule();
    Base::Console().log("Loading GUI of _TEMPLATE_ module... done\n");
    PyMOD_Return(mod);
}
