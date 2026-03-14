#include <GCE2d_MakeArcOfHyperbola.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Hypr2d.hxx>


#include "Geom2d/ArcOfHyperbola2dPy.h"
#include "Geom2d/ArcOfHyperbola2dPy.cpp"
#include "Geom2d/Hyperbola2dPy.h"
#include "OCCError.h"


using namespace Part;

extern const char* gce_ErrorStatusText(gce_ErrorType et);

// returns a string which represents the object e.g. when printed in python
std::string ArcOfHyperbola2dPy::representation() const
{
    return "<ArcOfHyperbola2d object>";
}

PyObject* ArcOfHyperbola2dPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of ArcOfHyperbola2dPy and the Twin object
    return new ArcOfHyperbola2dPy(new Geom2dArcOfHyperbola);
}

// constructor method
int ArcOfHyperbola2dPy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    PyObject* o;
    double u1, u2;
    PyObject* sense = Py_True;
    if (PyArg_ParseTuple(args, "O!dd|O!", &(Part::Hyperbola2dPy::Type), &o, &u1, &u2, &PyBool_Type, &sense)) {
        try {
            Handle(Geom2d_Hyperbola) hyperbola = Handle(Geom2d_Hyperbola)::DownCast(
                static_cast<Hyperbola2dPy*>(o)->getGeom2dHyperbolaPtr()->handle()
            );
            GCE2d_MakeArcOfHyperbola arc(hyperbola->Hypr2d(), u1, u2, Base::asBoolean(sense));
            if (!arc.IsDone()) {
                PyErr_SetString(PartExceptionOCCError, gce_ErrorStatusText(arc.Status()));
                return -1;
            }

            getGeom2dArcOfHyperbolaPtr()->setHandle(arc.Value());
            return 0;
        }
        catch (Standard_Failure& e) {

            PyErr_SetString(PartExceptionOCCError, e.GetMessageString());
            return -1;
        }
        catch (...) {
            PyErr_SetString(PartExceptionOCCError, "creation of arc failed");
            return -1;
        }
    }

    // All checks failed
    PyErr_SetString(
        PyExc_TypeError,
        "ArcOfHyperbola constructor expects an hyperbola curve and a parameter range"
    );
    return -1;
}

Py::Float ArcOfHyperbola2dPy::getMajorRadius() const
{
    return Py::Float(getGeom2dArcOfHyperbolaPtr()->getMajorRadius());
}

void ArcOfHyperbola2dPy::setMajorRadius(Py::Float arg)
{
    getGeom2dArcOfHyperbolaPtr()->setMajorRadius((double)arg);
}

Py::Float ArcOfHyperbola2dPy::getMinorRadius() const
{
    return Py::Float(getGeom2dArcOfHyperbolaPtr()->getMinorRadius());
}

void ArcOfHyperbola2dPy::setMinorRadius(Py::Float arg)
{
    getGeom2dArcOfHyperbolaPtr()->setMinorRadius((double)arg);
}

Py::Object ArcOfHyperbola2dPy::getHyperbola() const
{
    Handle(Geom2d_TrimmedCurve) trim = Handle(Geom2d_TrimmedCurve)::DownCast(
        getGeom2dArcOfHyperbolaPtr()->handle()
    );
    Handle(Geom2d_Hyperbola) hyperbola = Handle(Geom2d_Hyperbola)::DownCast(trim->BasisCurve());
    return Py::asObject(new Hyperbola2dPy(new Geom2dHyperbola(hyperbola)));
}

PyObject* ArcOfHyperbola2dPy::getCustomAttributes(const char*) const
{
    return nullptr;
}

int ArcOfHyperbola2dPy::setCustomAttributes(const char*, PyObject*)
{
    return 0;
}
