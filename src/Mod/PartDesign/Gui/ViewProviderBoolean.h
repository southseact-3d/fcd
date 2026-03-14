#ifndef PARTGUI_ViewProviderBoolean_H
#define PARTGUI_ViewProviderBoolean_H

#include "ViewProvider.h"
#include <Gui/ViewProviderGeoFeatureGroupExtension.h>
#include <Gui/Inventor/SoToggleSwitch.h>


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderBoolean: public ViewProvider,
                                               public Gui::ViewProviderGeoFeatureGroupExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(PartDesignGui::ViewProviderBoolean);

public:
    /// constructor
    ViewProviderBoolean();
    /// destructor
    ~ViewProviderBoolean() override;

    App::PropertyEnumeration Display;

    /// grouping handling
    void setupContextMenu(QMenu*, QObject*, const char*) override;

    bool onDelete(const std::vector<std::string>&) override;
    const char* getDefaultDisplayMode() const override;
    void onChanged(const App::Property* prop) override;

protected:
    void updateData(const App::Property* prop) override;

    void attachPreview() override;
    void updatePreview() override;

    TaskDlgFeatureParameters* getEditDialog() override;

    static const char* DisplayEnum[];

private:
    void updateBasePreviewVisibility();

    Gui::CoinPtr<SoGroup> pcToolsPreview;
    Gui::CoinPtr<SoToggleSwitch> pcBasePreviewToggle;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderBoolean_H
