#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>

#include "ViewProviderViewGroup.h"


using namespace AssemblyGui;

PROPERTY_SOURCE(AssemblyGui::ViewProviderViewGroup, Gui::ViewProviderDocumentObjectGroup)

ViewProviderViewGroup::ViewProviderViewGroup()
{}

ViewProviderViewGroup::~ViewProviderViewGroup() = default;

QIcon ViewProviderViewGroup::getIcon() const
{
    return Gui::BitmapFactory().pixmap("Assembly_ExplodedViewGroup.svg");
}
