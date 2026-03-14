#ifndef GUI_VIEWPROVIDERGROUPEXTENSION_H
#define GUI_VIEWPROVIDERGROUPEXTENSION_H

#include "ViewProviderExtensionPython.h"


namespace Gui
{

class GuiExport ViewProviderGroupExtension: public ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderGroupExtension);

public:
    /// Constructor
    ViewProviderGroupExtension();
    ~ViewProviderGroupExtension() override;

    std::vector<App::DocumentObject*> extensionClaimChildren() const override;
    bool extensionCanDragObjects() const override;
    bool extensionCanDragObject(App::DocumentObject*) const override;
    void extensionDragObject(App::DocumentObject*) override;
    bool extensionCanDropObjects() const override;
    bool extensionCanDropObject(App::DocumentObject*) const override;
    void extensionDropObject(App::DocumentObject*) override;

    void extensionHide() override;
    void extensionShow() override;

    bool extensionOnDelete(const std::vector<std::string>&) override;

private:
    bool guard {false};
    std::vector<ViewProvider*> nodes;
};

using ViewProviderGroupExtensionPython = ViewProviderExtensionPythonT<Gui::ViewProviderGroupExtension>;

}  // namespace Gui

#endif  // GUI_VIEWPROVIDERGROUPEXTENSION_H
