#ifndef FEATURE_MESH_TRANSFORM_H
#define FEATURE_MESH_TRANSFORM_H

#include <App/PropertyLinks.h>

#include "MeshFeature.h"


namespace Mesh
{

/**
 * The Import class reads the any supported mesh format
 * into the FreeCAD workspace.
 * @author Werner Mayer
 */
class Transform: public Mesh::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Transform);

public:
    Transform();

    App::PropertyLink Source;
    App::PropertyMatrix Position;


    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "MeshGui::ViewProviderMeshTransform";
    }
    //@}
};

}  // namespace Mesh

#endif  // FEATURE_MESH_TRANSFORM_H
