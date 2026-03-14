#include <BRep_Builder.hxx>
#include <Standard_Failure.hxx>
#include <TopoDS.hxx>
#include <TopoDS_CompSolid.hxx>


#include "OCCError.h"
#include "PartPyCXX.h"

// inclusion of the generated files (generated out of TopoShapeCompSolidPy.xml)
#include "TopoShapeCompSolidPy.h"
#include "TopoShapeCompSolidPy.cpp"
#include "TopoShapeSolidPy.h"
#include "TopoShapeOpCode.h"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string TopoShapeCompSolidPy::representation() const
{
    std::stringstream str;
    str << "<CompSolid object at " << getTopoShapePtr() << ">";

    return str.str();
}

PyObject* TopoShapeCompSolidPy::PyMake(struct _typeobject*, PyObject*, PyObject*)
{
    return new TopoShapeCompSolidPy(new TopoShape);
}

int TopoShapeCompSolidPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    if (PyArg_ParseTuple(args, "")) {
        // Undefined CompSolid
        getTopoShapePtr()->setShape(TopoDS_CompSolid());
        return 0;
    }

    PyErr_Clear();
    PyObject* pcObj;
    if (!PyArg_ParseTuple(args, "O", &pcObj)) {
        return -1;
    }
    try {
        getTopoShapePtr()->makeElementBoolean(Part::OpCodes::Compsolid, getPyShapes(pcObj));
    }
    _PY_CATCH_OCC(return (-1))
    return 0;
}

PyObject* TopoShapeCompSolidPy::add(PyObject* args)
{
    PyObject* obj;
    if (!PyArg_ParseTuple(args, "O!", &(Part::TopoShapeSolidPy::Type), &obj)) {
        return nullptr;
    }

    BRep_Builder builder;
    TopoDS_Shape comp = getTopoShapePtr()->getShape();
    if (comp.IsNull()) {
        builder.MakeCompSolid(TopoDS::CompSolid(comp));
    }
    auto shapes = getPyShapes(obj);

    try {
        for (auto& ts : shapes) {
            if (!ts.isNull()) {
                builder.Add(comp, ts.getShape());
            }
            else {
                Standard_Failure::Raise("Cannot empty shape to compound solid");
            }
        }
        auto& self = *getTopoShapePtr();
        shapes.push_back(self);
        TopoShape tmp(self.Tag, self.Hasher, comp);
        tmp.mapSubElement(shapes);
        self = tmp;
        Py_Return;
    }
    catch (Standard_Failure& e) {

        PyErr_SetString(PartExceptionOCCError, e.GetMessageString());
        return nullptr;
    }
}

PyObject* TopoShapeCompSolidPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int TopoShapeCompSolidPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
