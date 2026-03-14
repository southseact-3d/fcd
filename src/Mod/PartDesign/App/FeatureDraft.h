#ifndef PARTDESIGN_FEATUREDRAFT_H
#define PARTDESIGN_FEATUREDRAFT_H

#include <gp_Pln.hxx>
#include <gp_Dir.hxx>

#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include <App/PropertyLinks.h>
#include "FeatureDressUp.h"

namespace PartDesign
{
struct PartDesignExport DraftComputeProps
{
    gp_Dir pullDirection;
    gp_Pln neutralPlane;
};

class PartDesignExport Draft: public DressUp
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Draft);

public:
    Draft();

    App::PropertyAngle Angle;
    App::PropertyLinkSub NeutralPlane;
    App::PropertyLinkSub PullDirection;
    App::PropertyBool Reversed;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderDraft";
    }
    //@}

    /**
     * @brief getLastComputedProps: Returns the Pull Direction and Neutral Plane
     * computed during the last call of the execute method.
     * Note: The returned values might be in the default initialized state if
     * they were not computed or computation failed
     */
    DraftComputeProps getLastComputedProps() const
    {
        return computeProps;
    }

private:
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    static const App::PropertyAngle::Constraints floatAngle;

    DraftComputeProps computeProps;
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FEATUREDRAFT_H
