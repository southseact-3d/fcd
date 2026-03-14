#ifndef PARTGUI_ViewProviderPolarPattern_H
#define PARTGUI_ViewProviderPolarPattern_H

#include "ViewProviderTransformed.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderPolarPattern: public ViewProviderTransformed
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderPolarPattern)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderPolarPattern);

public:
    ViewProviderPolarPattern()
    {
        menuName = tr("Polar Pattern Parameters");
        sPixmap = "PartDesign_PolarPattern.svg";
    }

    const std::string& featureName() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly create dialog for the part to be placed in the task view
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderPolarPattern_H
