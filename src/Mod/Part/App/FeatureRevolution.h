#ifndef PART_FEATUREREVOLUTION_H
#define PART_FEATUREREVOLUTION_H

#include <App/PropertyStandard.h>
#include <Base/Vector3D.h>

#include <Mod/Part/PartGlobal.h>

#include "PartFeature.h"


namespace Part
{

class PartExport Revolution: public Part::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Part::Revolution);

public:
    Revolution();

    App::PropertyLink Source;
    App::PropertyVector Base;
    App::PropertyVector Axis;
    App::PropertyLinkSub AxisLink;
    App::PropertyFloatConstraint Angle;
    App::PropertyBool Symmetric;  // like "Midplane" in PartDesign
    App::PropertyBool Solid;
    App::PropertyString FaceMakerClass;

    /** @name methods override feature */
    //@{
    /// recalculate the feature
    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;

    void onChanged(const App::Property* prop) override;

    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartGui::ViewProviderRevolution";
    }
    //@}

    /**
     * @brief fetchAxisLink: read AxisLink to obtain the axis parameters and
     * angle span. Note: this routine is reused in Revolve dialog, hence it
     * is static.
     * @param axisLink (input): the link
     * @param center (output): base point of axis
     * @param dir (output): direction of axis
     * @param angle (output): if edge is an arc of circle, this argument is
     * used to return the angle span of the arc.
     * @return true if link was fetched. false if link was empty. Throws if the
     * link is wrong.
     */
    static bool fetchAxisLink(
        const App::PropertyLinkSub& axisLink,
        Base::Vector3d& center,
        Base::Vector3d& dir,
        double& angle
    );

private:
    static App::PropertyFloatConstraint::Constraints angleRangeU;

protected:
    void setupObject() override;
};

}  // namespace Part


#endif  // PART_FEATUREREVOLUTION_H
