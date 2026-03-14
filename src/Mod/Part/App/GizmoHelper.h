#ifndef GIZMO_HELPER_H
#define GIZMO_HELPER_H

#include <Base/Vector3D.h>
#include <Mod/Part/App/Geometry.h>
#include <Mod/Part/App/TopoShape.h>


struct PartExport EdgeMidPointProps
{
    Base::Vector3d position;
    Base::Vector3d tangent;
    double middle;
};
EdgeMidPointProps PartExport getEdgeMidPointProps(Part::TopoShape& edge);

Base::Vector3d PartExport getCentreOfMassFromFace(TopoDS_Face& face);

struct PartExport PointOnFaceNearEdgeProps
{
    enum class State : std::uint8_t
    {
        OnFace,
        OutsideFace,
        Undefined
    };

    Base::Vector3d position;
    Base::Vector3d normal;
    State state;
};
PointOnFaceNearEdgeProps PartExport
getFaceNormalFromPointNearEdge(Part::TopoShape& edge, double middle, TopoDS_Face& face);

Base::Vector3d PartExport getFaceNormalFromPoint(Base::Vector3d& point, TopoDS_Face& face);

std::pair<TopoDS_Face, TopoDS_Face> PartExport
getAdjacentFacesFromEdge(Part::TopoShape& edge, Part::TopoShape& baseShape);

struct PartExport DraggerPlacementProps
{
    Base::Vector3d position;
    Base::Vector3d dir;
};
DraggerPlacementProps PartExport
getDraggerPlacementFromEdgeAndFace(Part::TopoShape& edge, TopoDS_Face& face);

DraggerPlacementProps PartExport
getDraggerPlacementFromEdgeAndFace(Part::TopoShape& edge, Part::TopoShape& face);

std::vector<Part::TopoShape> PartExport getAdjacentEdgesFromFace(Part::TopoShape& face);

Base::Vector3d PartExport getMidPointFromFace(Part::TopoShape& face);

Base::Vector3d PartExport getMidPointFromProfile(Part::TopoShape& profile);

struct PartExport DraggerNormalProps
{
    Base::Vector3d normal;
    Base::Vector3d faceNormal;
};
struct PartExport DraggerPlacementPropsWithNormals
{
    DraggerPlacementProps placementProps;
    std::optional<DraggerNormalProps> normalProps;
};
std::optional<DraggerPlacementPropsWithNormals> PartExport
getDraggerPlacementFromPlaneAndFace(Part::TopoShape& face, gp_Pln& plane);

#endif /* GIZMO_HELPER_H */
