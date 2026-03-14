#ifndef TECHDRAWGUI_VIEWPROVIDERPAGEEXTENSION_H
#define TECHDRAWGUI_VIEWPROVIDERPAGEEXTENSION_H

#include <Gui/ViewProviderExtension.h>
#include <Gui/ViewProviderExtensionPython.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDrawGui
{
class ViewProviderPage;

class TechDrawGuiExport ViewProviderPageExtension: public Gui::ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderPageExtension);

public:
    /// Constructor
    ViewProviderPageExtension();
    ~ViewProviderPageExtension() override;

    bool extensionCanDragObjects() const override;
    bool extensionCanDragObject(App::DocumentObject*) const override;
    void extensionDragObject(App::DocumentObject*) override;
    bool extensionCanDropObjects() const override;
    bool extensionCanDropObject(App::DocumentObject*) const override;
    bool extensionCanDropObjectEx(App::DocumentObject* obj, App::DocumentObject* owner,
        const char* subname,
        const std::vector<std::string>& elements) const override;
    void extensionDropObject(App::DocumentObject*) override;

    void dropObject(App::DocumentObject* docObj);

    const ViewProviderPage* getViewProviderPage() const;
    const char* whoAmI() const;

private:
};

using ViewProviderPageExtensionPython =
    Gui::ViewProviderExtensionPythonT<TechDrawGui::ViewProviderPageExtension>;

}// namespace TechDrawGui

#endif// TECHDRAWGUI_VIEWPROVIDERPAGEEXTENSION_H
