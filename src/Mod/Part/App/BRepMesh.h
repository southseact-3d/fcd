#ifndef PART_BREPMESH_H
#define PART_BREPMESH_H

#include <Mod/Part/PartGlobal.h>
#include <App/ComplexGeoData.h>

namespace Part
{

class PartExport BRepMesh
{
public:
    using Facet = Data::ComplexGeoData::Facet;
    using Domain = Data::ComplexGeoData::Domain;
    using Segment = std::vector<std::size_t>;

    void getFacesFromDomains(
        const std::vector<Domain>& domains,
        std::vector<Base::Vector3d>& points,
        std::vector<Facet>& faces
    );
    std::vector<Segment> createSegments() const;

private:
    std::vector<std::size_t> domainSizes;
};

}  // namespace Part

#endif  // PART_BREPMESH_H
