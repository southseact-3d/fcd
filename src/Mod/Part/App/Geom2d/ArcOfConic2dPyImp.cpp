#include <Geom2d_Conic.hxx>
#include <Geom2d_TrimmedCurve.hxx>


#include <Base/GeometryPyCXX.h>

#include "Geom2d/ArcOfConic2dPy.h"
#include "Geom2d/ArcOfConic2dPy.cpp"
#include "OCCError.h"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string ArcOfConic2dPy::representation() const
{
    return "<Arc of conic2d object>";
}

PyObject* ArcOfConic2dPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // never create such objects with the constructor
    PyErr_SetString(
        PyExc_RuntimeError,
        "You cannot create an instance of the abstract class 'ArcOfConic2d'."
    );
    return nullptr;
}

// constructor method
int ArcOfConic2dPy::PyInit(PyObject* /*args*/, PyObject* /*kwds*/)
{
    return -1;
}

Py::Object ArcOfConic2dPy::getLocation() const
{
    Base::Vector2d loc = getGeom2dArcOfConicPtr()->getLocation();
    return Base::Vector2dPy::create(loc);
}

void ArcOfConic2dPy::setLocation(Py::Object arg)
{
    Base::Vector2d loc = Py::toVector2d(arg.ptr());
    getGeom2dArcOfConicPtr()->setLocation(loc);
}

Py::Float ArcOfConic2dPy::getEccentricity() const
{
    Handle(Geom2d_TrimmedCurve) curve = Handle(Geom2d_TrimmedCurve)::DownCast(
        getGeom2dArcOfConicPtr()->handle()
    );
    Handle(Geom2d_Conic) conic = Handle(Geom2d_Conic)::DownCast(curve->BasisCurve());
    return Py::Float(conic->Eccentricity());
}

Py::Object ArcOfConic2dPy::getXAxis() const
{
    Handle(Geom2d_TrimmedCurve) curve = Handle(Geom2d_TrimmedCurve)::DownCast(
        getGeom2dArcOfConicPtr()->handle()
    );
    Handle(Geom2d_Conic) conic = Handle(Geom2d_Conic)::DownCast(curve->BasisCurve());
    gp_Dir2d xdir = conic->XAxis().Direction();
    return Base::Vector2dPy::create(xdir.X(), xdir.Y());
}

void ArcOfConic2dPy::setXAxis(Py::Object arg)
{
    Handle(Geom2d_TrimmedCurve) curve = Handle(Geom2d_TrimmedCurve)::DownCast(
        getGeom2dArcOfConicPtr()->handle()
    );
    Handle(Geom2d_Conic) conic = Handle(Geom2d_Conic)::DownCast(curve->BasisCurve());
    Base::Vector2d dir = Py::toVector2d(arg.ptr());
    gp_Ax2d xaxis = conic->XAxis();
    xaxis.SetDirection(gp_Dir2d(dir.x, dir.y));
    conic->SetXAxis(xaxis);
}

Py::Object ArcOfConic2dPy::getYAxis() const
{
    Handle(Geom2d_TrimmedCurve) curve = Handle(Geom2d_TrimmedCurve)::DownCast(
        getGeom2dArcOfConicPtr()->handle()
    );
    Handle(Geom2d_Conic) conic = Handle(Geom2d_Conic)::DownCast(curve->BasisCurve());
    gp_Dir2d ydir = conic->YAxis().Direction();
    return Base::Vector2dPy::create(ydir.X(), ydir.Y());
}

void ArcOfConic2dPy::setYAxis(Py::Object arg)
{
    Handle(Geom2d_TrimmedCurve) curve = Handle(Geom2d_TrimmedCurve)::DownCast(
        getGeom2dArcOfConicPtr()->handle()
    );
    Handle(Geom2d_Conic) conic = Handle(Geom2d_Conic)::DownCast(curve->BasisCurve());
    Base::Vector2d dir = Py::toVector2d(arg.ptr());
    gp_Ax2d yaxis = conic->YAxis();
    yaxis.SetDirection(gp_Dir2d(dir.x, dir.y));
    conic->SetYAxis(yaxis);
}

PyObject* ArcOfConic2dPy::getCustomAttributes(const char*) const
{
    return nullptr;
}

int ArcOfConic2dPy::setCustomAttributes(const char*, PyObject*)
{
    return 0;
}
