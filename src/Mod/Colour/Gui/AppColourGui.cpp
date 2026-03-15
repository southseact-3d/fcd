// SPDX-License-Identifier: LGPL-2.1-or-later

#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>

#include "Command.h"
#include "ViewProviderColourMap.h"
#include "Workbench.h"

namespace ColourGui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("ColourGui")
    {
        initialize("This module is the ColourGui module.");
    }
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

} // namespace ColourGui

PyMOD_INIT_FUNC(ColourGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }

    // Register view providers
    ColourGui::ViewProviderColourMap::init();

    CreateColourCommands();
    ColourGui::Workbench::init();

    PyObject* mod = ColourGui::initModule();
    Base::Console().log("Loading GUI of Colour module... done\n");
    PyMOD_Return(mod);
}
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace ColourGui

PyMOD_INIT_FUNC(ColourGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }

    CreateColourCommands();
    ColourGui::Workbench::init();

    PyObject* mod = ColourGui::initModule();
    Base::Console().log("Loading GUI of Colour module... done\n");
    PyMOD_Return(mod);
}
