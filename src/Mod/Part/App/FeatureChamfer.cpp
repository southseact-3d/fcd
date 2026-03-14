#include <BRepFilletAPI_MakeChamfer.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>


#include "FeatureChamfer.h"
#include "TopoShapeOpCode.h"


using namespace Part;

PROPERTY_SOURCE(Part::Chamfer, Part::FilletBase)

Chamfer::Chamfer() = default;

App::DocumentObjectExecReturn* Chamfer::execute()
{
    App::DocumentObject* link = Base.getValue();
    if (!link) {
        return new App::DocumentObjectExecReturn("No object linked");
    }

    try {
        TopoShape baseTopoShape
            = Feature::getTopoShape(link, ShapeOption::ResolveLink | ShapeOption::Transform);
        const auto& baseShape = baseTopoShape.getShape();
        BRepFilletAPI_MakeChamfer mkChamfer(baseShape);
        TopTools_IndexedDataMapOfShapeListOfShape mapEdgeFace;
        TopExp::MapShapesAndAncestors(baseShape, TopAbs_EDGE, TopAbs_FACE, mapEdgeFace);
        TopTools_IndexedMapOfShape mapOfEdges;
        std::vector<Part::FilletElement> edges = Edges.getValues();
        TopExp::MapShapes(baseShape, TopAbs_EDGE, mapOfEdges);
        std::string fullErrMsg;

        const auto& vals = EdgeLinks.getSubValues();
        const auto& subs = EdgeLinks.getShadowSubs();
        if (subs.size() != (size_t)Edges.getSize()) {
            return new App::DocumentObjectExecReturn("Edge link size mismatch");
        }
        size_t i = 0;
        for (const auto& info : edges) {
            auto& sub = subs[i];
            auto& ref = sub.newName.empty() ? vals[i] : sub.newName;
            auto& oldName = sub.oldName.empty() ? "" : sub.oldName;
            ++i;

            if (Data::hasMissingElement(ref.c_str()) || Data::hasMissingElement(oldName.c_str())) {
                fullErrMsg.append("Missing edge link: ");
                fullErrMsg.append(ref);
                fullErrMsg.append("\n");

                auto removeIt = std::remove(edges.begin(), edges.end(), info);
                edges.erase(removeIt, edges.end());

                continue;
            }
            // Toponaming project March 2024:  Replaced this code because it wouldn't work:
            //            TopoDS_Shape edge;
            //            try {
            //                edge = baseTopoShape.getSubShape(ref.c_str());
            //            }catch(...){}
            auto id = Data::MappedName(ref.c_str()).toIndexedName().getIndex();
            const TopoDS_Edge& edge = TopoDS::Edge(mapOfEdges.FindKey(id));
            if (edge.IsNull()) {
                return new App::DocumentObjectExecReturn("Invalid edge link");
            }
            double radius1 = info.radius1;
            double radius2 = info.radius2;
            const TopoDS_Face& face = TopoDS::Face(mapEdgeFace.FindFromKey(edge).First());
            mkChamfer.Add(radius1, radius2, TopoDS::Edge(edge), face);
        }

        if (!fullErrMsg.empty()) {
            return new App::DocumentObjectExecReturn(fullErrMsg);
        }
        Edges.setValues(edges);

        TopoDS_Shape shape = mkChamfer.Shape();
        if (shape.IsNull()) {
            return new App::DocumentObjectExecReturn("Resulting shape is null");
        }

        TopoShape res(0);
        this->Shape.setValue(res.makeElementShape(mkChamfer, baseTopoShape, Part::OpCodes::Chamfer));
        return Part::FilletBase::execute();
    }
    catch (Standard_Failure& e) {
        return new App::DocumentObjectExecReturn(e.GetMessageString());
    }
}
