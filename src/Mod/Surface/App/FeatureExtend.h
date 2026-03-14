#ifndef SURFACE_FEATUREEXTEND_H
#define SURFACE_FEATUREEXTEND_H

#include <App/PropertyLinks.h>
#include <Mod/Part/App/FeaturePartSpline.h>
#include <Mod/Surface/SurfaceGlobal.h>


namespace Surface
{

class SurfaceExport Extend: public Part::Spline
{
    PROPERTY_HEADER_WITH_OVERRIDE(Surface::Extend);

public:
    Extend();

    App::PropertyLinkSub Face;
    App::PropertyFloatConstraint Tolerance;
    App::PropertyFloatConstraint ExtendUNeg;
    App::PropertyFloatConstraint ExtendUPos;
    App::PropertyBool ExtendUSymetric;
    App::PropertyFloatConstraint ExtendVNeg;
    App::PropertyFloatConstraint ExtendVPos;
    App::PropertyBool ExtendVSymetric;
    App::PropertyIntegerConstraint SampleU;
    App::PropertyIntegerConstraint SampleV;

    // recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "SurfaceGui::ViewProviderExtend";
    }

protected:
    void onChanged(const App::Property* prop) override;
    void handleChangedPropertyName(
        Base::XMLReader& reader,
        const char* TypeName,
        const char* PropName
    ) override;

private:
    bool lockOnChangeMutex {false};
};

}  // Namespace Surface

#endif
