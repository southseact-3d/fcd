#ifndef PART_FEATUREPARTIMPORTIGES_H
#define PART_FEATUREPARTIMPORTIGES_H

#include "PartFeature.h"


namespace Part
{

class ImportIges: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::ImportIges);

public:
    ImportIges();

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


#endif  // PART_FEATUREPARTIMPORTIGES_H
