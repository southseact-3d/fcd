#ifndef PARTGUI_ViewProviderHelix_H
#define PARTGUI_ViewProviderHelix_H

#include "ViewProvider.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderHelix: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderHelix);

public:
    /// constructor
    ViewProviderHelix();
    /// destructor
    ~ViewProviderHelix() override;

    void setupContextMenu(QMenu*, QObject*, const char*) override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;

protected:
    QIcon getIcon() const override;

    /// Returns a newly created TaskDlgHelixParameters
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderHelix_H
