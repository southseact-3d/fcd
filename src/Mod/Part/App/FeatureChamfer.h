#ifndef PART_FEATURECHAMFER_H
#define PART_FEATURECHAMFER_H

#include "PartFeature.h"

#include <Mod/Part/PartGlobal.h>

namespace Part
{

class PartExport Chamfer: public Part::FilletBase
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Chamfer);

public:
    Chamfer();

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderChamfer";
    }
    //@}
};

}  // namespace Part


#endif  // PART_FEATURECHAMFER_H
