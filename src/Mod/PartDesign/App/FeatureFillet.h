#ifndef PARTDESIGN_FEATUREFILLET_H
#define PARTDESIGN_FEATUREFILLET_H

#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include "FeatureDressUp.h"

namespace PartDesign
{

class PartDesignExport Fillet: public DressUp
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Fillet);

public:
    Fillet();

    App::PropertyQuantityConstraint Radius;
    App::PropertyBool UseAllEdges;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderFillet";
    }
    //@}

protected:
    void Restore(Base::XMLReader& reader) override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FEATUREFILLET_H
