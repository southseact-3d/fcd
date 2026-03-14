#ifndef PARTGUI_ViewProviderThickness_H
#define PARTGUI_ViewProviderThickness_H

#include "ViewProviderDressUp.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderThickness: public ViewProviderDressUp
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderThickness)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderThickness);

public:
    /// constructor
    ViewProviderThickness()
    {
        sPixmap = "PartDesign_Thickness.svg";
        menuName = tr("Thickness Parameters");
    }

    /// return "Thickness"
    const std::string& featureName() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly create dialog for the part to be placed in the task view
    TaskDlgFeatureParameters* getEditDialog() override;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderThickness_H
