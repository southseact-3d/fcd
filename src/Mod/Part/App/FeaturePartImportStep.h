#ifndef PART_FEATUREPARTIMPORTSTEP_H
#define PART_FEATUREPARTIMPORTSTEP_H

#include <App/PropertyStandard.h>

#include "PartFeature.h"


namespace Part
{

class ImportStep: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::ImportStep);

public:
    ImportStep();

    App::PropertyString FileName;

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderImport";
    }
    //@}
};

}  // namespace Part


#endif  // PART_FEATUREPARTIMPORTSTEP_H
