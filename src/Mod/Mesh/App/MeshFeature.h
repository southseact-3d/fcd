#ifndef MESH_FEATURE_H
#define MESH_FEATURE_H

#include <App/FeatureCustom.h>
#include <App/FeaturePython.h>
#include <App/GeoFeature.h>  // must be before FeatureCustom.h

#include "Core/MeshKernel.h"

#include "Mesh.h"
#include "MeshProperties.h"


namespace Base
{
class XMLReader;
class Writer;
}  // namespace Base

namespace MeshCore
{
class MeshKernel;
}

namespace Mesh
{

class MeshFeaturePy;

/** Base class of all mesh feature classes in FreeCAD.
 * This class holds a MeshKernel object.
 * \author Werner Mayer
 */
class MeshExport Feature: public App::GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Mesh::Feature);

public:
    /// Constructor
    Feature();

    /** @name Properties */
    //@{
    /** Property that holds the mesh data. */
    PropertyMeshKernel Mesh;
    //@}

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    void onChanged(const App::Property* prop) override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "MeshGui::ViewProviderMeshFaceSet";
    }
    const App::PropertyComplexGeoData* getPropertyOfGeometry() const override
    {
        return &Mesh;
    }

    /// handles the MeshPy object
    PyObject* getPyObject() override;
};

using FeatureCustom = App::FeatureCustomT<Feature>;
using FeaturePython = App::FeaturePythonT<Feature>;

}  // namespace Mesh


#endif
