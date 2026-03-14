#include <App/Document.h>

#include "ViewProviderDrawingViewExtension.h"
#include "ViewProviderDrawingView.h"
#include "ViewProviderPage.h"


using namespace TechDrawGui;

EXTENSION_PROPERTY_SOURCE(TechDrawGui::ViewProviderDrawingViewExtension, Gui::ViewProviderExtension)

ViewProviderDrawingViewExtension::ViewProviderDrawingViewExtension()
{
    initExtensionType(ViewProviderDrawingViewExtension::getExtensionClassTypeId());
}

ViewProviderDrawingViewExtension::~ViewProviderDrawingViewExtension() {}

bool ViewProviderDrawingViewExtension::extensionCanDragObjects() const { return true; }

//we don't support dragging children of Views (Dimensions, Balloons, Hatches, etc) now, but we don't want another
//extension to drag our children and cause problems
bool ViewProviderDrawingViewExtension::extensionCanDragObject(App::DocumentObject* docObj) const
{
    (void)docObj;
    return true;
}

//the default drag will remove the object from the document until it is dropped and re-added, so we claim
//to do the dragging.
void ViewProviderDrawingViewExtension::extensionDragObject(App::DocumentObject* obj) { (void)obj; }

//we don't support dropping of new children of Views (Dimensions, Balloons, Hatches, etc) now, but we don't want another
//extension to try to drop on us and cause problems
bool ViewProviderDrawingViewExtension::extensionCanDropObjects() const { return true; }

//let the page have any drops we receive.
bool ViewProviderDrawingViewExtension::extensionCanDropObject(App::DocumentObject* obj) const
{
    // it can happen that if the tree gets badly corrupted, there can be loose
    // objects that have no page or view provider, so we need to check that
    // all these objects exist.
    auto vpdv = getViewProviderDrawingView();
    if (!vpdv) {
        return false;
    }
    auto vpp  = vpdv->getViewProviderPage();
    if (!vpp) {
        return false;
    }
    auto vppEx = vpp->getVPPExtension();
    if (!vppEx) {
        return false;
    }
    return getViewProviderDrawingView()
        ->getViewProviderPage()
        ->getVPPExtension()
        ->extensionCanDropObject(obj);
}

//let the page have any drops we receive
void ViewProviderDrawingViewExtension::extensionDropObject(App::DocumentObject* obj)
{
    getViewProviderDrawingView()->getViewProviderPage()->getVPPExtension()->extensionDropObject(
        obj);
}

const ViewProviderDrawingView* ViewProviderDrawingViewExtension::getViewProviderDrawingView() const
{
    return dynamic_cast<const ViewProviderDrawingView*>(getExtendedViewProvider());
}

const char* ViewProviderDrawingViewExtension::whoAmI() const
{
    auto parent = getViewProviderDrawingView();
    if (parent) {
        return parent->whoAmI();
    }
    return nullptr;
}

namespace Gui
{
EXTENSION_PROPERTY_SOURCE_TEMPLATE(TechDrawGui::ViewProviderDrawingViewExtensionPython,
                                   TechDrawGui::ViewProviderDrawingViewExtension)

// explicit template instantiation
template class TechDrawGuiExport
    ViewProviderExtensionPythonT<TechDrawGui::ViewProviderDrawingViewExtension>;
}// namespace Gui
