#include <Base/Placement.h>

#include "Placement.h"

using namespace App;


PROPERTY_SOURCE(App::Placement, App::GeoFeature)


//===========================================================================
// Feature
//===========================================================================

Placement::Placement() = default;

Placement::~Placement() = default;


// Python feature ---------------------------------------------------------
namespace App
{
PROPERTY_SOURCE_TEMPLATE(App::PlacementPython, App::Placement)
template<>
const char* App::PlacementPython::getViewProviderName() const
{
    return "Gui::ViewProviderPlacementPython";
}
template class AppExport FeaturePythonT<App::Placement>;
}  // namespace App
