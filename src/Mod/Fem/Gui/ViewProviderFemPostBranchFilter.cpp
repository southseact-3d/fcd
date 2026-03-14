#include "TaskPostBoxes.h"
#include "ViewProviderFemPostBranchFilter.h"
#include <Mod/Fem/App/FemPostGroupExtension.h>
#include <Gui/BitmapFactory.h>


using namespace FemGui;


PROPERTY_SOURCE_WITH_EXTENSIONS(FemGui::ViewProviderFemPostBranchFilter, FemGui::ViewProviderFemPostObject)

ViewProviderFemPostBranchFilter::ViewProviderFemPostBranchFilter()
    : Gui::ViewProviderGroupExtension()
{
    Gui::ViewProviderGroupExtension::initExtension(this);
    sPixmap = "FEM_PostBranchFilter";
}

ViewProviderFemPostBranchFilter::~ViewProviderFemPostBranchFilter()
{}

void ViewProviderFemPostBranchFilter::setupTaskDialog(TaskDlgPost* dlg)
{
    // add the branch ui
    auto panel = new TaskPostBranch(this);
    dlg->addTaskBox(panel->windowIcon().pixmap(32), panel);

    // add the display options
    FemGui::ViewProviderFemPostObject::setupTaskDialog(dlg);
}

bool ViewProviderFemPostBranchFilter::acceptReorderingObjects() const
{
    return true;
}

bool ViewProviderFemPostBranchFilter::canDragObjectToTarget(
    App::DocumentObject*,
    App::DocumentObject* target
) const
{

    // allow drag only to other post groups
    if (target) {
        return target->hasExtension(Fem::FemPostGroupExtension::getExtensionClassTypeId());
    }
    else {
        return false;
    }
}
