#include <Base/Console.h>
#include <Base/Interpreter.h>

#include "StepShape.h"
#include "StepShapePy.h"


namespace Import
{
extern PyObject* initModule();
}

PyMOD_INIT_FUNC(Import)
{
    PyObject* importModule = Import::initModule();

    try {
        Base::Interpreter().loadModule("Part");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    // add mesh elements
    Base::Interpreter().addType(&Import::StepShapePy ::Type, importModule, "StepShape");


    Base::Console().log("Loading Import module... done\n");
    PyMOD_Return(importModule);
}
