#include <Mod/Part/PartGlobal.h>

#include <Precision.hxx>


#include "ShapeFix/ShapeFix_RootPy.h"
#include "ShapeFix/ShapeFix_RootPy.cpp"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string ShapeFix_RootPy::representation() const
{
    return "<ShapeFix_Root object>";
}

PyObject* ShapeFix_RootPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of ShapeFix_RootPy
    return new ShapeFix_RootPy(nullptr);
}

// constructor method
int ShapeFix_RootPy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }

    setHandle(new ShapeFix_Root);
    return 0;
}

PyObject* ShapeFix_RootPy::limitTolerance(PyObject* args) const
{
    double tol;
    if (!PyArg_ParseTuple(args, "d", &tol)) {
        return nullptr;
    }

    tol = getShapeFix_RootPtr()->LimitTolerance(tol);
    return Py::new_reference_to(Py::Float(tol));
}

Py::Float ShapeFix_RootPy::getPrecision() const
{
    return Py::Float(getShapeFix_RootPtr()->Precision());
}

void ShapeFix_RootPy::setPrecision(Py::Float arg)
{
    getShapeFix_RootPtr()->SetPrecision(arg);
}

Py::Float ShapeFix_RootPy::getMinTolerance() const
{
    return Py::Float(getShapeFix_RootPtr()->MinTolerance());
}

void ShapeFix_RootPy::setMinTolerance(Py::Float arg)
{
    getShapeFix_RootPtr()->SetMinTolerance(arg);
}

Py::Float ShapeFix_RootPy::getMaxTolerance() const
{
    return Py::Float(getShapeFix_RootPtr()->MaxTolerance());
}

void ShapeFix_RootPy::setMaxTolerance(Py::Float arg)
{
    getShapeFix_RootPtr()->SetMaxTolerance(arg);
}

PyObject* ShapeFix_RootPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ShapeFix_RootPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
