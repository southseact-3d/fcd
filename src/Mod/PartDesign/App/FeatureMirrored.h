#ifndef PARTDESIGN_FeatureMirrored_H
#define PARTDESIGN_FeatureMirrored_H

#include "FeatureTransformed.h"


namespace PartDesign
{

class PartDesignExport Mirrored: public PartDesign::Transformed
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Mirrored);

public:
    Mirrored();

    App::PropertyLinkSub MirrorPlane;

    /** @name methods override feature */
    //@{
    short mustExecute() const override;

    /// returns the type name of the view provider
    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderMirrored";
    }
    //@}

    /** Create transformations
     * Returns a list containing one transformation since the first, untransformed instance
     * is not counted. The transformation will mirror the shape it is applied to on a plane
     * If MirrorPlane contains a feature and a face name, then the mirror plane will be
     * the given face, which must be planar
     */
    const std::list<gp_Trsf> getTransformations(const std::vector<App::DocumentObject*>) override;

private:
    std::list<gp_Trsf> createTransformations(gp_Pnt& axbase, gp_Dir& axdir) const;
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FeatureMirrored_H
