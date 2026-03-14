#ifndef GUI_VIEWPROVIDERATTACHEXTENSION_H
#define GUI_VIEWPROVIDERATTACHEXTENSION_H

#include <Gui/ViewProviderExtensionPython.h>
#include <Mod/Part/PartGlobal.h>


namespace PartGui
{

class PartGuiExport ViewProviderAttachExtension: public Gui::ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderAttachExtension);

public:
    /// Constructor
    ViewProviderAttachExtension();
    ~ViewProviderAttachExtension() override = default;

    QIcon extensionMergeColorfullOverlayIcons(const QIcon& orig) const override;

    void extensionUpdateData(const App::Property*) override;
    void extensionSetupContextMenu(QMenu*, QObject*, const char*) override;

    void showAttachmentEditor(std::function<void()> onAccept = {}, std::function<void()> onReject = {});
};

using ViewProviderAttachExtensionPython
    = Gui::ViewProviderExtensionPythonT<PartGui::ViewProviderAttachExtension>;

}  // namespace PartGui

#endif  // GUI_VIEWPROVIDERATTACHMENTEXTENSION_H
