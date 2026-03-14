#ifndef PARTGUI_ViewProviderShapeBinder_H
#define PARTGUI_ViewProviderShapeBinder_H

#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Part/Gui/ViewProvider.h>
#include <Mod/PartDesign/PartDesignGlobal.h>

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderShapeBinder: public PartGui::ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderShapeBinder);

public:
    /// Constructor
    ViewProviderShapeBinder();
    ~ViewProviderShapeBinder() override;

    void setupContextMenu(QMenu*, QObject*, const char*) override;
    void highlightReferences(bool on);

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
    void attach(App::DocumentObject* obj) override;

private:
    std::vector<Base::Color> originalLineColors;
    std::vector<App::Material> originalFaceAppearance;
};

class PartDesignGuiExport ViewProviderSubShapeBinder: public PartGui::ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderShapeBinder);

public:
    App::PropertyBool UseBinderStyle;

    /// Constructor
    ViewProviderSubShapeBinder();

    bool canDropObjects() const override
    {
        return true;
    }
    bool canDragAndDropObject(App::DocumentObject*) const override
    {
        return false;
    }
    bool canDropObjectEx(
        App::DocumentObject* obj,
        App::DocumentObject* owner,
        const char* subname,
        const std::vector<std::string>& elements
    ) const override;
    std::string dropObjectEx(
        App::DocumentObject*,
        App::DocumentObject*,
        const char*,
        const std::vector<std::string>&
    ) override;
    std::vector<App::DocumentObject*> claimChildren() const override;

    bool doubleClicked() override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;
    bool setEdit(int ModNum) override;
    void attach(App::DocumentObject* obj) override;
    void onChanged(const App::Property* prop) override;

private:
    enum
    {
        Synchronize = 0,
        SelectObject = 4  // must be higher than 3 (EditMode::Color)
    };
    void updatePlacement(bool transaction);
};

using ViewProviderSubShapeBinderPython = Gui::ViewProviderFeaturePythonT<ViewProviderSubShapeBinder>;

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderShapeBinder_H
