#ifndef TECHDRAW_COSMETIC_VERTEX_H
#define TECHDRAW_COSMETIC_VERTEX_H

#include <App/FeaturePython.h>
#include <Base/Persistence.h>
#include <Base/Vector3D.h>

#include "Geometry.h"


class TopoDS_Edge;

namespace TechDraw {
class DrawViewPart;

class TechDrawExport CosmeticVertex: public Base::Persistence, public TechDraw::Vertex
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    CosmeticVertex();
    CosmeticVertex(const CosmeticVertex* cv);
    CosmeticVertex(const Base::Vector3d& loc);
    ~CosmeticVertex() override = default;

    Base::Vector3d point() const  { return permaPoint; };
    void point(Base::Vector3d newPoint) { permaPoint = newPoint; }
    void move(const Base::Vector3d& newPos);
    void moveRelative(const Base::Vector3d& movement);

    std::string toString() const;
    void dump(const char* title) override;
    Base::Vector3d scaled(const double factor) const;
    Base::Vector3d rotatedAndScaled(const double scale, const double rotDegrees) const;

    static Base::Vector3d makeCanonicalPoint(DrawViewPart* dvp, Base::Vector3d point, bool unscale = true);
    static Base::Vector3d makeCanonicalPointInverted(DrawViewPart* dvp, Base::Vector3d invertedPoint, bool unscale = true);
    static bool restoreCosmetic();

    // Persistence implementer ---------------------
    unsigned int getMemSize() const override;
    void Save(Base::Writer &/*writer*/) const override;
    void Restore(Base::XMLReader &/*reader*/) override;

    PyObject *getPyObject() override;
    CosmeticVertex* copy() const;
    CosmeticVertex* clone() const;

    Base::Vector3d permaPoint{Base::Vector3d()};           //permanent, unscaled value
    int            linkGeom{-1};             //connection to corresponding "geom" Vertex (fragile - index based!)
                                         //better to do reverse search for CosmeticTag in vertex geometry
    Base::Color     color{Base::Color()};
    double         size{1.0};
    int            style{1};
    bool           visible{true};              //base class vertex also has visible property

protected:
    Py::Object PythonObject;
};

} //end namespace TechDraw

#endif  // TECHDRAW_COSMETIC_VERTEX_H
