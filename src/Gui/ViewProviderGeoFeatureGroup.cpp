#include "ViewProviderGeoFeatureGroup.h"


using namespace Gui;


PROPERTY_SOURCE_WITH_EXTENSIONS(Gui::ViewProviderGeoFeatureGroup, Gui::ViewProviderDocumentObject)

ViewProviderGeoFeatureGroup::ViewProviderGeoFeatureGroup()
{
    ViewProviderGeoFeatureGroupExtension::initExtension(this);
}

ViewProviderGeoFeatureGroup::~ViewProviderGeoFeatureGroup() = default;


// Python feature -----------------------------------------------------------------------

namespace Gui
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(Gui::ViewProviderGeoFeatureGroupPython, Gui::ViewProviderGeoFeatureGroup)
/// @endcond

// explicit template instantiation
template class GuiExport ViewProviderFeaturePythonT<ViewProviderGeoFeatureGroup>;
}  // namespace Gui
