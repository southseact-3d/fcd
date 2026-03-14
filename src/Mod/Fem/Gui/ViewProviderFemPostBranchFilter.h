#ifndef FEM_VIEWPROVIDERFEMPOSTBRANCHFILTER_H
#define FEM_VIEWPROVIDERFEMPOSTBRANCHFILTER_H

#include "ViewProviderFemPostObject.h"
#include <Gui/ViewProviderGroupExtension.h>


namespace Gui
{
class SelectionChanges;
class SoFCColorBar;
}  // namespace Gui

namespace FemGui
{

class TaskDlgPost;

class FemGuiExport ViewProviderFemPostBranchFilter: public ViewProviderFemPostObject,
                                                    public Gui::ViewProviderGroupExtension
{
    PROPERTY_HEADER_WITH_EXTENSIONS(FemGui::ViewProviderFemPostBranchFilter);

public:
    ViewProviderFemPostBranchFilter();
    ~ViewProviderFemPostBranchFilter() override;

protected:
    virtual void setupTaskDialog(TaskDlgPost* dlg) override;

    // change default group drag/drop behaviour slightly
    bool acceptReorderingObjects() const override;
    bool canDragObjectToTarget(App::DocumentObject* obj, App::DocumentObject* target) const override;

    // override, to not show/hide children as the parent is shown/hidden like normal groups
    void extensionHide() override {};
    void extensionShow() override {};
};

}  // namespace FemGui


#endif  // FEM_VIEWPROVIDERFEMPOSTBRANCHFILTER_H
