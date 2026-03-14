#ifndef MESH_PROJECTION_H
#define MESH_PROJECTION_H

#include <Base/BoundBox.h>
#include <vector>

#include "Elements.h"


using Base::Vector3f;

#ifdef FC_USE_OCC
class TopoDS_Edge;
class TopoDS_Shape;
#endif

namespace MeshCore
{

class MeshFacetGrid;
class MeshKernel;
class MeshGeomFacet;

class MeshExport MeshProjection
{
public:
    explicit MeshProjection(const MeshKernel&);

    bool projectLineOnMesh(
        const MeshFacetGrid& grid,
        const Base::Vector3f& p1,
        FacetIndex f1,
        const Base::Vector3f& p2,
        FacetIndex f2,
        const Base::Vector3f& view,
        std::vector<Base::Vector3f>& polyline
    );

protected:
    bool bboxInsideRectangle(
        const Base::BoundBox3f& bbox,
        const Base::Vector3f& p1,
        const Base::Vector3f& p2,
        const Base::Vector3f& view
    ) const;
    bool isPointInsideDistance(
        const Base::Vector3f& p1,
        const Base::Vector3f& p2,
        const Base::Vector3f& pt
    ) const;
    bool connectLines(
        std::list<std::pair<Base::Vector3f, Base::Vector3f>>& cutLines,
        const Base::Vector3f& startPoint,
        const Base::Vector3f& endPoint,
        std::vector<Base::Vector3f>& polyline
    ) const;

private:
    const MeshKernel& kernel;
};

}  // namespace MeshCore

#endif  // MESH_PROJECTION_H
