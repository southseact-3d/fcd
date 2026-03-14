#ifndef GUI_VIEWPROVIDERCOSMETICEXTENSION_H
#define GUI_VIEWPROVIDERCOSMETICEXTENSION_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Gui/ViewProviderExtensionPython.h>

namespace TechDrawGui
{

class TechDrawGuiExport ViewProviderCosmeticExtension : public Gui::ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(TechDrawGui::ViewProviderCosmeticExtension);

public:
    /// Constructor
    ViewProviderCosmeticExtension();
    ~ViewProviderCosmeticExtension() override = default;

    QIcon extensionMergeGreyableOverlayIcons(const QIcon & orig) const override;
};

using ViewProviderCosmeticExtensionPython = Gui::ViewProviderExtensionPythonT<TechDrawGui::ViewProviderCosmeticExtension>;

} //namespace TechDrawGui

#endif // GUI_VIEWPROVIDERCOSMETICEXTENSION_H
