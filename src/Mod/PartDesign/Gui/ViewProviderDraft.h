#ifndef PARTGUI_ViewProviderDraft_H
#define PARTGUI_ViewProviderDraft_H

#include "ViewProviderDressUp.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderDraft: public ViewProviderDressUp
{
    Q_DECLARE_TR_FUNCTIONS(PartDesignGui::ViewProviderDraft)
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderDraft);

public:
    /// constructor
    ViewProviderDraft()
    {
        sPixmap = "PartDesign_Draft.svg";
        menuName = tr("Draft Parameters");
    }

    /// return "Draft"
    const std::string& featureName() const override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly create dialog for the part to be placed in the task view
    TaskDlgFeatureParameters* getEditDialog() override;
};

}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderDraft_H
