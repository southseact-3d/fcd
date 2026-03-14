#ifndef TECHDRAWGUI_VIEWPROVIDERTEMPLATEEXTENSION_H
#define TECHDRAWGUI_VIEWPROVIDERTEMPLATEEXTENSION_H

#include <Gui/ViewProviderExtension.h>
#include <Gui/ViewProviderExtensionPython.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDrawGui
{
class ViewProviderTemplate;

class TechDrawGuiExport ViewProviderTemplateExtension: public Gui::ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderTemplateExtension);

public:
    /// Constructor
    ViewProviderTemplateExtension();
    ~ViewProviderTemplateExtension() override;

    bool extensionCanDragObjects() const override;
    bool extensionCanDragObject(App::DocumentObject* docObj) const override;
    bool extensionCanDropObjects() const override;
    bool extensionCanDropObject(App::DocumentObject* docObj) const override;

    const ViewProviderTemplate* getViewProviderTemplate() const;
    const char* whoAmI() const;

private:
};

using ViewProviderTemplateExtensionPython =
    Gui::ViewProviderExtensionPythonT<TechDrawGui::ViewProviderTemplateExtension>;

}// namespace TechDrawGui

#endif// TECHDRAWGUI_VIEWPROVIDERTEMPLATEEXTENSION_H
