#ifndef SURFACE_FEATURESECTIONS_H
#define SURFACE_FEATURESECTIONS_H

#include <App/PropertyLinks.h>
#include <Mod/Part/App/FeaturePartSpline.h>
#include <Mod/Surface/SurfaceGlobal.h>


namespace Surface
{

class SurfaceExport Sections: public Part::Spline
{
    PROPERTY_HEADER_WITH_OVERRIDE(Surface::Sections);

public:
    Sections();

    App::PropertyLinkSubList NSections;

    // recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "SurfaceGui::ViewProviderSections";
    }
};

}  // Namespace Surface

#endif
