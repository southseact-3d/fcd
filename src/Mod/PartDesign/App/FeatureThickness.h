#ifndef PARTDESIGN_FEATURETHICKNESS_H
#define PARTDESIGN_FEATURETHICKNESS_H

#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include "FeatureDressUp.h"

namespace PartDesign
{

class PartDesignExport Thickness: public DressUp
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Thickness);

public:
    Thickness();

    App::PropertyLength Value;
    App::PropertyBool Reversed;
    App::PropertyBool Intersection;
    App::PropertyEnumeration Mode;
    App::PropertyEnumeration Join;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderThickness";
    }
    //@}
private:
    static const char* ModeEnums[];
    static const char* JoinEnums[];
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FEATURETHICKNESS_H
