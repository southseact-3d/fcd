#ifndef PARTDESIGN_Pad_H
#define PARTDESIGN_Pad_H

#include "FeatureExtrude.h"

namespace PartDesign
{

class PartDesignExport Pad: public FeatureExtrude
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Pad);

public:
    Pad();

    /** @name methods override feature */
    //@{
    /** Recalculate the feature
     * Extrudes the Sketch in the direction of the sketch face normal
     * If Type is "Length" then Length gives the extrusion length, the direction will be away from
     * the support If Type is "UpToLast" then the extrusion will stop at the last face of the
     * support that is cut by a line through the centre of gravite of the sketch If Type is
     * "UpToFirst" then extrusion will stop at the first face of the support If Type is "UpToFace"
     * then the extrusion will stop at FaceName in the support If Midplane is true, then the
     * extrusion will extend for half of the length on both sides of the sketch plane If Reversed is
     * true then the direction of revolution will be reversed. The created material will be fused
     * with the sketch support (if there is one)
     */
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderPad";
    }
    //@}

private:
    static const char* TypeEnums[];
};

}  // namespace PartDesign


#endif  // PART_Pad_H
