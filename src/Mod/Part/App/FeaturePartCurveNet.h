#ifndef PART_FeaturePartCurveNet_H
#define PART_FeaturePartCurveNet_H

#include "PartFeature.h"


namespace Part
{

class CurveNet: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::CurveNet);

public:
    CurveNet();

    App::PropertyString FileName;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderCurveNet";
    }
    //@}
};

}  // namespace Part


#endif  // PART_FeaturePartCurveNet_H
