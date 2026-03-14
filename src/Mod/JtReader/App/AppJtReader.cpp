#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>

namespace JtReaderNS
{
extern PyObject* initModule();
}

PyMOD_INIT_FUNC(JtReader)
{
    PyObject* jtReaderModule = JtReaderNS::initModule();

    // load dependent module
    Base::Interpreter().loadModule("Mesh");

    Base::Console().log("Loading JtReader module... done\n");

    PyMOD_Return(jtReaderModule);
}
