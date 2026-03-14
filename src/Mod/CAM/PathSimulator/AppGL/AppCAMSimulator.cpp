#include <Base/Console.h>
#include <Base/Interpreter.h>

#include "CAMSim.h"
#include "CAMSimPy.h"


namespace CAMSimulator
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("CAMSimulator")
    {
        initialize("This module is the CAMSimulator module.");  // register with Python
    }

    ~Module() override
    {}

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}


}  // namespace CAMSimulator


/* Python entry */
PyMOD_INIT_FUNC(CAMSimulator)
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
    PyObject* mod = CAMSimulator::initModule();
    Base::Console().log("Loading CAMSimulator module.... done\n");

    // Add Types to module
    Base::Interpreter().addType(&CAMSimulator::CAMSimPy::Type, mod, "PathSim");

    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.
    CAMSimulator::CAMSim::init();

    PyMOD_Return(mod);
}
