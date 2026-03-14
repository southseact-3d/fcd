#include <Python.h>

// clang-format off
#include "FemPostBranchFilter.h"
#include "FemPostBranchFilterPy.h"
#include "FemPostBranchFilterPy.cpp"
// clang-format on


using namespace Fem;

// returns a string which represents the object e.g. when printed in python
std::string FemPostBranchFilterPy::representation() const
{
    return {"<FemPostBranchFilter object>"};
}

PyObject* FemPostBranchFilterPy::getFilter(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    auto filters = getFemPostBranchFilterPtr()->getFilter();
    Py::List sequence;
    for (auto filter : filters) {
        sequence.append(Py::asObject(filter->getPyObject()));
    }

    return Py::new_reference_to(sequence);
}

PyObject* FemPostBranchFilterPy::recomputeChildren(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    getFemPostBranchFilterPtr()->recomputeChildren();
    Py_Return;
}

PyObject* FemPostBranchFilterPy::getLastPostObject(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    App::DocumentObject* obj = getFemPostBranchFilterPtr()->getLastPostObject();
    if (obj) {
        return obj->getPyObject();
    }
    Py_Return;
}

PyObject* FemPostBranchFilterPy::holdsPostObject(PyObject* args)
{
    PyObject* py;
    if (!PyArg_ParseTuple(args, "O!", &(App::DocumentObjectPy::Type), &py)) {
        return nullptr;
    }

    App::DocumentObject* obj = static_cast<App::DocumentObjectPy*>(py)->getDocumentObjectPtr();
    if (!obj->isDerivedFrom<FemPostObject>()) {
        PyErr_SetString(PyExc_TypeError, "object is not a post-processing object");
        return nullptr;
    }

    bool ok = getFemPostBranchFilterPtr()->holdsPostObject(static_cast<FemPostObject*>(obj));
    return Py_BuildValue("O", (ok ? Py_True : Py_False));
}

PyObject* FemPostBranchFilterPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int FemPostBranchFilterPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
