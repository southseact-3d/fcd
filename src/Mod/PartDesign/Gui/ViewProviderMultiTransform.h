#ifndef PARTGUI_ViewProviderMultiTransform_H
#define PARTGUI_ViewProviderMultiTransform_H

#include "ViewProviderTransformed.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderMultiTransform: public ViewProviderTransformed
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderMultiTransform)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderMultiTransform);

public:
    ViewProviderMultiTransform()
    {
        menuName = tr("Multi-Transform Parameters");
        sPixmap = "PartDesign_MultiTransform.svg";
    }

    const std::string& featureName() const override;
    std::vector<App::DocumentObject*> claimChildren() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

    bool onDelete(const std::vector<std::string>&) override;

protected:
    /// Returns a newly create dialog for the part to be placed in the task view
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderMultiTransform_H
