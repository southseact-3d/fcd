#include <Gui/Control.h>
#include <Mod/Robot/Gui/TaskDlgTrajectoryDressUp.h>

#include "ViewProviderTrajectoryDressUp.h"


using namespace Gui;
using namespace RobotGui;

PROPERTY_SOURCE(RobotGui::ViewProviderTrajectoryDressUp, RobotGui::ViewProviderTrajectory)

// bool ViewProviderTrajectoryDressUp::doubleClicked(void)
//{
//     Gui::TaskView::TaskDialog* dlg = new
//     TaskDlgTrajectoryDressUp(getObject<Robot::TrajectoryDressUpObject >());
//     Gui::Control().showDialog(dlg);
//     return true;
// }
//

bool ViewProviderTrajectoryDressUp::setEdit(int)
{
    Gui::TaskView::TaskDialog* dlg = new TaskDlgTrajectoryDressUp(
        getObject<Robot::TrajectoryDressUpObject>()
    );
    Gui::Control().showDialog(dlg);
    return true;
}

void ViewProviderTrajectoryDressUp::unsetEdit(int)
{
    // when pressing ESC make sure to close the dialog
    Gui::Control().closeDialog();
}

std::vector<App::DocumentObject*> ViewProviderTrajectoryDressUp::claimChildren() const
{
    std::vector<App::DocumentObject*> temp;
    temp.push_back(getObject<Robot::TrajectoryDressUpObject>()->Source.getValue());

    return temp;
}
