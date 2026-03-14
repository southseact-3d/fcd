#ifndef PARTGUI_ViewProviderGroove_H
#define PARTGUI_ViewProviderGroove_H

#include "ViewProviderSketchBased.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderGroove: public ViewProviderSketchBased
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderGroove);

public:
    /// constructor
    ViewProviderGroove();
    /// destructor
    ~ViewProviderGroove() override;

    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /**
     * Returns a newly created TaskDlgRevolutionParameters
     * NOTE: as for now groove and revolution share the dialog implementation
     */
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderGroove_H
