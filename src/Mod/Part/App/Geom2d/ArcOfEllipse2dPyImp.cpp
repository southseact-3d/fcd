#include <GCE2d_MakeArcOfEllipse.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Elips2d.hxx>


#include "Geom2d/ArcOfEllipse2dPy.h"
#include "Geom2d/ArcOfEllipse2dPy.cpp"
#include "Geom2d/Ellipse2dPy.h"
#include "OCCError.h"


using namespace Part;

extern const char* gce_ErrorStatusText(gce_ErrorType et);

// returns a string which represents the object e.g. when printed in python
std::string ArcOfEllipse2dPy::representation() const
{
    return "<Arc of ellipse2d object>";
}

PyObject* ArcOfEllipse2dPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of ArcOfEllipse2dPy and the Twin object
    return new ArcOfEllipse2dPy(new Geom2dArcOfEllipse);
}

// constructor method
int ArcOfEllipse2dPy::PyInit(PyObject* args, PyObject* /*kwds*/)
{
    PyObject* o;
    double u1, u2;
    PyObject* sense = Py_True;
    if (PyArg_ParseTuple(args, "O!dd|O!", &(Part::Ellipse2dPy::Type), &o, &u1, &u2, &PyBool_Type, &sense)) {
        try {
            Handle(Geom2d_Ellipse) ellipse = Handle(Geom2d_Ellipse)::DownCast(
                static_cast<Ellipse2dPy*>(o)->getGeom2dEllipsePtr()->handle()
            );
            GCE2d_MakeArcOfEllipse arc(ellipse->Elips2d(), u1, u2, Base::asBoolean(sense));
            if (!arc.IsDone()) {
                PyErr_SetString(PartExceptionOCCError, gce_ErrorStatusText(arc.Status()));
                return -1;
            }

            getGeom2dArcOfEllipsePtr()->setHandle(arc.Value());
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
        "ArcOfEllipse2d constructor expects an ellipse curve and a parameter range"
    );
    return -1;
}

Py::Float ArcOfEllipse2dPy::getMajorRadius() const
{
    return Py::Float(getGeom2dArcOfEllipsePtr()->getMajorRadius());
}

void ArcOfEllipse2dPy::setMajorRadius(Py::Float arg)
{
    getGeom2dArcOfEllipsePtr()->setMajorRadius((double)arg);
}

Py::Float ArcOfEllipse2dPy::getMinorRadius() const
{
    return Py::Float(getGeom2dArcOfEllipsePtr()->getMinorRadius());
}

void ArcOfEllipse2dPy::setMinorRadius(Py::Float arg)
{
    getGeom2dArcOfEllipsePtr()->setMinorRadius((double)arg);
}

Py::Object ArcOfEllipse2dPy::getEllipse() const
{
    Handle(Geom2d_TrimmedCurve) curve = Handle(Geom2d_TrimmedCurve)::DownCast(
        getGeom2dArcOfConicPtr()->handle()
    );
    Handle(Geom2d_Ellipse) ellipse = Handle(Geom2d_Ellipse)::DownCast(curve->BasisCurve());
    return Py::asObject(new Ellipse2dPy(new Geom2dEllipse(ellipse)));
}

PyObject* ArcOfEllipse2dPy::getCustomAttributes(const char*) const
{
    return nullptr;
}

int ArcOfEllipse2dPy::setCustomAttributes(const char*, PyObject*)
{
    return 0;
}
