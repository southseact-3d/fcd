// generated out of Precision.pyi
#include "PrecisionPy.h"
#include "PrecisionPy.cpp"

using Base::Precision;
using Base::PrecisionPy;

// returns a string which represents the object e.g. when printed in python
std::string PrecisionPy::representation() const
{
    return {"<Precision object>"};
}

PyObject* PrecisionPy::angular(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    Py::Float v(Precision::Angular());
    return Py::new_reference_to(v);
}

PyObject* PrecisionPy::confusion(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    Py::Float v(Precision::Confusion());
    return Py::new_reference_to(v);
}

PyObject* PrecisionPy::squareConfusion(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    Py::Float v(Precision::SquareConfusion());
    return Py::new_reference_to(v);
}

PyObject* PrecisionPy::intersection(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    Py::Float v(Precision::Intersection());
    return Py::new_reference_to(v);
}

PyObject* PrecisionPy::approximation(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    Py::Float v(Precision::Approximation());
    return Py::new_reference_to(v);
}

PyObject* PrecisionPy::parametric(PyObject* args)
{
    double p {};
    if (PyArg_ParseTuple(args, "d", &p)) {
        Py::Float v(Precision::Parametric(p));
        return Py::new_reference_to(v);
    }

    PyErr_Clear();
    double t {};
    if (PyArg_ParseTuple(args, "dd", &p, &t)) {
        Py::Float v(Precision::Parametric(p, t));
        return Py::new_reference_to(v);
    }

    PyErr_SetString(PyExc_ValueError, "one or two floats expected");
    return nullptr;
}

PyObject* PrecisionPy::isInfinite(PyObject* args)
{
    double v {};
    if (!PyArg_ParseTuple(args, "d", &v)) {
        return nullptr;
    }

    Py::Boolean b(Precision::IsInfinite(v));
    return Py::new_reference_to(b);
}

PyObject* PrecisionPy::isPositiveInfinite(PyObject* args)
{
    double v {};
    if (!PyArg_ParseTuple(args, "d", &v)) {
        return nullptr;
    }

    Py::Boolean b(Precision::IsPositiveInfinite(v));
    return Py::new_reference_to(b);
}

PyObject* PrecisionPy::isNegativeInfinite(PyObject* args)
{
    double v {};
    if (!PyArg_ParseTuple(args, "d", &v)) {
        return nullptr;
    }

    Py::Boolean b(Precision::IsNegativeInfinite(v));
    return Py::new_reference_to(b);
}

PyObject* PrecisionPy::infinite(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    Py::Float v(Precision::Infinite());
    return Py::new_reference_to(v);
}

PyObject* PrecisionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int PrecisionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
