#ifndef MESHTRIM_BY_PLANE_H
#define MESHTRIM_BY_PLANE_H

#include "MeshKernel.h"


namespace MeshCore
{

/**
 * Trim the facets in 3D with a plane
 * \author Werner Mayer
 */
class MeshExport MeshTrimByPlane
{
public:
    explicit MeshTrimByPlane(MeshKernel& mesh);

public:
    /**
     * Checks all facets for intersection with the plane and writes all touched facets into the
     * vector
     */
    void CheckFacets(
        const MeshFacetGrid& rclGrid,
        const Base::Vector3f& base,
        const Base::Vector3f& normal,
        std::vector<FacetIndex>& trimFacets,
        std::vector<FacetIndex>& removeFacets
    ) const;

    /**
     * The facets from \a trimFacets will be trimmed or deleted and \a trimmedFacets holds the newly
     * generated facets
     */
    void TrimFacets(
        const std::vector<FacetIndex>& trimFacets,
        const Base::Vector3f& base,
        const Base::Vector3f& normal,
        std::vector<MeshGeomFacet>& trimmedFacets
    );

private:
    void CreateOneFacet(
        const Base::Vector3f& base,
        const Base::Vector3f& normal,
        unsigned short shift,
        const MeshGeomFacet& facet,
        std::vector<MeshGeomFacet>& trimmedFacets
    ) const;
    void CreateTwoFacet(
        const Base::Vector3f& base,
        const Base::Vector3f& normal,
        unsigned short shift,
        const MeshGeomFacet& facet,
        std::vector<MeshGeomFacet>& trimmedFacets
    ) const;

private:
    MeshKernel& myMesh;
};

}  // namespace MeshCore

#endif  // MESHTRIM_BY_PLANE_H
