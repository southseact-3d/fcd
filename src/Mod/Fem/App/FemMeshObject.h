#ifndef Fem_FemMeshObject_H
#define Fem_FemMeshObject_H

#include <App/FeaturePython.h>
#include <App/GeoFeature.h>
#include <App/SuppressibleExtension.h>

#include "FemMesh.h"
#include "FemMeshProperty.h"


namespace Fem
{

class FemExport FemMeshObject: public App::GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::FemMeshObject);

public:
    /// Constructor
    FemMeshObject();
    ~FemMeshObject() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemMesh";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;
    const App::PropertyComplexGeoData* getPropertyOfGeometry() const override
    {
        return &FemMesh;
    }

    PropertyFemMesh FemMesh;

protected:
    /// get called by the container when a property has changed
    void onChanged(const App::Property* prop) override;

private:
    App::SuppressibleExtension suppressibleExt;
};

using FemMeshObjectPython = App::FeaturePythonT<FemMeshObject>;


}  // namespace Fem


#endif  // Fem_FemMeshObject_H
