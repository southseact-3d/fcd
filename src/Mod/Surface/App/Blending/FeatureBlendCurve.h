#ifndef FEATURE_BLEND_CURVE_H
#define FEATURE_BLEND_CURVE_H

#include <App/PropertyLinks.h>
#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include <Mod/Part/App/FeaturePartSpline.h>
#include <Mod/Surface/App/Blending/BlendPoint.h>
#include <Mod/Surface/SurfaceGlobal.h>

namespace Surface
{

class SurfaceExport FeatureBlendCurve: public Part::Spline
{
    PROPERTY_HEADER_WITH_OVERRIDE(Surface::FeatureBlendCurve);

public:
    FeatureBlendCurve();

    App::PropertyLinkSub StartEdge;
    App::PropertyFloatConstraint StartParameter;
    App::PropertyIntegerConstraint StartContinuity;
    App::PropertyFloatConstraint StartSize;

    App::PropertyLinkSub EndEdge;
    App::PropertyFloatConstraint EndParameter;
    App::PropertyIntegerConstraint EndContinuity;
    App::PropertyFloatConstraint EndSize;

    Standard_Integer maxDegree;

    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    const char* getViewProviderName() const override
    {
        return "SurfaceGui::ViewProviderBlendCurve";
    }

private:
    BlendPoint GetBlendPoint(
        App::PropertyLinkSub& link,
        App::PropertyFloatConstraint& param,
        App::PropertyIntegerConstraint& Continuity
    );
    double RelativeToRealParameters(double, double, double);

protected:
    void onChanged(const App::Property* prop) override;
};

}  // Namespace Surface

#endif
