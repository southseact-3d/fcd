#ifndef FeatureMeshCurvature_H
#define FeatureMeshCurvature_H

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>

#include "MeshProperties.h"


namespace Mesh
{

/**
 * The Curvature class calculates the curvature values of a related mesh feature.
 * @author Werner Mayer
 */
class MeshExport Curvature: public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Curvature);

public:
    Curvature();

    App::PropertyLink Source;
    PropertyCurvatureList CurvInfo;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "MeshGui::ViewProviderMeshCurvature";
    }
    //@}
};

}  // namespace Mesh

#endif  // Curvature_H
