#include <Geom_Curve.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>


#include <Base/GeometryPyCXX.h>
#include <Base/VectorPy.h>

#include "SurfaceOfExtrusionPy.h"
#include "SurfaceOfExtrusionPy.cpp"
#include "GeometryCurvePy.h"
#include "OCCError.h"


using namespace Part;

// returns a string which represents the object e.g. when printed in python
std::string SurfaceOfExtrusionPy::representation() const
{
    return {"<SurfaceOfExtrusion object>"};
}

PyObject* SurfaceOfExtrusionPy::PyMake(struct _typeobject*, PyObject*, PyObject*)  // Python wrapper
{
    // create a new instance of SurfaceOfExtrusionPy and the Twin object
    return new SurfaceOfExtrusionPy(new GeomSurfaceOfExtrusion);
}

// constructor method
int SurfaceOfExtrusionPy::PyInit(PyObject* args, PyObject* /*kwd*/)
{
    PyObject* pGeom;
    PyObject* pDir;
    if (!PyArg_ParseTuple(args, "O!O!", &(GeometryPy::Type), &pGeom, &(Base::VectorPy::Type), &pDir)) {
        return -1;
    }

    GeometryPy* pcGeo = static_cast<GeometryPy*>(pGeom);
    Handle(Geom_Curve) curve = Handle(Geom_Curve)::DownCast(pcGeo->getGeometryPtr()->handle());
    if (curve.IsNull()) {
        PyErr_SetString(PyExc_TypeError, "geometry is not a curve");
        return -1;
    }

    try {
        Base::Vector3d dir = static_cast<Base::VectorPy*>(pDir)->value();
        Handle(Geom_SurfaceOfLinearExtrusion) curve2
            = new Geom_SurfaceOfLinearExtrusion(curve, gp_Dir(dir.x, dir.y, dir.z));
        getGeomSurfaceOfExtrusionPtr()->setHandle(curve2);
        return 0;
    }
    catch (Standard_Failure& e) {

        PyErr_SetString(PartExceptionOCCError, e.GetMessageString());
        return -1;
    }
}

Py::Object SurfaceOfExtrusionPy::getDirection() const
{
    Handle(Geom_SurfaceOfLinearExtrusion) curve = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(
        getGeometryPtr()->handle()
    );
    const gp_Dir& dir = curve->Direction();
    return Py::Vector(Base::Vector3d(dir.X(), dir.Y(), dir.Z()));
}

void SurfaceOfExtrusionPy::setDirection(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(Base::VectorPy::Type))) {
        Base::Vector3d dir = static_cast<Base::VectorPy*>(p)->value();
        Handle(Geom_SurfaceOfLinearExtrusion) curve = Handle(
            Geom_SurfaceOfLinearExtrusion
        )::DownCast(getGeometryPtr()->handle());
        curve->SetDirection(gp_Dir(dir.x, dir.y, dir.z));
    }
    else if (PyObject_TypeCheck(p, &PyTuple_Type)) {
        Base::Vector3d dir = Base::getVectorFromTuple<double>(p);
        Handle(Geom_SurfaceOfLinearExtrusion) curve = Handle(
            Geom_SurfaceOfLinearExtrusion
        )::DownCast(getGeometryPtr()->handle());
        curve->SetDirection(gp_Dir(dir.x, dir.y, dir.z));
    }
    else {
        std::string error = std::string("type must be 'Vector', not ");
        error += p->ob_type->tp_name;
        throw Py::TypeError(error);
    }
}

namespace Part
{
extern const Py::Object makeGeometryCurvePy(const Handle(Geom_Curve) & c);
}

Py::Object SurfaceOfExtrusionPy::getBasisCurve() const
{
    Handle(Geom_SurfaceOfLinearExtrusion) surf = Handle(Geom_SurfaceOfLinearExtrusion)::DownCast(
        getGeometryPtr()->handle()
    );
    Handle(Geom_Curve) curve = surf->BasisCurve();
    return makeGeometryCurvePy(curve);
}

void SurfaceOfExtrusionPy::setBasisCurve(Py::Object arg)
{
    PyObject* p = arg.ptr();
    if (PyObject_TypeCheck(p, &(GeometryPy::Type))) {
        GeometryPy* pcGeo = static_cast<GeometryPy*>(p);
        Handle(Geom_Curve) curve = Handle(Geom_Curve)::DownCast(pcGeo->getGeometryPtr()->handle());
        if (curve.IsNull()) {
            throw Py::TypeError("geometry is not a curve");
        }

        try {
            Handle(Geom_SurfaceOfLinearExtrusion) curve2 = Handle(
                Geom_SurfaceOfLinearExtrusion
            )::DownCast(getGeometryPtr()->handle());
            curve2->SetBasisCurve(curve);
        }
        catch (Standard_Failure& e) {
            throw Py::RuntimeError(e.GetMessageString());
        }
    }
}

PyObject* SurfaceOfExtrusionPy::getCustomAttributes(const char* /*attr*/) const
{
    return nullptr;
}

int SurfaceOfExtrusionPy::setCustomAttributes(const char* /*attr*/, PyObject* /*obj*/)
{
    return 0;
}
