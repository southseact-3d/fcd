#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <GeomFill_NSections.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <Standard_Version.hxx>
#include <TopLoc_Location.hxx>
#include <TopoDS.hxx>


#include "FeatureSections.h"


using namespace Surface;

PROPERTY_SOURCE(Surface::Sections, Part::Spline)

Sections::Sections()
{
    ADD_PROPERTY_TYPE(NSections, (nullptr), "Sections", App::Prop_None, "Section curves");
    NSections.setScope(App::LinkScope::Global);
}

App::DocumentObjectExecReturn* Sections::execute()
{
    TColGeom_SequenceOfCurve curveSeq;
    auto edge_obj = NSections.getValues();
    auto edge_sub = NSections.getSubValues();
    if (edge_obj.size() == edge_sub.size()) {
        for (std::size_t index = 0; index < edge_obj.size(); index++) {
            // get the part object
            App::DocumentObject* obj = edge_obj[index];
            const std::string& sub = edge_sub[index];
            if (obj && obj->isDerivedFrom<Part::Feature>()) {
                // get the sub-edge of the part's shape
                const Part::TopoShape& shape = static_cast<Part::Feature*>(obj)->Shape.getShape();
                TopoDS_Shape edge = shape.getSubShape(sub.c_str());
                if (!edge.IsNull() && edge.ShapeType() == TopAbs_EDGE) {
                    BRepAdaptor_Curve curve_adapt(TopoDS::Edge(edge));
                    const TopLoc_Location& loc = edge.Location();
                    Handle(Geom_TrimmedCurve) hCurve = new Geom_TrimmedCurve(
                        curve_adapt.Curve().Curve(),
                        curve_adapt.FirstParameter(),
                        curve_adapt.LastParameter()
                    );
                    if (!loc.IsIdentity()) {
                        hCurve->Transform(loc.Transformation());
                    }
                    curveSeq.Append(hCurve);
                }
            }
        }
    }

    if (curveSeq.Length() < 2) {
        return new App::DocumentObjectExecReturn("At least two sections are required.");
    }

    GeomFill_NSections fillOp(curveSeq);
    fillOp.ComputeSurface();

    Handle(Geom_BSplineSurface) aSurf = fillOp.BSplineSurface();
    if (aSurf.IsNull()) {
        return new App::DocumentObjectExecReturn("Failed to create surface from sections.");
    }

    BRepBuilderAPI_MakeFace mkFace(aSurf, Precision::Confusion());

    Shape.setValue(mkFace.Face());
    return StdReturn;
}
