#ifndef _AppPlacement_h_
#define _AppPlacement_h_

#include "FeaturePython.h"

namespace App
{

/** Placement Object
 *  Handles the repositioning of data. Also can do grouping
 */
class AppExport Placement: public App::GeoFeature
{
    PROPERTY_HEADER_WITH_OVERRIDE(App::Placement);

public:
    /// Constructor
    Placement();
    ~Placement() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "Gui::ViewProviderPlacement";
    }
};
using PlacementPython = App::FeaturePythonT<App::Placement>;


}  // namespace App


#endif
