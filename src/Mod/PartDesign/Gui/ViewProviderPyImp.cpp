#include "ViewProvider.h"

// inclusion of the generated files (generated out of ViewProviderPy.xml)
#include "ViewProviderPy.h"
#include "ViewProviderPy.cpp"

using namespace PartDesignGui;

// returns a string which represent the object e.g. when printed in python
std::string ViewProviderPy::representation() const
{
    return {"<PartDesign::ViewProvider>"};
}

PyObject* ViewProviderPy::getCustomAttributes(const char*) const
{
    return nullptr;
}

int ViewProviderPy::setCustomAttributes(const char*, PyObject*)
{
    return 0;
}

PyObject* ViewProviderPy::setBodyMode(PyObject* args)
{
    PyObject* b_mode;
    if (!PyArg_ParseTuple(args, "O!", &PyBool_Type, &b_mode)) {
        return nullptr;
    }

    PartDesignGui::ViewProvider* base = getViewProviderPtr();
    base->setBodyMode(Base::asBoolean(b_mode));

    return Py::new_reference_to(Py::None());
}

PyObject* ViewProviderPy::makeTemporaryVisible(PyObject* args)
{
    PyObject* b_vis;
    if (!PyArg_ParseTuple(args, "O!", &PyBool_Type, &b_vis)) {
        return nullptr;
    }

    PartDesignGui::ViewProvider* base = getViewProviderPtr();
    base->makeTemporaryVisible(Base::asBoolean(b_vis));

    return Py::new_reference_to(Py::None());
}
