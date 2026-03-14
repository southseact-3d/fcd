#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>

#include "ViewProviderSimulationGroup.h"


using namespace AssemblyGui;

PROPERTY_SOURCE(AssemblyGui::ViewProviderSimulationGroup, Gui::ViewProviderDocumentObjectGroup)

ViewProviderSimulationGroup::ViewProviderSimulationGroup()
{}

ViewProviderSimulationGroup::~ViewProviderSimulationGroup() = default;

QIcon ViewProviderSimulationGroup::getIcon() const
{
    return Gui::BitmapFactory().pixmap("Assembly_SimulationGroup.svg");
}
