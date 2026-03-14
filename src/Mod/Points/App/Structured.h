#ifndef POINTS_VIEW_FEATURE_H
#define POINTS_VIEW_FEATURE_H

#include "PointsFeature.h"


namespace Points
{

/*! For the Structured class it is expected that the Point property has Width*Height vertices
  and that with respect to their x,y coordinates they are ordered in a grid structure.
  If a point is marked invalid then one of its coordinates is set to NaN.
 */
class PointsExport Structured: public Feature
{
    PROPERTY_HEADER_WITH_OVERRIDE(Points::Structured);

public:
    /// Constructor
    Structured();

    App::PropertyInteger Width;  /**< The width of the structured cloud. */
    App::PropertyInteger Height; /**< The height of the structured cloud. */

    /** @name methods override Feature */
    //@{
    /// recalculate the Feature
    App::DocumentObjectExecReturn* execute() override;
    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "PointsGui::ViewProviderStructured";
    }
    //@}
};

using StructuredCustom = App::FeatureCustomT<Structured>;

}  // namespace Points


#endif
