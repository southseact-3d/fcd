#include <Gui/Control.h>
#include <Mod/Robot/Gui/TaskDlgEdge2Trac.h>

#include "ViewProviderEdge2TracObject.h"


using namespace Gui;
using namespace RobotGui;

PROPERTY_SOURCE(RobotGui::ViewProviderEdge2TracObject, RobotGui::ViewProviderTrajectory)

bool ViewProviderEdge2TracObject::doubleClicked()
{
    Gui::TaskView::TaskDialog* dlg = new TaskDlgEdge2Trac(getObject<Robot::Edge2TracObject>());
    Gui::Control().showDialog(dlg);
    return true;
}


bool ViewProviderEdge2TracObject::setEdit(int)
{
    Gui::TaskView::TaskDialog* dlg = new TaskDlgEdge2Trac(getObject<Robot::Edge2TracObject>());
    Gui::Control().showDialog(dlg);
    return true;
}

void ViewProviderEdge2TracObject::unsetEdit(int)
{}
