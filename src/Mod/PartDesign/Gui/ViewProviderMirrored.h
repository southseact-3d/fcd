#ifndef PARTGUI_ViewProviderMirrored_H
#define PARTGUI_ViewProviderMirrored_H

#include "ViewProviderTransformed.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderMirrored: public ViewProviderTransformed
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGuii::ViewProviderMirrored)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderMirrored);

public:
    ViewProviderMirrored()
    {
        menuName = tr("Mirror Parameters");
        sPixmap = "PartDesign_Mirrored.svg";
    }

    const std::string& featureName() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly created dialog for the part to be placed in the task view
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderMirrored_H
