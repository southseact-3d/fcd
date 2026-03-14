#include <Base/Console.h>
#include <Base/PyObjectBase.h>

#include "InspectionFeature.h"


namespace Inspection
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("Inspection")
    {
        initialize("This module is the Inspection module.");  // register with Python
    }

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace Inspection


/* Python entry */
PyMOD_INIT_FUNC(Inspection)
{
    // ADD YOUR CODE HERE
    //
    //
    PyObject* mod = Inspection::initModule();
    Base::Console().log("Loading Inspection module… done\n");
    // clang-format off
    Inspection::PropertyDistanceList    ::init();
    Inspection::Feature                 ::init();
    Inspection::Group                   ::init();
    // clang-format on
    PyMOD_Return(mod);
}
