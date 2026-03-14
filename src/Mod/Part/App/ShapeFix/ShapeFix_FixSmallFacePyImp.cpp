#include <Mod/Part/PartGlobal.h>

#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>


#include "ShapeFix/ShapeFix_FixSmallFacePy.h"
#include "ShapeFix/ShapeFix_FixSmallFacePy.cpp"
#include "TopoShapeFacePy.h"


using namespace Part;

std::string ShapeFix_FixSmallFacePy::representation() const
{
    return "<ShapeFix_FixSmallFace object>";
}

PyObject* ShapeFix_FixSmallFacePy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of ShapeFix_FacePy
    return new ShapeFix_FixSmallFacePy(new ShapeFix_FixSmallFace);
}

// constructor method
int ShapeFix_FixSmallFacePy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }
    return 0;
}

PyObject* ShapeFix_FixSmallFacePy::init(PyObject* args)
{
    PyObject* shape;
    if (!PyArg_ParseTuple(args, "O!", &TopoShapePy::Type, &shape)) {
        return nullptr;
    }

    getShapeFix_FixSmallFacePtr()->Init(
        static_cast<TopoShapePy*>(shape)->getTopoShapePtr()->getShape()
    );
    Py_Return;
}

PyObject* ShapeFix_FixSmallFacePy::perform(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    getShapeFix_FixSmallFacePtr()->Perform();
    Py_Return;
}

PyObject* ShapeFix_FixSmallFacePy::fixSpotFace(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    TopoShape sh = getShapeFix_FixSmallFacePtr()->FixSpotFace();
    return sh.getPyObject();
}

PyObject* ShapeFix_FixSmallFacePy::replaceVerticesInCaseOfSpot(PyObject* args)
{
    PyObject* face;
    if (!PyArg_ParseTuple(args, "O!", &TopoShapeFacePy::Type, &face)) {
        return nullptr;
    }

    TopoDS_Face shape = TopoDS::Face(
        static_cast<TopoShapeFacePy*>(face)->getTopoShapePtr()->getShape()
    );
    getShapeFix_FixSmallFacePtr()->ReplaceVerticesInCaseOfSpot(shape, 0.0);
    return TopoShape(shape).getPyObject();
}

PyObject* ShapeFix_FixSmallFacePy::removeFacesInCaseOfSpot(PyObject* args)
{
    PyObject* face;
    if (!PyArg_ParseTuple(args, "O!", &TopoShapeFacePy::Type, &face)) {
        return nullptr;
    }

    TopoDS_Face shape = TopoDS::Face(
        static_cast<TopoShapeFacePy*>(face)->getTopoShapePtr()->getShape()
    );
    bool ok = getShapeFix_FixSmallFacePtr()->RemoveFacesInCaseOfSpot(shape);
    return Py::new_reference_to(Py::Boolean(ok));
}

PyObject* ShapeFix_FixSmallFacePy::fixStripFace(PyObject* args)
{
    PyObject* wasdone = Py_False;
    if (!PyArg_ParseTuple(args, "|O!", &PyBool_Type, &wasdone)) {
        return nullptr;
    }

    TopoShape sh = getShapeFix_FixSmallFacePtr()->FixStripFace(Base::asBoolean(wasdone));
    return sh.getPyObject();
}

PyObject* ShapeFix_FixSmallFacePy::removeFacesInCaseOfStrip(PyObject* args)
{
    PyObject* face;
    if (!PyArg_ParseTuple(args, "O!", &TopoShapeFacePy::Type, &face)) {
        return nullptr;
    }

    TopoDS_Face shape = TopoDS::Face(
        static_cast<TopoShapeFacePy*>(face)->getTopoShapePtr()->getShape()
    );
    bool ok = getShapeFix_FixSmallFacePtr()->RemoveFacesInCaseOfStrip(shape);
    return Py::new_reference_to(Py::Boolean(ok));
}

PyObject* ShapeFix_FixSmallFacePy::fixSplitFace(PyObject* args)
{
    PyObject* shape;
    if (!PyArg_ParseTuple(args, "O!", &TopoShapePy::Type, &shape)) {
        return nullptr;
    }

    TopoShape sh = getShapeFix_FixSmallFacePtr()->FixSplitFace(
        static_cast<TopoShapeFacePy*>(shape)->getTopoShapePtr()->getShape()
    );
    return sh.getPyObject();
}

PyObject* ShapeFix_FixSmallFacePy::fixFace(PyObject* args)
{
    PyObject* face;
    if (!PyArg_ParseTuple(args, "O!", &TopoShapeFacePy::Type, &face)) {
        return nullptr;
    }

    TopoDS_Face shape = TopoDS::Face(
        static_cast<TopoShapeFacePy*>(face)->getTopoShapePtr()->getShape()
    );
    TopoShape sh = getShapeFix_FixSmallFacePtr()->FixFace(shape);
    return sh.getPyObject();
}

PyObject* ShapeFix_FixSmallFacePy::fixShape(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    TopoShape shape = getShapeFix_FixSmallFacePtr()->FixShape();
    return shape.getPyObject();
}

PyObject* ShapeFix_FixSmallFacePy::shape(PyObject* args)
{
    if (!PyArg_ParseTuple(args, "")) {
        return nullptr;
    }

    TopoShape sh = getShapeFix_FixSmallFacePtr()->Shape();
    return sh.getPyObject();
}

PyObject* ShapeFix_FixSmallFacePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int ShapeFix_FixSmallFacePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
