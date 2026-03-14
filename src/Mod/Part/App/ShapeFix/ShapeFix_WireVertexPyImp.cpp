#include <Mod/Part/PartGlobal.h>

#include <TopoDS.hxx>


#include "ShapeFix/ShapeFix_WireVertexPy.h"
#include "ShapeFix/ShapeFix_WireVertexPy.cpp"
#include "TopoShapeWirePy.h"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string ShapeFix_WireVertexPy::representation() const
{
    return "<ShapeFix_WireVertex object>";
}

PyObject* ShapeFix_WireVertexPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    return new ShapeFix_WireVertexPy(new ShapeFix_WireVertex);
}

// constructor method
int ShapeFix_WireVertexPy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }

    return 0;
}

PyObject* ShapeFix_WireVertexPy::init(PyObject* args)
{
    PyObject* shape;
    double prec;
    if (!PyArg_ParseTuple(args, "O!d", &TopoShapeWirePy::Type, &shape, &prec)) {
        return nullptr;
    }

    TopoDS_Shape sh = static_cast<TopoShapePy*>(shape)->getTopoShapePtr()->getShape();
    getShapeFix_WireVertexPtr()->Init(TopoDS::Wire(sh), prec);
    Py_Return;
}

PyObject* ShapeFix_WireVertexPy::wire(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    TopoShape w = getShapeFix_WireVertexPtr()->Wire();
    return w.getPyObject();
}

PyObject* ShapeFix_WireVertexPy::fixSame(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    int num = getShapeFix_WireVertexPtr()->FixSame();
    return Py::new_reference_to(Py::Long(num));
}

PyObject* ShapeFix_WireVertexPy::fix(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    int num = getShapeFix_WireVertexPtr()->Fix();
    return Py::new_reference_to(Py::Long(num));
}

PyObject* ShapeFix_WireVertexPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ShapeFix_WireVertexPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
