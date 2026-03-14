#ifndef PARTDESIGN_FeatureRefine_H
#define PARTDESIGN_FeatureRefine_H

#include "Feature.h"

/// Base class of all features that can be refined PartDesign
namespace PartDesign
{

class PartDesignExport FeatureRefine: public PartDesign::Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::FeatureRefine);

public:
    enum class RefineErrorPolicy
    {
        Raise = 0,
        Warn
    };

    FeatureRefine();

    App::PropertyBool Refine;

protected:
    // store the shape before refinement
    TopoShape rawShape;


    /**
     * Check if the feature *only* requires the refinement operation, and do that refinement if so.
     * Typically called as the first operation in a subclass's `execute()` method to provide an
     * early exit if no other parameters have been changed (so the base feature is still
     * up-to-date).
     *
     * @return true if the refine was done and that was the only thing needed, or false if further
     * computation is necessary.
     */
    bool onlyHaveRefined();
    TopoShape refineShapeIfActive(
        const TopoShape& oldShape,
        const RefineErrorPolicy onError = RefineErrorPolicy::Warn
    ) const;
};

using FeatureRefinePython = App::FeaturePythonT<FeatureRefine>;

}  // namespace PartDesign


#endif  // PARTDESIGN_FeatureRefine_H
