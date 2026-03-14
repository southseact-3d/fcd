#include <Gui/Control.h>
#include <Mod/Robot/App/TrajectoryCompound.h>
#include <Mod/Robot/Gui/TaskDlgTrajectoryCompound.h>

#include "ViewProviderTrajectoryCompound.h"


using namespace Gui;
using namespace RobotGui;

PROPERTY_SOURCE(RobotGui::ViewProviderTrajectoryCompound, RobotGui::ViewProviderTrajectory)

// bool ViewProviderTrajectoryCompound::doubleClicked(void)
//{
//     Gui::TaskView::TaskDialog* dlg = new
//     TaskDlgTrajectoryCompound(getObject<Robot::TrajectoryCompound >());
//     Gui::Control().showDialog(dlg);
//     return true;
// }


bool ViewProviderTrajectoryCompound::setEdit(int)
{
    Gui::TaskView::TaskDialog* dlg = new TaskDlgTrajectoryCompound(
        getObject<Robot::TrajectoryCompound>()
    );
    Gui::Control().showDialog(dlg);
    return true;
}

void ViewProviderTrajectoryCompound::unsetEdit(int)
{
    // when pressing ESC make sure to close the dialog
    Gui::Control().closeDialog();
}

std::vector<App::DocumentObject*> ViewProviderTrajectoryCompound::claimChildren() const
{
    return getObject<Robot::TrajectoryCompound>()->Source.getValues();
}
