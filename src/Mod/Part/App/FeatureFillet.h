#ifndef PART_FEATUREFILLET_H
#define PART_FEATUREFILLET_H

#include <Mod/Part/PartGlobal.h>

#include "PartFeature.h"


namespace Part
{

class PartExport Fillet: public Part::FilletBase
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Fillet);

public:
    Fillet();

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderFillet";
    }
    //@}
};

}  // namespace Part


#endif  // PART_FEATUREFILLET_H
