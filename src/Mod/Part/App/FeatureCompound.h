#ifndef PART_FEATURECOMPOUND_H
#define PART_FEATURECOMPOUND_H

#include <App/PropertyLinks.h>

#include <Mod/Part/PartGlobal.h>

#include "PartFeature.h"


namespace Part
{

class PartExport Compound: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Compound);

public:
    Compound();
    ~Compound() override;

    App::PropertyLinkList Links;

    /** @name methods override feature */
    //@{
    short mustExecute() const override;
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderCompound";
    }
    //@}
};

/// Same as Part::Compound, except it marks the Shape as transient, and rebuild it during restore
class Compound2: public Compound
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Compound2);

public:
    Compound2();
    void onDocumentRestored() override;
};

}  // namespace Part


#endif  // PART_FEATURECOMPOUND_H
