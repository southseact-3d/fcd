#include <Mod/Part/PartGlobal.h>

#include "ShapeFix/ShapeFix_SplitCommonVertexPy.h"
#include "ShapeFix/ShapeFix_SplitCommonVertexPy.cpp"
#include "TopoShapePy.h"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string ShapeFix_SplitCommonVertexPy::representation() const
{
    return "<ShapeFix_SplitCommonVertex object>";
}

PyObject* ShapeFix_SplitCommonVertexPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python
                                                                                           // wrapper
{
    // create a new instance of ShapeFix_ShapePy
    return new ShapeFix_SplitCommonVertexPy(nullptr);
}

// constructor method
int ShapeFix_SplitCommonVertexPy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    PyObject* shape = nullptr;
    if (!PyArg_ParseTuple(args, "|O!", &TopoShapePy::Type, &shape)) {
        return -1;
    }

    setHandle(new ShapeFix_SplitCommonVertex);
    if (shape) {
        getShapeFix_SplitCommonVertexPtr()->Init(
            static_cast<TopoShapePy*>(shape)->getTopoShapePtr()->getShape()
        );
    }

    return 0;
}

PyObject* ShapeFix_SplitCommonVertexPy::init(PyObject* args)
{
    PyObject* shape;
    if (!PyArg_ParseTuple(args, "O!", &TopoShapePy::Type, &shape)) {
        return nullptr;
    }

    getShapeFix_SplitCommonVertexPtr()->Init(
        static_cast<TopoShapePy*>(shape)->getTopoShapePtr()->getShape()
    );
    Py_Return;
}

PyObject* ShapeFix_SplitCommonVertexPy::perform(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    getShapeFix_SplitCommonVertexPtr()->Perform();
    Py_Return;
}

PyObject* ShapeFix_SplitCommonVertexPy::shape(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    TopoShape shape = getShapeFix_SplitCommonVertexPtr()->Shape();
    return shape.getPyObject();
}

PyObject* ShapeFix_SplitCommonVertexPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ShapeFix_SplitCommonVertexPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
