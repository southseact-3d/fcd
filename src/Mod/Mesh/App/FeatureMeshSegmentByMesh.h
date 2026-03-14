#ifndef FEATURE_MESH_SEGMENTBYMESH_H
#define FEATURE_MESH_SEGMENTBYMESH_H

#include <App/PropertyLinks.h>

#include "MeshFeature.h"


namespace Mesh
{

/**
 * The SegmentByMesh class creates a mesh segment from two meshes whereat
 * the facets of the first mesh that lie inside the second mesh build up the segment.
 * @author Werner Mayer
 */
class SegmentByMesh: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::SegmentByMesh);

public:
    SegmentByMesh();

    App::PropertyLink Source;
    App::PropertyLink Tool;
    App::PropertyVector Base;
    App::PropertyVector Normal;


    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}
};

}  // namespace Mesh

#endif  // FEATURE_MESH_SEGMENTBYMESH_H
