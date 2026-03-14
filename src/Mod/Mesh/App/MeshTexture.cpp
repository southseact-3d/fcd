#include "MeshTexture.h"


using namespace Mesh;

MeshTexture::MeshTexture(const Mesh::MeshObject& mesh, const MeshCore::Material& material)
    : materialRefMesh(material)
    , countPointsRefMesh {mesh.countPoints()}
{
    unsigned long countFacets = mesh.countFacets();

    if (material.binding == MeshCore::MeshIO::PER_VERTEX
        && material.diffuseColor.size() == countPointsRefMesh) {
        binding = MeshCore::MeshIO::PER_VERTEX;
        kdTree = std::make_unique<MeshCore::MeshKDTree>(mesh.getKernel().GetPoints());
    }
    else if (material.binding == MeshCore::MeshIO::PER_FACE
             && material.diffuseColor.size() == countFacets) {
        binding = MeshCore::MeshIO::PER_FACE;
        kdTree = std::make_unique<MeshCore::MeshKDTree>(mesh.getKernel().GetPoints());
        refPnt2Fac = std::make_unique<MeshCore::MeshRefPointToFacets>(mesh.getKernel());
    }
}

void MeshTexture::apply(
    const Mesh::MeshObject& mesh,
    const Base::Color& defaultColor,
    MeshCore::Material& material
)
{
    apply(mesh, true, defaultColor, -1.0F, material);
}

void MeshTexture::apply(
    const Mesh::MeshObject& mesh,
    const Base::Color& defaultColor,
    float max_dist,
    MeshCore::Material& material
)
{
    apply(mesh, true, defaultColor, max_dist, material);
}

void MeshTexture::apply(const Mesh::MeshObject& mesh, MeshCore::Material& material)
{
    Base::Color defaultColor;
    apply(mesh, false, defaultColor, -1.0F, material);
}

void MeshTexture::apply(const Mesh::MeshObject& mesh, float max_dist, MeshCore::Material& material)
{
    Base::Color defaultColor;
    apply(mesh, false, defaultColor, max_dist, material);
}

void MeshTexture::apply(
    const Mesh::MeshObject& mesh,
    bool addDefaultColor,
    const Base::Color& defaultColor,
    float max_dist,
    MeshCore::Material& material
)
{
    // copy the color values because the passed material could be the same instance as
    // 'materialRefMesh'
    std::vector<Base::Color> textureColor = materialRefMesh.diffuseColor;
    material.diffuseColor.clear();
    material.binding = MeshCore::MeshIO::OVERALL;

    if (kdTree) {
        // the points of the current mesh
        std::vector<Base::Color> diffuseColor;
        const MeshCore::MeshPointArray& points = mesh.getKernel().GetPoints();
        const MeshCore::MeshFacetArray& facets = mesh.getKernel().GetFacets();

        if (binding == MeshCore::MeshIO::PER_VERTEX) {
            diffuseColor.reserve(points.size());
            for (size_t index = 0; index < points.size(); index++) {
                PointIndex pos = findIndex(points[index], max_dist);
                if (pos < countPointsRefMesh) {
                    diffuseColor.push_back(textureColor[pos]);
                }
                else if (addDefaultColor) {
                    diffuseColor.push_back(defaultColor);
                }
            }

            if (diffuseColor.size() == points.size()) {
                material.diffuseColor.swap(diffuseColor);
                material.binding = MeshCore::MeshIO::PER_VERTEX;
            }
        }
        else if (binding == MeshCore::MeshIO::PER_FACE) {
            // the values of the map give the point indices of the original mesh
            std::vector<PointIndex> pointMap;
            pointMap.reserve(points.size());
            for (size_t index = 0; index < points.size(); index++) {
                PointIndex pos = findIndex(points[index], max_dist);
                if (pos < countPointsRefMesh) {
                    pointMap.push_back(pos);
                }
                else if (addDefaultColor) {
                    pointMap.push_back(MeshCore::POINT_INDEX_MAX);
                }
            }

            // now determine the facet indices of the original mesh
            if (pointMap.size() == points.size()) {
                diffuseColor.reserve(facets.size());
                for (const auto& it : facets) {
                    PointIndex index1 = pointMap[it._aulPoints[0]];
                    PointIndex index2 = pointMap[it._aulPoints[1]];
                    PointIndex index3 = pointMap[it._aulPoints[2]];
                    if (index1 != MeshCore::POINT_INDEX_MAX && index2 != MeshCore::POINT_INDEX_MAX
                        && index3 != MeshCore::POINT_INDEX_MAX) {
                        std::vector<FacetIndex> found = refPnt2Fac->GetIndices(index1, index2, index3);
                        if (found.size() == 1) {
                            diffuseColor.push_back(textureColor[found.front()]);
                        }
                    }
                    else if (addDefaultColor) {
                        diffuseColor.push_back(defaultColor);
                    }
                }
            }

            if (diffuseColor.size() == facets.size()) {
                material.diffuseColor.swap(diffuseColor);
                material.binding = MeshCore::MeshIO::PER_FACE;
            }
        }
    }
}
