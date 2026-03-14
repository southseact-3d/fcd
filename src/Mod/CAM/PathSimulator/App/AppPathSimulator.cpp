#include <Base/Console.h>
#include <Base/Interpreter.h>

#include "PathSim.h"
#include "PathSimPy.h"


namespace PathSimulator
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("PathSimulator")
    {
        initialize("This module is the PathSimulator module.");  // register with Python
    }

    ~Module() override
    {}

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}


}  // namespace PathSimulator


/* Python entry */
PyMOD_INIT_FUNC(PathSimulator)
{
    // load dependent module
    try {
        Base::Interpreter().runString("import Part");
        Base::Interpreter().runString("import Path");
        Base::Interpreter().runString("import Mesh");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    //
    PyObject* mod = PathSimulator::initModule();
    Base::Console().log("Loading PathSimulator module.... done\n");

    // Add Types to module
    Base::Interpreter().addType(&PathSimulator::PathSimPy::Type, mod, "PathSim");

    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.
    PathSimulator::PathSim::init();

    PyMOD_Return(mod);
}
