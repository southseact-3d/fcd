#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>


namespace DraftUtils
{
extern PyObject* initModule();
}

/* Python entry */
PyMOD_INIT_FUNC(DraftUtils)
{
    // load dependent module
    try {
        Base::Interpreter().loadModule("Part");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }
    PyObject* mod = DraftUtils::initModule();
    Base::Console().log("Loading DraftUtils module… done\n");
    PyMOD_Return(mod);
}
