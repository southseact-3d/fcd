#ifndef PART_FEATUREPARTSPLINE_H
#define PART_FEATUREPARTSPLINE_H

#include <Mod/Part/PartGlobal.h>

#include "PartFeature.h"


namespace Part
{

class PartExport Spline: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Spline);

public:
    Spline();
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderSpline";
    }
};

}  // namespace Part


#endif  // PART_FEATUREPARTSPLINE_H
