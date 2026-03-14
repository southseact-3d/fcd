#ifndef PARTGUI_ViewProviderChamfer_H
#define PARTGUI_ViewProviderChamfer_H

#include "ViewProviderDressUp.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderChamfer: public ViewProviderDressUp
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderChamfer)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderChamfer);

public:
    /// constructor
    ViewProviderChamfer()
    {
        sPixmap = "PartDesign_Chamfer.svg";
        menuName = tr("Chamfer Parameters");
    }

    /// return "Chamfer"
    const std::string& featureName() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly create dialog for the part to be placed in the task view
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderChamfer_H
