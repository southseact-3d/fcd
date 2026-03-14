#ifndef PARTGUI_ViewProviderPocket_H
#define PARTGUI_ViewProviderPocket_H

#include "ViewProviderExtrude.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderPocket: public ViewProviderExtrude
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderPocket);

public:
    /// constructor
    ViewProviderPocket();
    /// destructor
    ~ViewProviderPocket() override;

    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly created TaskDlgPocketParameters
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderPocket_H
