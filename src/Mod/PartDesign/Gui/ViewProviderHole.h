#ifndef PARTGUI_ViewProviderHole_H
#define PARTGUI_ViewProviderHole_H

#include "ViewProvider.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderHole: public ViewProvider
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderHole);

public:
    /// constructor
    ViewProviderHole();
    /// destructor
    ~ViewProviderHole() override;

    /// grouping handling
    std::vector<App::DocumentObject*> claimChildren() const override;
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;

protected:
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderHole_H
