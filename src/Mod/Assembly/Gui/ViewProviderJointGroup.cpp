#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>

#include "ViewProviderJointGroup.h"


using namespace AssemblyGui;

PROPERTY_SOURCE(AssemblyGui::ViewProviderJointGroup, Gui::ViewProviderDocumentObjectGroup)

ViewProviderJointGroup::ViewProviderJointGroup()
{}

ViewProviderJointGroup::~ViewProviderJointGroup() = default;

QIcon ViewProviderJointGroup::getIcon() const
{
    return Gui::BitmapFactory().pixmap("Assembly_JointGroup.svg");
}
