#include <Gui/Control.h>
#include <Gui/PythonWrapper.h>
#include "ViewProviderFemPostFilter.h"
#include "TaskPostBoxes.h"
#ifdef FC_USE_VTK_PYTHON
#include "TaskPostExtraction.h"
#endif
// inclusion of the generated files (generated out of ViewProviderFemPostFilterPy.xml)
#include "ViewProviderFemPostFilterPy.h"
#include "ViewProviderFemPostFilterPy.cpp"
#include <Base/PyWrapParseTupleAndKeywords.h>
// clang-format on


using namespace FemGui;

// returns a string which represents the object e.g. when printed in python
std::string ViewProviderFemPostFilterPy::representation() const
{
    return {"<ViewProviderFemPostFilter object>"};
}

PyObject* ViewProviderFemPostFilterPy::createDisplayTaskWidget(PyObject* args)
{
    // we take no arguments
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    auto panel = new TaskPostDisplay(getViewProviderFemPostObjectPtr());

    Gui::PythonWrapper wrap;
    if (wrap.loadCoreModule()) {
        return Py::new_reference_to(wrap.fromQWidget(panel));
    }

    PyErr_SetString(PyExc_TypeError, "creating the panel failed");
    return nullptr;
}

PyObject* ViewProviderFemPostFilterPy::createExtractionTaskWidget(PyObject* args)
{
#ifdef FC_USE_VTK_PYTHON
    // we take no arguments
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    auto panel = new TaskPostExtraction(getViewProviderFemPostObjectPtr());

    Gui::PythonWrapper wrap;
    if (wrap.loadCoreModule()) {
        return Py::new_reference_to(wrap.fromQWidget(panel));
    }

    PyErr_SetString(PyExc_TypeError, "creating the panel failed");
    return nullptr;
#else
    (void)args;
    PyErr_SetString(PyExc_NotImplementedError, "VTK python wrapper not available");
    return nullptr;
#endif
}

PyObject* ViewProviderFemPostFilterPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ViewProviderFemPostFilterPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
