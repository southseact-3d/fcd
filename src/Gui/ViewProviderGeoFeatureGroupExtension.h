#ifndef GUI_VIEWPROVIDERGEOFEATUREGROUPEXTENSION_H
#define GUI_VIEWPROVIDERGEOFEATUREGROUPEXTENSION_H

#include "ViewProviderGroupExtension.h"


namespace Gui
{

class GuiExport ViewProviderGeoFeatureGroupExtension: public ViewProviderGroupExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderGeoFeatureGroupExtension);

public:
    /// Constructor
    ViewProviderGeoFeatureGroupExtension();
    ~ViewProviderGeoFeatureGroupExtension() override;

    std::vector<App::DocumentObject*> extensionClaimChildren3D() const override;
    std::vector<App::DocumentObject*> extensionClaimChildren() const override;
    SoSeparator* extensionGetFrontRoot() const override
    {
        return pcGroupFront;
    }
    SoSeparator* extensionGetBackRoot() const override
    {
        return pcGroupBack;
    }
    SoGroup* extensionGetChildRoot() const override
    {
        return pcGroupChildren;
    }
    void extensionAttach(App::DocumentObject* pcObject) override;
    void extensionSetDisplayMode(const char* ModeName) override;
    std::vector<std::string> extensionGetDisplayModes() const override;
    void extensionFinishRestoring() override;

    /// Show the object in the view: suppresses behavior of DocumentObjectGroup
    void extensionShow() override
    {
        ViewProviderExtension::extensionShow();  // clazy:exclude=skipped-base-method
    }
    /// Hide the object in the view: suppresses behavior of DocumentObjectGroup
    void extensionHide() override
    {
        ViewProviderExtension::extensionHide();  // clazy:exclude=skipped-base-method
    }

    void extensionUpdateData(const App::Property*) override;

protected:
    SoSeparator* pcGroupFront;
    SoSeparator* pcGroupBack;
    SoGroup* pcGroupChildren;
};

using ViewProviderGeoFeatureGroupExtensionPython
    = ViewProviderExtensionPythonT<Gui::ViewProviderGeoFeatureGroupExtension>;

}  // namespace Gui

#endif  // GUI_VIEWPROVIDERGEOFEATUREGROUPEXTENSION_H
