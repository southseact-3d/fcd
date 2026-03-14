#ifndef PARTGUI_ViewProviderPad_H
#define PARTGUI_ViewProviderPad_H

#include "ViewProviderExtrude.h"

namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderPad: public ViewProviderExtrude
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderPad);

public:
    /// constructor
    ViewProviderPad();
    /// destructor
    ~ViewProviderPad() override;

    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly created TaskDlgPadParameters
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderPad_H
