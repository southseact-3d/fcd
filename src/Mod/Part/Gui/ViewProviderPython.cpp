#include <Inventor/nodes/SoSeparator.h>

#include <Gui/ViewProviderBuilder.h>

#include "ViewProviderPython.h"


using namespace PartGui;

PROPERTY_SOURCE(PartGui::ViewProviderCustom, PartGui::ViewProviderPart)

ViewProviderCustom::ViewProviderCustom() = default;

ViewProviderCustom::~ViewProviderCustom() = default;

void ViewProviderCustom::onChanged(const App::Property* prop)
{
    std::map<const App::Property*, Gui::ViewProvider*>::iterator it;
    for (it = propView.begin(); it != propView.end(); ++it) {
        App::Property* view = it->second->getPropertyByName(prop->getName());
        if (view) {
            App::Property* copy = prop->Copy();
            if (copy) {
                view->Paste(*copy);
                delete copy;
            }
        }
    }
    PartGui::ViewProviderPart::onChanged(prop);
}

void ViewProviderCustom::updateData(const App::Property* prop)
{
    if (prop->isDerivedFrom<App::PropertyComplexGeoData>()) {
        std::map<const App::Property*, Gui::ViewProvider*>::iterator it = propView.find(prop);
        if (it == propView.end()) {
            Gui::ViewProvider* view = Gui::ViewProviderBuilder::create(prop->getTypeId());
            if (view) {
                if (view->isDerivedFrom<Gui::ViewProviderDocumentObject>()) {
                    static_cast<Gui::ViewProviderDocumentObject*>(view)->attach(this->getObject());
                    static_cast<Gui::ViewProviderDocumentObject*>(view)->setDisplayMode(
                        this->getActiveDisplayMode().c_str()
                    );
                }
                propView[prop] = view;
                view->updateData(prop);
                this->getRoot()->addChild(view->getRoot());
            }
        }
        else {
            it->second->updateData(prop);
        }
    }
}

// -----------------------------------------------------------------------

namespace Gui
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(PartGui::ViewProviderPython, PartGui::ViewProviderPart)
/// @endcond

// explicit template instantiation
template class PartGuiExport ViewProviderFeaturePythonT<PartGui::ViewProviderPart>;

/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(PartGui::ViewProviderCustomPython, PartGui::ViewProviderCustom)
/// @endcond

// explicit template instantiation
template class PartGuiExport ViewProviderFeaturePythonT<PartGui::ViewProviderCustom>;
}  // namespace Gui
