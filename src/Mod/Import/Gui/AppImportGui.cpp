#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>

#include "Workbench.h"


// use a different name to CreateCommand()
void CreateImportCommands();


namespace ImportGui
{
extern PyObject* initModule();
}

PyMOD_INIT_FUNC(ImportGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }

    try {
        Base::Interpreter().loadModule("PartGui");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* mod = ImportGui::initModule();
    Base::Console().log("Loading GUI of Import module... done\n");

    CreateImportCommands();
    ImportGui::Workbench::init();

    PyMOD_Return(mod);
}
