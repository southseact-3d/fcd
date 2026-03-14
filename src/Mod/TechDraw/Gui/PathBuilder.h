// a class for converting geometry into QPainterPaths


#ifndef PATHBUILDER_H
#define PATHBUILDER_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Mod/TechDraw/App/Geometry.h>

#include "QGIViewPart.h"

namespace TechDrawGui {

class TechDrawGuiExport PathBuilder {
public:
    PathBuilder() {}
    PathBuilder(QGIViewPart* qvp) { m_qvp = qvp; }
    ~PathBuilder() = default;

    QPainterPath geomToPainterPath(TechDraw::BaseGeomPtr baseGeom, double rot) const;
    void pathArc(QPainterPath& path, double rx, double ry, double x_axis_rotation,
                          bool large_arc_flag, bool sweep_flag, double x, double y, double curx,
                          double cury) const;
    void pathArcSegment(QPainterPath& path, double xc, double yc, double th0, double th1,
                                 double rx, double ry, double xAxisRotation) const;

private:
    QGIViewPart* m_qvp;

};

} //end namespace TechDraw
#endif
