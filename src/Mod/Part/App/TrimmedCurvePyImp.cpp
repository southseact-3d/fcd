#include <sstream>


#include "OCCError.h"
#include "TrimmedCurvePy.h"
#include "TrimmedCurvePy.cpp"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string TrimmedCurvePy::representation() const
{
    return "<Curve object>";
}

PyObject* TrimmedCurvePy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // never create such objects with the constructor
    PyErr_SetString(
        PyExc_RuntimeError,
        "You cannot create an instance of the abstract class 'TrimmedCurve'."
    );
    return nullptr;
}

// constructor method
int TrimmedCurvePy::PyInit(PyObject* /*args*/, PyObject* /*kwd*/)
{
    return 0;
}

PyObject* TrimmedCurvePy::setParameterRange(PyObject* args)
{
    Handle(Geom_Geometry) g = getGeomTrimmedCurvePtr()->handle();
    Handle(Geom_TrimmedCurve) c = Handle(Geom_TrimmedCurve)::DownCast(g);
    try {
        if (!c.IsNull()) {
            double u, v;
            u = c->FirstParameter();
            v = c->LastParameter();
            if (!PyArg_ParseTuple(args, "|dd", &u, &v)) {
                return nullptr;
            }
            getGeomTrimmedCurvePtr()->setRange(u, v);
            Py_Return;
        }
    }
    catch (Base::CADKernelError& e) {
        PyErr_SetString(PartExceptionOCCError, e.what());
        return nullptr;
    }

    PyErr_SetString(PartExceptionOCCError, "Geometry is not a trimmed curve");
    return nullptr;
}

PyObject* TrimmedCurvePy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int TrimmedCurvePy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
