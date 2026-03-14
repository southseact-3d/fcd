#ifndef BASE_PYTHONTYPEEXT_H
#define BASE_PYTHONTYPEEXT_H

#include <FCGlobal.h>

namespace Py
{
class PythonType;
}

using PyObject = struct _object;

namespace Base
{

/// class to extend PyCXX PythonType class
class BaseExport PythonTypeExt
{
public:
    explicit PythonTypeExt(Py::PythonType& type);

    Py::PythonType& set_tp_descr_get(
        PyObject* (*tp_descr_get)(PyObject* self, PyObject* obj, PyObject* type)
    );
    Py::PythonType& set_tp_descr_set(
        int (*tp_descr_set)(PyObject* self, PyObject* obj, PyObject* value)
    );

private:
    Py::PythonType& pytype;
};

}  // namespace Base

#endif  // BASE_PYTHONTYPEEXT_H
