#include "GeometryPyCXX.h"

// generated out of Axis.pyi
#include "AxisPy.h"
#include "AxisPy.cpp"

#include "PlacementPy.h"
#include "VectorPy.h"

using namespace Base;

// returns a string which represents the object e.g. when printed in python
std::string AxisPy::representation() const
{
    AxisPy::PointerType ptr = getAxisPtr();
    std::stringstream str;
    str << "Axis [Base=(";
    str << ptr->getBase().x << "," << ptr->getBase().y << "," << ptr->getBase().z;
    str << "), Direction=(";
    str << ptr->getDirection().x << "," << ptr->getDirection().y << "," << ptr->getDirection().z
        << ")]";

    return str.str();
}

PyObject* AxisPy::PyMake(PyTypeObject* /*unused*/, PyObject* /*unused*/, PyObject* /*unused*/)
{
    // create a new instance of AxisPy and the Twin object
    return new AxisPy(new Axis);
}

// constructor method
int AxisPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject* o {};
    if (PyArg_ParseTuple(args, "")) {
        return 0;
    }

    PyErr_Clear();
    if (PyArg_ParseTuple(args, "O!", &(AxisPy::Type), &o)) {
        auto a = static_cast<AxisPy*>(o)->getAxisPtr();
        *(getAxisPtr()) = *a;
        return 0;
    }

    PyErr_Clear();
    PyObject* d {};
    if (PyArg_ParseTuple(args, "O!O!", &(VectorPy::Type), &o, &(VectorPy::Type), &d)) {
        // NOTE: The first parameter defines the base (origin) and the second the direction.
        *getAxisPtr() = Axis(static_cast<VectorPy*>(o)->value(), static_cast<VectorPy*>(d)->value());
        return 0;
    }

    PyErr_SetString(PyExc_TypeError, "empty parameter list, axis or base and direction expected");
    return -1;
}

PyObject* AxisPy::move(PyObject* args)
{
    PyObject* vec {};
    if (!PyArg_ParseTuple(args, "O!", &(VectorPy::Type), &vec)) {
        return nullptr;
    }
    getAxisPtr()->move(static_cast<VectorPy*>(vec)->value());
    Py_Return;
}

PyObject* AxisPy::multiply(PyObject* args)
{
    PyObject* plm {};
    if (!PyArg_ParseTuple(args, "O!", &(PlacementPy::Type), &plm)) {
        return nullptr;
    }
    Axis mult = (*getAxisPtr()) * (*static_cast<PlacementPy*>(plm)->getPlacementPtr());
    return new AxisPy(new Axis(mult));
}

PyObject* AxisPy::copy(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    return new AxisPy(new Axis(*getAxisPtr()));
}

PyObject* AxisPy::reversed(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }
    return new AxisPy(new Axis(getAxisPtr()->reversed()));
}

Py::Object AxisPy::getBase() const
{
    return Py::Vector(getAxisPtr()->getBase());  // NOLINT
}

void AxisPy::setBase(Py::Object arg)
{
    getAxisPtr()->setBase(Py::Vector(arg).toVector());
}

Py::Object AxisPy::getDirection() const
{
    return Py::Vector(getAxisPtr()->getDirection());  // NOLINT
}

void AxisPy::setDirection(Py::Object arg)
{
    getAxisPtr()->setDirection(Py::Vector(arg).toVector());
}

PyObject* AxisPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int AxisPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
