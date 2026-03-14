//this file originally part of Drawing workbench
//migrated to TechDraw workbench 2022-01-26 by Wandererfan

#ifndef TECHDRAW_EXPORT_H
#define TECHDRAW_EXPORT_H

#include <string>
#include <TopoDS_Edge.hxx>

#include <Mod/TechDraw/TechDrawGlobal.h>


class TopoDS_Shape;
class BRepAdaptor_Curve;

namespace TechDraw
{

class TechDrawExport TechDrawOutput
{
public:
    // If the curve is approximately a circle it will be returned,
    // otherwise a null edge is returned.
    TopoDS_Edge asCircle(const BRepAdaptor_Curve&) const;
    TopoDS_Edge asBSpline(const BRepAdaptor_Curve&, int maxDegree) const;
};

class TechDrawExport SVGOutput : public TechDrawOutput
{
public:
    SVGOutput();
    std::string exportEdges(const TopoDS_Shape&);

private:
    void printCircle(const BRepAdaptor_Curve&, std::ostream&);
    void printEllipse(const BRepAdaptor_Curve&, int id, std::ostream&);
    void printBSpline(const BRepAdaptor_Curve&, int id, std::ostream&);
    void printBezier(const BRepAdaptor_Curve&, int id, std::ostream&);
    void printGeneric(const BRepAdaptor_Curve&, int id, std::ostream&);
};

/* dxf output section - Dan Falck 2011/09/25  */
class TechDrawExport DXFOutput : public TechDrawOutput
{
public:
    DXFOutput();
    std::string exportEdges(const TopoDS_Shape&);

private:
    void printHeader(std::ostream& out);
    void printCircle(const BRepAdaptor_Curve&, std::ostream&);
    void printEllipse(const BRepAdaptor_Curve&, int id, std::ostream&);
    void printBSpline(const BRepAdaptor_Curve&, int id, std::ostream&);
    void printGeneric(const BRepAdaptor_Curve&, int id, std::ostream&);
};

} //namespace TechDraw

#endif // TECHDRAW_EXPORT_H
