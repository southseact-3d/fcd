#include <Mod/Part/PartGlobal.h>

#include <ShapeBuild_ReShape.hxx>


#include "ShapeFix/ShapeFix_FixSmallSolidPy.h"
#include "ShapeFix/ShapeFix_FixSmallSolidPy.cpp"
#include "TopoShapePy.h"


using namespace Part;

std::string ShapeFix_FixSmallSolidPy::representation() const
{
    return "<ShapeFix_FixSmallSolid object>";
}

PyObject* ShapeFix_FixSmallSolidPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    return new ShapeFix_FixSmallSolidPy(new ShapeFix_FixSmallSolid);
}

// constructor method
int ShapeFix_FixSmallSolidPy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }
    return 0;
}

PyObject* ShapeFix_FixSmallSolidPy::setFixMode(PyObject* args)
{
    int mode;
    if (!PyArg_ParseTuple(args, "i", &mode)) {
        return nullptr;
    }

    getShapeFix_FixSmallSolidPtr()->SetFixMode(mode);
    Py_Return;
}

PyObject* ShapeFix_FixSmallSolidPy::setVolumeThreshold(PyObject* args)
{
    double value = -1.0;
    if (!PyArg_ParseTuple(args, "|d", &value)) {
        return nullptr;
    }

    getShapeFix_FixSmallSolidPtr()->SetVolumeThreshold(value);
    Py_Return;
}

PyObject* ShapeFix_FixSmallSolidPy::setWidthFactorThreshold(PyObject* args)
{
    double value = -1.0;
    if (!PyArg_ParseTuple(args, "|d", &value)) {
        return nullptr;
    }

    getShapeFix_FixSmallSolidPtr()->SetWidthFactorThreshold(value);
    Py_Return;
}

PyObject* ShapeFix_FixSmallSolidPy::remove(PyObject* args)
{
    PyObject* shape;
    if (!PyArg_ParseTuple(args, "O!", &TopoShapePy::Type, &shape)) {
        return nullptr;
    }

    Handle(ShapeBuild_ReShape) context = new ShapeBuild_ReShape();
    TopoShape sh = getShapeFix_FixSmallSolidPtr()->Remove(
        static_cast<TopoShapePy*>(shape)->getTopoShapePtr()->getShape(),
        context
    );
    return sh.getPyObject();
}

PyObject* ShapeFix_FixSmallSolidPy::merge(PyObject* args)
{
    PyObject* shape;
    if (!PyArg_ParseTuple(args, "O!", &TopoShapePy::Type, &shape)) {
        return nullptr;
    }

    Handle(ShapeBuild_ReShape) context = new ShapeBuild_ReShape();
    TopoShape sh = getShapeFix_FixSmallSolidPtr()->Merge(
        static_cast<TopoShapePy*>(shape)->getTopoShapePtr()->getShape(),
        context
    );
    return sh.getPyObject();
}

PyObject* ShapeFix_FixSmallSolidPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ShapeFix_FixSmallSolidPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
