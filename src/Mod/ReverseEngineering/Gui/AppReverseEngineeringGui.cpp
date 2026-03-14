#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>
#include <Gui/Language/Translator.h>

#include "Workbench.h"


// use a different name to CreateCommand()
void CreateReverseEngineeringCommands();

void loadReverseEngineeringResource()
{
    // add resources and reloads the translators
    Q_INIT_RESOURCE(ReverseEngineering);
    Q_INIT_RESOURCE(ReverseEngineering_translation);
    Gui::Translator::instance()->refresh();
}

namespace ReverseEngineeringGui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("ReverseEngineeringGui")
    {
        initialize("This module is the ReverseEngineeringGui module.");  // register with Python
    }

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace ReverseEngineeringGui


/* Python entry */
PyMOD_INIT_FUNC(ReverseEngineeringGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }

    // load dependent module
    try {
        Base::Interpreter().loadModule("MeshGui");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* mod = ReverseEngineeringGui::initModule();
    Base::Console().log("Loading GUI of ReverseEngineering module… done\n");

    // instantiating the commands
    CreateReverseEngineeringCommands();
    ReverseEngineeringGui::Workbench::init();

    // add resources and reloads the translators
    loadReverseEngineeringResource();
    PyMOD_Return(mod);
}
