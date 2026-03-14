#ifndef PARTDESIGN_Pocket_H
#define PARTDESIGN_Pocket_H

#include "FeatureExtrude.h"

namespace PartDesign
{

class PartDesignExport Pocket: public FeatureExtrude
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Pocket);

public:
    Pocket();

    /** @name methods override feature */
    //@{
    /** Recalculate the feature
     * Extrudes the Sketch in the direction of the sketch face normal
     * If Type is "Length" then Length gives the extrusion length, the direction will be into the
     * support If Type is "ThroughAll" then the extrusion length will be infinite If Type is
     * "UpToFirst" then extrusion will stop at the first face of the support that is cut by a line
     * through the centre of gravite of the sketch If Type is "UpToFace" then the extrusion will
     * stop at FaceName in the support If Midplane is true, then the extrusion will extend for half
     * of the length on both sides of the sketch plane The created material will be cut out of the
     * sketch support
     */
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderPocket";
    }

    Base::Vector3d getProfileNormal() const override;

private:
    static const char* TypeEnums[];
};

}  // namespace PartDesign


#endif  // PART_Pocket_H
