#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>

#include "ViewProviderBomGroup.h"


using namespace AssemblyGui;

PROPERTY_SOURCE(AssemblyGui::ViewProviderBomGroup, Gui::ViewProviderDocumentObjectGroup)

ViewProviderBomGroup::ViewProviderBomGroup()
{}

ViewProviderBomGroup::~ViewProviderBomGroup() = default;

QIcon ViewProviderBomGroup::getIcon() const
{
    return Gui::BitmapFactory().pixmap("Assembly_BillOfMaterialsGroup.svg");
}
