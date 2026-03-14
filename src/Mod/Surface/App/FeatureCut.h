#ifndef SURFACE_FEATURECUT_H
#define SURFACE_FEATURECUT_H

#include <App/PropertyLinks.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Surface/SurfaceGlobal.h>


namespace Surface
{

class SurfaceExport Cut: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Surface::Cut);

public:
    Cut();

    App::PropertyLinkSubList ShapeList;  // Shapes to be cut.

    // recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the view provider
    //    const char* getViewProviderName(void) const {
    //        return "SurfaceGui::ViewProviderCut";
    //    }
};

}  // Namespace Surface

#endif
