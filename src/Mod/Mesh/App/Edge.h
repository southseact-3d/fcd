#ifndef MESH_EDGE_H
#define MESH_EDGE_H

#include <Base/Handle.h>

#include <Mod/Mesh/App/Core/Elements.h>


namespace Mesh
{
// forward declaration
class MeshObject;

/** The Edge helper class
 * The Edge class provides an interface for the EdgePy class for
 * convenient access to the Mesh data structure. This class should not be used
 * for programming algorithms in C++. Use Mesh Core classes instead!
 */
class MeshExport Edge: public MeshCore::MeshGeomEdge
{
public:
    Edge();
    Edge(const Edge& f);
    Edge(Edge&& f);
    ~Edge();

    bool isBound() const
    {
        return Index != -1;
    }
    void unbound();
    Edge& operator=(const Edge& e);
    Edge& operator=(Edge&& e);

    int Index {-1};
    MeshCore::PointIndex PIndex[2];
    MeshCore::FacetIndex NIndex[2];
    Base::Reference<const MeshObject> Mesh;
};

}  // namespace Mesh


#endif  // MESH_EDGE_H
