#include <Mod/Part/PartGlobal.h>

#include <TopoDS.hxx>


#include "ShapeFix/ShapeFix_ShapeTolerancePy.h"
#include "ShapeFix/ShapeFix_ShapeTolerancePy.cpp"
#include "TopoShapePy.h"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string ShapeFix_ShapeTolerancePy::representation() const
{
    return "<ShapeFix_ShapeTolerance object>";
}

PyObject* ShapeFix_ShapeTolerancePy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    return new ShapeFix_ShapeTolerancePy(new ShapeFix_ShapeTolerance);
}

// constructor method
int ShapeFix_ShapeTolerancePy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }
    return 0;
}

PyObject* ShapeFix_ShapeTolerancePy::limitTolerance(PyObject* args)
{
    PyObject* shape;
    double tmin;
    double tmax = 0.0;
    TopAbs_ShapeEnum styp = TopAbs_SHAPE;
    if (!PyArg_ParseTuple(args, "O!d|di", &TopoShapePy::Type, &shape, &tmin, &tmax, &styp)) {
        return nullptr;
    }

    TopoDS_Shape sh = static_cast<TopoShapePy*>(shape)->getTopoShapePtr()->getShape();
    bool ok = getShapeFix_ShapeTolerancePtr()->LimitTolerance(sh, tmin, tmax, styp);
    return Py::new_reference_to(Py::Boolean(ok));
}

PyObject* ShapeFix_ShapeTolerancePy::setTolerance(PyObject* args)
{
    PyObject* shape;
    double prec;
    TopAbs_ShapeEnum styp = TopAbs_SHAPE;
    if (!PyArg_ParseTuple(args, "O!d|i", &TopoShapePy::Type, &shape, &prec, &styp)) {
        return nullptr;
    }

    TopoDS_Shape sh = static_cast<TopoShapePy*>(shape)->getTopoShapePtr()->getShape();
    getShapeFix_ShapeTolerancePtr()->SetTolerance(sh, prec, styp);
    Py_Return;
}

PyObject* ShapeFix_ShapeTolerancePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ShapeFix_ShapeTolerancePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
