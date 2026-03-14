#ifndef PARTGUI_ViewProviderRevolution_H
#define PARTGUI_ViewProviderRevolution_H

#include "ViewProviderSketchBased.h"


namespace PartDesignGui
{

class PartDesignGuiExport ViewProviderRevolution: public ViewProviderSketchBased
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesignGui::ViewProviderRevolution);

public:
    /// constructor
    ViewProviderRevolution();
    /// destructor
    ~ViewProviderRevolution() override;

    void setupContextMenu(QMenu*, QObject*, const char*) override;

protected:
    /// Returns a newly created TaskDlgRevolutionParameters
    TaskDlgFeatureParameters* getEditDialog() override;
};


}  // namespace PartDesignGui


#endif  // PARTGUI_ViewProviderRevolution_H
