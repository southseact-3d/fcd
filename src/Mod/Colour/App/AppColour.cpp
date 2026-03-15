// SPDX-License-Identifier: LGPL-2.1-or-later

#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>

#include "ColourMapObject.h"

namespace Colour
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("Colour")
    {
        initialize("This module is the Colour paint module.");
    }
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

} // namespace Colour

PyMOD_INIT_FUNC(Colour)
{
    PyObject* mod = Colour::initModule();

    // Register document object types
    Colour::ColourMapObject::init();

    Base::Console().log("Loading Colour module... done\n");
    PyMOD_Return(mod);
}
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace Colour

PyMOD_INIT_FUNC(Colour)
{
    PyObject* mod = Colour::initModule();
    Base::Console().log("Loading Colour module... done\n");
    PyMOD_Return(mod);
}
