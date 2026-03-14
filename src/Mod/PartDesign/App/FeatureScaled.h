#ifndef PARTDESIGN_FeatureScaled_H
#define PARTDESIGN_FeatureScaled_H

#include <App/PropertyStandard.h>
#include "FeatureTransformed.h"


namespace PartDesign
{

class PartDesignExport Scaled: public PartDesign::Transformed
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Scaled);

public:
    Scaled();

    App::PropertyFloat Factor;
    App::PropertyInteger Occurrences;

    /** @name methods override feature */
    //@{
    short mustExecute() const override;

    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderScaled";
    }
    //@}

    /** Create transformations
     * Returns a list containing (Occurrences-1) transformation since the first, untransformed
     * instance is not counted. Each transformation will scale the shape it is applied to by the
     * factor (Factor / (Occurrences - 1)) The centre point of the scaling (currently) is the centre
     * of gravity of the first original shape for this reason we need to pass the list of originals
     * into the feature
     */
    // Note: We can't just use the Originals property because this will fail if the Scaled feature
    // is being used inside a MultiTransform feature
    const std::list<gp_Trsf> getTransformations(
        const std::vector<App::DocumentObject*> originals
    ) override;
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FeatureScaled_H
