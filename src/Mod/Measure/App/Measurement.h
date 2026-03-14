#ifndef MEASURE_MEASUREMENT_H
#define MEASURE_MEASUREMENT_H

#include <gp_Pnt.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>

#include <Base/BaseClass.h>
#include <Base/Vector3D.h>
#include <Mod/Measure/MeasureGlobal.h>


class TopoDS_Shape;
namespace Measure
{
enum class MeasureType
{
    Volumes,           // Measure the Volume(s)
    Edges,             // Measure the Edge(s)
    Line,              // One Line
    TwoLines,          // Two lines
    TwoParallelLines,  // Two parallel lines
    Circle,            // One circle
    CircleArc,         // One circle arc
    TwoCircles,
    CircleToEdge,
    CircleToSurface,
    CircleToCylinder,
    Surfaces,         // Measure the surface(s)
    Cylinder,         // One Cylinder
    CylinderSection,  // One cylinder section
    TwoCylinders,
    Cone,       // One Cone
    Sphere,     // One Sphere
    Torus,      // One Torus
    Plane,      // One Plane
    TwoPlanes,  // One Plane
    Points,
    PointToPoint,  // Measure between TWO points
    PointToEdge,   // Measure between ONE point and ONE edge
    PointToCircle,
    PointToSurface,  // Measure between ONE point and ONE surface
    PointToCylinder,
    EdgeToEdge,  // Measure between TWO edges
    Invalid
};

class MeasureExport Measurement: public Base::BaseClass
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    App::PropertyLinkSubList References3D;

public:
    Measurement();
    ~Measurement() override;

    void clear();
    bool has3DReferences();

    /// Add a reference
    int addReference3D(App::DocumentObject* obj, const std::string& subName);
    int addReference3D(App::DocumentObject* obj, const char* subName);

    MeasureType getType();
    MeasureType findType();

    // from base class
    PyObject* getPyObject() override;
    virtual unsigned int getMemSize() const;

    // Methods for distances (edge length, two points, edge and a point
    double length() const;
    Base::Vector3d delta() const;  // when would client use delta??
    double lineLineDistance() const;
    double circleCenterDistance() const;
    double planePlaneDistance() const;
    double cylinderAxisDistance() const;

    // Calculates the radius for an arc or circular edge
    double radius() const;

    // Calculates the diameter for a circle or a cylinder
    double diameter() const;

    // Calculates the angle between two edges
    double angle(const Base::Vector3d& param = Base::Vector3d(0, 0, 0)) const;  // param is never used???

    // Calculate the center of mass
    Base::Vector3d massCenter() const;

    // Calculate the volume of selected volumes
    double volume() const;

    // Calculate the area of selection
    double area() const;

    static Base::Vector3d toVector3d(const gp_Pnt gp)
    {
        return Base::Vector3d(gp.X(), gp.Y(), gp.Z());
    }

    bool planesAreParallel() const;
    bool linesAreParallel() const;

protected:
    // Hint parameter helps sort out compound shapes by specifying a subelement type
    // use hint = TopAbs_COMPOUND to give no hint
    TopoDS_Shape getShape(
        App::DocumentObject* obj,
        const char* subName,
        TopAbs_ShapeEnum hint = TopAbs_COMPOUND
    ) const;

private:
    MeasureType measureType;
    Py::SmartPtr PythonObject;
};


}  // namespace Measure


#endif  // MEASURE_MEASUREMENT_H
