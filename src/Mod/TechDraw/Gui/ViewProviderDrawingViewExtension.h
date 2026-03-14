#ifndef TECHDRAWGUI_VIEWPROVIDERDRAWINGVIEWEXTENSION_H
#define TECHDRAWGUI_VIEWPROVIDERDRAWINGVIEWEXTENSION_H

#include <Gui/ViewProviderExtension.h>
#include <Gui/ViewProviderExtensionPython.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


namespace TechDrawGui
{
class ViewProviderDrawingView;

class TechDrawGuiExport ViewProviderDrawingViewExtension: public Gui::ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderDrawingViewExtension);

public:
    /// Constructor
    ViewProviderDrawingViewExtension();
    ~ViewProviderDrawingViewExtension() override;

    bool extensionCanDragObjects() const override;
    bool extensionCanDragObject(App::DocumentObject*) const override;
    void extensionDragObject(App::DocumentObject*) override;
    bool extensionCanDropObjects() const override;
    bool extensionCanDropObject(App::DocumentObject*) const override;
    void extensionDropObject(App::DocumentObject*) override;

    const ViewProviderDrawingView* getViewProviderDrawingView() const;
    const char* whoAmI() const;

private:
};

using ViewProviderDrawingViewExtensionPython =
    Gui::ViewProviderExtensionPythonT<TechDrawGui::ViewProviderDrawingViewExtension>;

}// namespace TechDrawGui

#endif// TECHDRAWGUI_VIEWPROVIDERDRAWINGVIEWEXTENSION_H
