#include <Base/Interpreter.h>
#include <Base/Tools.h>
#include <Base/Console.h>

#include <Base/PyObjectBase.h>

#include <gsl/pointers>

namespace Start
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("Start")
    {
        initialize("This module is the Start module.");  // register with Python
    }
};

PyObject* initModule()
{
    auto newModule = gsl::owner<Module*>(new Module);
    return Base::Interpreter().addModule(newModule);  // Transfer ownership
}

}  // namespace Start

/* Python entry */
PyMOD_INIT_FUNC(Start)
{
    PyObject* mod = Start::initModule();
    Base::Console().log("Loading Start module... done\n");
    PyMOD_Return(mod);
}
