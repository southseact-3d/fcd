#include <CXX/Extensions.hxx>

#include <Base/PythonTypeExt.h>


using namespace Base;

PythonTypeExt::PythonTypeExt(Py::PythonType& type)
    : pytype(type)
{}

Py::PythonType& PythonTypeExt::set_tp_descr_get(
    PyObject* (*tp_descr_get)(PyObject* self, PyObject* obj, PyObject* type)
)
{
    pytype.type_object()->tp_descr_get = tp_descr_get;

    return pytype;
}

Py::PythonType& PythonTypeExt::set_tp_descr_set(
    int (*tp_descr_set)(PyObject* self, PyObject* obj, PyObject* value)
)
{
    pytype.type_object()->tp_descr_set = tp_descr_set;

    return pytype;
}
