#ifndef MESH_FACET_H
#define MESH_FACET_H

#include "Edge.h"


namespace Mesh
{
// forward declaration
class MeshObject;

/** The Facet helper class
 * The MeshFacet class provides an interface for the MeshFacetPy class for
 * convenient access to the Mesh data structure. This class should not be used
 * for programming algorithms in C++. Use Mesh Core classes instead!
 */
class MeshExport Facet: public MeshCore::MeshGeomFacet
{
public:
    explicit Facet(
        const MeshCore::MeshFacet& face = MeshCore::MeshFacet(),
        const MeshObject* obj = nullptr,
        MeshCore::FacetIndex index = MeshCore::FACET_INDEX_MAX
    );
    Facet(const Facet& f);
    Facet(Facet&& f);
    ~Facet();

    bool isBound() const
    {
        return Index != MeshCore::FACET_INDEX_MAX;
    }
    Facet& operator=(const Facet& f);
    Facet& operator=(Facet&& f);
    Edge getEdge(int) const;

    MeshCore::FacetIndex Index;
    MeshCore::PointIndex PIndex[3];
    MeshCore::FacetIndex NIndex[3];
    Base::Reference<const MeshObject> Mesh;
};

}  // namespace Mesh


#endif  // MESH_FACET_H
