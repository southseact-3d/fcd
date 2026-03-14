#ifndef PART_FEATUREPARTIMPORTBREP_H
#define PART_FEATUREPARTIMPORTBREP_H

#include "PartFeature.h"


namespace Part
{

class ImportBrep: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::ImportBrep);

public:
    ImportBrep();

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


#endif  // PART_FEATUREPARTIMPORTBREP_H
