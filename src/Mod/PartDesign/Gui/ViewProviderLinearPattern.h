#ifndef PARTGUI_ViewProviderLinearPattern_H
#define PARTGUI_ViewProviderLinearPattern_H

#include "ViewProviderTransformed.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderLinearPattern: public ViewProviderTransformed
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderLinearPattern)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderLinearPattern);

public:
    ViewProviderLinearPattern()
    {
        menuName = tr("Linear Pattern Parameters");
        sPixmap = "PartDesign_LinearPattern.svg";
    }

    const std::string& featureName() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly create dialog for the part to be placed in the task view
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderLinearPattern_H
