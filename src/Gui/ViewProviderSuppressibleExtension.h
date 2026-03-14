#ifndef VIEWPROVIDERSUPPRESSIBLEEXTENSION_H
#define VIEWPROVIDERSUPPRESSIBLEEXTENSION_H

#include "ViewProviderExtensionPython.h"


namespace Gui
{

class GuiExport ViewProviderSuppressibleExtension: public ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderSuppressibleExtension);

public:
    ViewProviderSuppressibleExtension();
    ~ViewProviderSuppressibleExtension() override;

    void extensionUpdateData(const App::Property* prop) override;

    void setSuppressedIcon(bool onoff);
    QIcon extensionMergeColorfullOverlayIcons(const QIcon& orig) const override;
    void extensionSetupContextMenu(QMenu* menu, QObject*, const char*) override;

private:
    bool isSetSuppressedIcon {false};
};

using ViewProviderSuppressibleExtensionPython
    = ViewProviderExtensionPythonT<Gui::ViewProviderSuppressibleExtension>;

}  // namespace Gui

#endif  // VIEWPROVIDERSUPPRESSIBLEEXTENSION_H
