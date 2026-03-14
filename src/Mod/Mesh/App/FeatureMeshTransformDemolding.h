#ifndef FEATURE_MESH_TRANSFORM_DEMOLDING_H
#define FEATURE_MESH_TRANSFORM_DEMOLDING_H

#include <App/PropertyLinks.h>

#include "FeatureMeshTransform.h"


namespace Mesh
{

/**
 * The TransformDemolding class.
 */
class TransformDemolding: public Transform
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::TransformDemolding);

public:
    TransformDemolding();

    App::PropertyLink Source;
    App::PropertyFloat Rotation;
    App::PropertyVector Axis;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "MeshGui::ViewProviderMeshTransformDemolding";
    }
    //@}
};

}  // namespace Mesh

#endif  // FEATURE_MESH_TRANSFORM_H
