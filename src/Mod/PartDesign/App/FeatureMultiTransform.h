#ifndef PARTDESIGN_FeatureMultiTransform_H
#define PARTDESIGN_FeatureMultiTransform_H

#include "FeatureTransformed.h"


namespace PartDesign
{

class PartDesignExport MultiTransform: public PartDesign::Transformed
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::MultiTransform);

public:
    MultiTransform();

    App::PropertyLinkList Transformations;

    /** @name methods override feature */
    //@{
    short mustExecute() const override;

    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderMultiTransform";
    }
    //@}

    /** Create transformations
     * Returns a list containing the product of all transformations of the subfeatures given
     * by the Transformations property. Subfeatures can be Mirrored, LinearPattern, PolarPattern and
     * Scaled.
     */
    const std::list<gp_Trsf> getTransformations(
        const std::vector<App::DocumentObject*> originals
    ) override;

protected:
    void positionBySupport() override;
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FeatureMultiTransform_H
