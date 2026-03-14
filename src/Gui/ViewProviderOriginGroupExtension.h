#ifndef GUI_VIEWPROVIDERORIGINGROUPEXTENSION_H
#define GUI_VIEWPROVIDERORIGINGROUPEXTENSION_H

#include "ViewProviderGeoFeatureGroup.h"


namespace Gui
{

class GuiExport ViewProviderOriginGroupExtension: public ViewProviderGeoFeatureGroupExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderOriginGroupExtension);

public:
    /// Constructor
    ViewProviderOriginGroupExtension();
    ~ViewProviderOriginGroupExtension() override;

    std::vector<App::DocumentObject*> extensionClaimChildren() const override;
    std::vector<App::DocumentObject*> extensionClaimChildren3D() const override;

private:
    std::vector<App::DocumentObject*> constructChildren(
        const std::vector<App::DocumentObject*>& children
    ) const;
};

using ViewProviderOriginGroupExtensionPython
    = ViewProviderExtensionPythonT<Gui::ViewProviderOriginGroupExtension>;

}  // namespace Gui

#endif  // GUI_VIEWPROVIDERORIGINGROUPEXTENSION_H
