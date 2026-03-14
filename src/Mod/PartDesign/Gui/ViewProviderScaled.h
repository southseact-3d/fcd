#ifndef PARTGUI_ViewProviderScaled_H
#define PARTGUI_ViewProviderScaled_H

#include "ViewProviderTransformed.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderScaled: public ViewProviderTransformed
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderScaled)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderScaled);

public:
    ViewProviderScaled()
    {
        menuName = tr("Scale Parameters");
        sPixmap = "PartDesign_Scaled.svg";
    }

    const std::string& featureName() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly create dialog for the part to be placed in the task view
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderScaled_H
