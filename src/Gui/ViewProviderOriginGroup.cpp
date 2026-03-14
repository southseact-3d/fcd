#include "ViewProviderOriginGroup.h"


using namespace Gui;


PROPERTY_SOURCE_WITH_EXTENSIONS(Gui::ViewProviderOriginGroup, Gui::ViewProviderDocumentObject)

ViewProviderOriginGroup::ViewProviderOriginGroup()
{
    initExtension(this);
}

ViewProviderOriginGroup::~ViewProviderOriginGroup() = default;
