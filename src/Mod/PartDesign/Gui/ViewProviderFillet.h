#ifndef PARTGUI_ViewProviderFillet_H
#define PARTGUI_ViewProviderFillet_H

#include "ViewProviderDressUp.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderFillet: public ViewProviderDressUp
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderFillet)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderFillet);

public:
    /// constructor
    ViewProviderFillet()
    {
        sPixmap = "PartDesign_Fillet.svg";
        menuName = tr("Fillet Parameters");
    }

    /// return "Fillet"
    const std::string& featureName() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly create dialog for the part to be placed in the task view
    TaskDlgFeatureParameters* getEditDialog() override;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderFillet_H
