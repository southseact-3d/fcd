#ifndef PATH_FeaturePathShape_H
#define PATH_FeaturePathShape_H

#include <App/DocumentObject.h>
#include <App/FeaturePython.h>
#include <App/PropertyGeo.h>

#include "FeatureArea.h"
#include "FeaturePath.h"


namespace Path
{

class PathExport FeatureShape: public Path::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Path::FeatureShape);

public:
    /// Constructor
    FeatureShape();
    ~FeatureShape() override;

    // Part::PropertyPartShape Shape;
    App::PropertyLinkList Sources;
    App::PropertyVector StartPoint;
    App::PropertyBool UseStartPoint;
    PARAM_PROP_DECLARE(AREA_PARAMS_PATH)

    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    //@}

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PathGui::ViewProviderPathShape";
    }

protected:
    /// get called by the container when a property has changed
    // void onChanged (const App::Property* prop) override;
};

using FeatureShapePython = App::FeaturePythonT<FeatureShape>;

}  // namespace Path


#endif  // PATH_FeaturePathShape_H
