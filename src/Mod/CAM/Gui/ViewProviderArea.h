#ifndef PATH_ViewProviderArea_H
#define PATH_ViewProviderArea_H

#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Part/Gui/ViewProviderPlaneParametric.h>
#include <Mod/CAM/PathGlobal.h>

namespace PathGui
{

class PathGuiExport ViewProviderArea: public PartGui::ViewProviderPlaneParametric
{
    PROPERTY_HEADER_WITH_OVERRIDE(PathGui::ViewProviderArea);

public:
    ViewProviderArea();
    ~ViewProviderArea() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    void updateData(const App::Property*) override;
    bool onDelete(const std::vector<std::string>&) override;

    /// drag and drop
    bool canDragObjects() const override;
    bool canDragObject(App::DocumentObject*) const override;
    void dragObject(App::DocumentObject*) override;
    bool canDropObjects() const override;
    bool canDropObject(App::DocumentObject*) const override;
    void dropObject(App::DocumentObject*) override;
};

using ViewProviderAreaPython = Gui::ViewProviderFeaturePythonT<ViewProviderArea>;


class PathGuiExport ViewProviderAreaView: public PartGui::ViewProviderPlaneParametric
{
    PROPERTY_HEADER_WITH_OVERRIDE(PathGui::ViewProviderAreaView);

public:
    ViewProviderAreaView();
    ~ViewProviderAreaView() override;
    std::vector<App::DocumentObject*> claimChildren() const override;
    void updateData(const App::Property*) override;
    bool onDelete(const std::vector<std::string>&) override;

    /// drag and drop
    bool canDragObjects() const override;
    bool canDragObject(App::DocumentObject*) const override;
    void dragObject(App::DocumentObject*) override;
    bool canDropObjects() const override;
    bool canDropObject(App::DocumentObject*) const override;
    void dropObject(App::DocumentObject*) override;
};

using ViewProviderAreaViewPython = Gui::ViewProviderFeaturePythonT<ViewProviderAreaView>;

}  // namespace PathGui


#endif  // PATH_ViewProviderArea_H
