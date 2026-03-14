#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>

#include "AssemblyObject.h"
#include "AssemblyLink.h"
#include "BomObject.h"
#include "BomGroup.h"
#include "JointGroup.h"
#include "ViewGroup.h"
#include "SimulationGroup.h"


namespace Assembly
{
extern PyObject* initModule();
}

/* Python entry */
PyMOD_INIT_FUNC(AssemblyApp)
{
    // load dependent module
    try {
        Base::Interpreter().runString("import Part");
        Base::Interpreter().runString("import Spreadsheet");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* mod = Assembly::initModule();
    Base::Console().log("Loading Assembly module... done\n");


    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.

    Assembly::AssemblyObject ::init();
    Assembly::AssemblyLink ::init();
    Assembly::BomObject ::init();

    Assembly::BomGroup ::init();
    Assembly::JointGroup ::init();
    Assembly::ViewGroup ::init();
    Assembly::SimulationGroup ::init();

    PyMOD_Return(mod);
}
