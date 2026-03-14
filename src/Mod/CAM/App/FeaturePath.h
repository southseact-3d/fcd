#ifndef PATH_FeaturePath_H
#define PATH_FeaturePath_H

#include <App/DocumentObject.h>
#include <App/GeoFeature.h>
#include <App/FeaturePython.h>

#include "PropertyPath.h"


namespace Path
{

class PathExport Feature: public App::GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Path::Feature);

public:
    /// Constructor
    Feature();
    ~Feature() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PathGui::ViewProviderPath";
    }
    App::DocumentObjectExecReturn* execute() override
    {
        return App::DocumentObject::StdReturn;
    }
    short mustExecute() const override;
    PyObject* getPyObject() override;

    PropertyPath Path;


protected:
    /// get called by the container when a property has changed
    void onChanged(const App::Property* prop) override;
};

using FeaturePython = App::FeaturePythonT<Feature>;

}  // namespace Path


#endif  // PATH_FeaturePath_H
