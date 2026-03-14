#include <App/DocumentObject.h>

#include "ViewProviderTemplateExtension.h"
#include "ViewProviderTemplate.h"


using namespace TechDrawGui;

EXTENSION_PROPERTY_SOURCE(TechDrawGui::ViewProviderTemplateExtension, Gui::ViewProviderExtension)

ViewProviderTemplateExtension::ViewProviderTemplateExtension()
{
    initExtensionType(ViewProviderTemplateExtension::getExtensionClassTypeId());
}

ViewProviderTemplateExtension::~ViewProviderTemplateExtension() {}

//there are no child objects to drag currently, so we will say we handle any dragging rather than letting some
//other extension trying to drag and causing problems.
bool ViewProviderTemplateExtension::extensionCanDragObjects() const { return true; }

//there are no child objects to drag currently, so we will say we handle any dragging
bool ViewProviderTemplateExtension::extensionCanDragObject(App::DocumentObject* docObj) const
{
    (void)docObj;
    return true;
}

//templates do not accept drops, so rather that let some other extension try to drop into a template, we will
//claim that we can handle drops
bool ViewProviderTemplateExtension::extensionCanDropObjects() const { return true; }

//templates do not accept drops, so rather that let some other extension try to drop into a template, we will
//claim that we can handle drops
bool ViewProviderTemplateExtension::extensionCanDropObject(App::DocumentObject* docObj) const
{
    (void)docObj;
    return true;
}

const ViewProviderTemplate* ViewProviderTemplateExtension::getViewProviderTemplate() const
{
    return dynamic_cast<const ViewProviderTemplate*>(getExtendedViewProvider());
}

const char* ViewProviderTemplateExtension::whoAmI() const
{
    auto parent = getViewProviderTemplate();
    if (parent) {
        return parent->whoAmI();
    }
    return nullptr;
}

namespace Gui
{
EXTENSION_PROPERTY_SOURCE_TEMPLATE(TechDrawGui::ViewProviderTemplateExtensionPython,
                                   TechDrawGui::ViewProviderTemplateExtension)

// explicit template instantiation
template class TechDrawGuiExport
    ViewProviderExtensionPythonT<TechDrawGui::ViewProviderTemplateExtension>;
}// namespace Gui
