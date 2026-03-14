#ifndef MESH_MESHTEXTURE_H
#define MESH_MESHTEXTURE_H

#include <memory>

#include "Core/Algorithm.h"
#include "Core/KDTree.h"

#include "Mesh.h"


namespace Mesh
{

/*! The MeshTexture class.
  This algorithm is useful to update the material after a mesh has been modified
  by removing points or facets. If the coordinates of points have changed or if
  new points have been added then a predefined color will be set.
  @author Werner Mayer
 */
class MeshExport MeshTexture
{
public:
    /*!
      A mesh with material. The number of points or facets must match with the number of colors.
     */
    MeshTexture(const Mesh::MeshObject& mesh, const MeshCore::Material& material);
    /*!
     Find common points or facets of this to the original mesh. For points or facets
     that don't match \a defaultColor will be used instead, otherwise the color of the
     original material is used.
     */
    void apply(
        const Mesh::MeshObject& mesh,
        const Base::Color& defaultColor,
        MeshCore::Material& material
    );
    /*!
     Find common points or facets of this to the original mesh. For points or facets
     that don't match \a defaultColor will be used instead, otherwise the color of the
     original material is used.
     */
    void apply(
        const Mesh::MeshObject& mesh,
        const Base::Color& defaultColor,
        float max_dist,
        MeshCore::Material& material
    );
    /*!
     Find common points or facets of this to the original mesh and use the color of the original
     material. If for a point of \a mesh no matching point of the original mesh can be found the
     texture mapping will fail.
     */
    void apply(const Mesh::MeshObject& mesh, MeshCore::Material& material);
    /*!
     Find common points or facets of this to the original mesh and use the color of the original
     material. If for a point of \a mesh no matching point of the original mesh can be found the
     texture mapping will fail.
     */
    void apply(const Mesh::MeshObject& mesh, float max_dist, MeshCore::Material& material);

private:
    void apply(
        const Mesh::MeshObject& mesh,
        bool addDefaultColor,
        const Base::Color& defaultColor,
        float max_dist,
        MeshCore::Material& material
    );
    PointIndex findIndex(const Base::Vector3f& p, float max_dist) const
    {
        if (max_dist < 0.0F) {
            return kdTree->FindExact(p);
        }

        Base::Vector3f n;
        float dist {};
        return kdTree->FindNearest(p, max_dist, n, dist);
    }

private:
    const MeshCore::Material& materialRefMesh;
    unsigned long countPointsRefMesh;
    std::unique_ptr<MeshCore::MeshKDTree> kdTree;
    std::unique_ptr<MeshCore::MeshRefPointToFacets> refPnt2Fac;
    MeshCore::MeshIO::Binding binding = MeshCore::MeshIO::OVERALL;
};

}  // namespace Mesh

#endif  // MESH_MESHTEXTURE_H
