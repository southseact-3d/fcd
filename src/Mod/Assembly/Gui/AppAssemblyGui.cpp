#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>

#include "Commands.h"
#include "ViewProviderAssembly.h"
#include "ViewProviderAssemblyLink.h"
#include "ViewProviderBom.h"
#include "ViewProviderBomGroup.h"
#include "ViewProviderJointGroup.h"
#include "ViewProviderViewGroup.h"
#include "ViewProviderSimulationGroup.h"

namespace AssemblyGui
{
extern PyObject* initModule();
}

/* Python entry */
PyMOD_INIT_FUNC(AssemblyGui)
{
    // load dependent module
    try {
        Base::Interpreter().runString("import SpreadsheetGui");
    }
    catch (const Base::Exception& e) {
        PyErr_SetString(PyExc_ImportError, e.what());
        PyMOD_Return(nullptr);
    }

    PyObject* mod = AssemblyGui::initModule();
    Base::Console().log("Loading AssemblyGui module... done\n");

    AssemblyGui::CreateAssemblyCommands();

    // NOTE: To finish the initialization of our own type objects we must
    // call PyType_Ready, otherwise we run into a segmentation fault, later on.
    // This function is responsible for adding inherited slots from a type's base class.

    AssemblyGui::ViewProviderAssembly::init();
    AssemblyGui::ViewProviderAssemblyLink::init();
    AssemblyGui::ViewProviderBom::init();
    AssemblyGui::ViewProviderBomGroup::init();
    AssemblyGui::ViewProviderJointGroup::init();
    AssemblyGui::ViewProviderViewGroup::init();
    AssemblyGui::ViewProviderSimulationGroup::init();

    PyMOD_Return(mod);
}
