#ifndef _MeshAlgos_h_
#define _MeshAlgos_h_

#include <vector>

#include "CurveProjector.h"


class TopoDS_Edge;
class TopoDS_Shape;

namespace MeshCore
{
class MeshKernel;
}

using MeshCore::MeshKernel;

namespace MeshPart
{

/** The mesh algorithms container class
 */
class MeshPartExport MeshAlgos
{
public:
    /** Calculate per Vertex normals and adds the Normal property bag
     */
    static void offset(MeshCore::MeshKernel* Mesh, float fSize);
    static void offsetSpecial2(MeshCore::MeshKernel* Mesh, float fSize);
    static void offsetSpecial(MeshCore::MeshKernel* Mesh, float fSize, float zmax, float zmin);

    /** makes a boolean add
     * The int Type stears the boolean oberation: 0=add;1=intersection;2=diff
     */
    static MeshCore::MeshKernel* boolean(
        MeshCore::MeshKernel* Mesh1,
        MeshCore::MeshKernel* Mesh2,
        MeshCore::MeshKernel* pResult,
        int Type = 0
    );

    static void cutByShape(
        const TopoDS_Shape& aShape,
        const MeshCore::MeshKernel* pMesh,
        MeshCore::MeshKernel* pToolMesh
    );

    /// helper to discredicice a Edge...
    static void GetSampledCurves(
        const TopoDS_Edge& aEdge,
        std::vector<Base::Vector3f>& rclPoints,
        unsigned long ulNbOfPoints = 30
    );

    /// creates a mesh loft on base of a curve and an up vector
    static void LoftOnCurve(
        MeshCore::MeshKernel& ResultMesh,
        const TopoDS_Shape& Shape,
        const std::vector<Base::Vector3f>& poly,
        const Base::Vector3f& up = Base::Vector3f(0, 0, 1),
        float MaxSize = 0.1
    );


    static void cutByCurve(
        MeshCore::MeshKernel* pMesh,
        const std::vector<CurveProjector::FaceSplitEdge>& vSplitEdges
    );
};

}  // namespace MeshPart

#endif
