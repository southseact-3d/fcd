#include <Base/Interpreter.h>


namespace AssemblyGui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("AssemblyGui")
    {
        initialize("This module is the Assembly module.");  // register with Python
    }
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace AssemblyGui
