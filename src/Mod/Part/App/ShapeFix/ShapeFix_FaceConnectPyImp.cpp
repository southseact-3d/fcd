#include <Mod/Part/PartGlobal.h>

#include <TopoDS.hxx>
#include <TopoDS_Shell.hxx>


#include "ShapeFix/ShapeFix_FaceConnectPy.h"
#include "ShapeFix/ShapeFix_FaceConnectPy.cpp"
#include "TopoShapeFacePy.h"
#include "TopoShapeShellPy.h"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string ShapeFix_FaceConnectPy::representation() const
{
    return "<ShapeFix_FaceConnect object>";
}

PyObject* ShapeFix_FaceConnectPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    return new ShapeFix_FaceConnectPy(new ShapeFix_FaceConnect);
}

// constructor method
int ShapeFix_FaceConnectPy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }
    return 0;
}

PyObject* ShapeFix_FaceConnectPy::add(PyObject* args)
{
    PyObject* face1;
    PyObject* face2;
    if (!PyArg_ParseTuple(args, "O!O!", &TopoShapeFacePy::Type, &face1, &TopoShapeFacePy::Type, &face2)) {
        return nullptr;
    }

    TopoDS_Shape f1 = static_cast<TopoShapeFacePy*>(face1)->getTopoShapePtr()->getShape();
    TopoDS_Shape f2 = static_cast<TopoShapeFacePy*>(face2)->getTopoShapePtr()->getShape();
    getShapeFix_FaceConnectPtr()->Add(TopoDS::Face(f1), TopoDS::Face(f2));
    Py_Return;
}

PyObject* ShapeFix_FaceConnectPy::build(PyObject* args)
{
    PyObject* shell;
    double sewtoler;
    double fixtoler;
    if (!PyArg_ParseTuple(args, "O!dd", &TopoShapeShellPy::Type, &shell, &sewtoler, &fixtoler)) {
        return nullptr;
    }

    TopoDS_Shape sh = static_cast<TopoShapeShellPy*>(shell)->getTopoShapePtr()->getShape();
    TopoShape res = getShapeFix_FaceConnectPtr()->Build(TopoDS::Shell(sh), sewtoler, fixtoler);
    return res.getPyObject();
}

PyObject* ShapeFix_FaceConnectPy::clear(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    getShapeFix_FaceConnectPtr()->Clear();
    Py_Return;
}

PyObject* ShapeFix_FaceConnectPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ShapeFix_FaceConnectPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
