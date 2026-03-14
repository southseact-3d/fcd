// a class to handle changes to dimension reference geometry

#ifndef GEOMETRYMATCHER_H
#define GEOMETRYMATCHER_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <DrawViewDimension.h>

namespace Part
{
class TopoShape;
}

namespace TechDraw
{

class TechDrawExport GeometryMatcher
{
public:
    GeometryMatcher() = default;

    bool compareGeometry(const Part::TopoShape& geom1, const Part::TopoShape& geom2);

    double getPointTolerance() const
    {
        return m_pointTolerance;
    }
    void setPointTolerance(double tol)
    {
        m_pointTolerance = tol;
    }

private:
    static bool comparePoints(const TopoDS_Shape& shape1, const TopoDS_Shape& shape2);
    static bool compareEdges(const TopoDS_Shape& shape1, const TopoDS_Shape& shape2);
    static bool compareFaces(const TopoDS_Shape& shape1, const TopoDS_Shape& shape2);

    static bool compareLines(const TopoDS_Edge& edge1, const TopoDS_Edge& edge2);
    static bool compareCircles(const TopoDS_Edge& edge1, const TopoDS_Edge& edge2);
    static bool compareEllipses(const TopoDS_Edge& edge1, const TopoDS_Edge& edge2);
    static bool compareBSplines(const TopoDS_Edge& edge1, const TopoDS_Edge& edge2);
    static bool compareDifferent(const TopoDS_Edge& edge1, const TopoDS_Edge& edge2);

    static bool compareCircleArcs(const TopoDS_Edge& edge1, const TopoDS_Edge& edge2);
    static bool compareEllipseArcs(const TopoDS_Edge& edge1, const TopoDS_Edge& edge2);
    static bool compareEndPoints(const TopoDS_Edge& edge1, const TopoDS_Edge& edge2);

    double m_pointTolerance {EWTOLERANCE};
};

}  // end namespace TechDraw
#endif
