#ifndef FEATURE_MESH_SETOPERATIONS_H
#define FEATURE_MESH_SETOPERATIONS_H

#include <App/PropertyLinks.h>

#include "MeshFeature.h"


namespace Mesh
{

/**
 * Set Operations on meshes
 * Operations so far: Intersections, Union, Difference
 * @author Berthold Grupp
 */
class SetOperations: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::SetOperations);

public:
    SetOperations();

    App::PropertyLink Source1;
    App::PropertyLink Source2;
    App::PropertyString OperationType;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    //@}
};

}  // namespace Mesh

#endif  // FEATURE_MESH_SEGMENT_H
