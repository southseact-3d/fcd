#include <Base/Console.h>
#include <Base/Interpreter.h>
#include <Base/PyObjectBase.h>
#include <Gui/Application.h>

#include "Blending/ViewProviderBlendCurve.h"

#include "TaskFilling.h"
#include "TaskGeomFillSurface.h"
#include "TaskSections.h"
#include "ViewProviderExtend.h"
#include "Workbench.h"


// use a different name to CreateCommand()
void CreateSurfaceCommands();


namespace SurfaceGui
{
class Module: public Py::ExtensionModule<Module>
{
public:
    Module()
        : Py::ExtensionModule<Module>("SurfaceGui")
    {
        initialize("This module is the SurfaceGui module.");  // register with Python
    }

private:
};

PyObject* initModule()
{
    return Base::Interpreter().addModule(new Module);
}

}  // namespace SurfaceGui

/* Python entry */
PyMOD_INIT_FUNC(SurfaceGui)
{
    if (!Gui::Application::Instance) {
        PyErr_SetString(PyExc_ImportError, "Cannot load Gui module in console application.");
        PyMOD_Return(nullptr);
    }

    Base::Interpreter().runString("import Surface");
    Base::Interpreter().runString("import PartGui");

    // clang-format off
    // instantiating the commands
    CreateSurfaceCommands();

    SurfaceGui::Workbench::init();
    SurfaceGui::ViewProviderGeomFillSurface ::init();
    SurfaceGui::ViewProviderFilling         ::init();
    SurfaceGui::ViewProviderSections        ::init();
    SurfaceGui::ViewProviderExtend          ::init();
    SurfaceGui::ViewProviderBlendCurve      ::init();
    // SurfaceGui::ViewProviderCut::init();
    // clang-format on

    PyObject* mod = SurfaceGui::initModule();
    Base::Console().log("Loading GUI of Surface module… done\n");
    PyMOD_Return(mod);
}
