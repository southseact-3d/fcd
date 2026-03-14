#ifndef PART_FeatureGeometrySet_H
#define PART_FeatureGeometrySet_H

#include <Mod/Part/PartGlobal.h>

#include "PartFeature.h"
#include "PropertyGeometryList.h"


namespace Part
{

class PartExport FeatureGeometrySet: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::FeatureGeometrySet);

public:
    FeatureGeometrySet();

    PropertyGeometryList GeometrySet;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderGeometrySet";
    }
    //@}
};

}  // namespace Part


#endif  // PART_FeatureGeometrySet_H
