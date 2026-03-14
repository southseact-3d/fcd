#include <Gui/Control.h>
#include <Mod/Fem/App/FemSetNodesObject.h>
#include <Mod/Fem/Gui/TaskDlgCreateNodeSet.h>

#include "ViewProviderSetNodes.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderSetNodes, Gui::ViewProviderGeometryObject)

bool ViewProviderSetNodes::doubleClicked()
{
    Gui::TaskView::TaskDialog* dlg = new TaskDlgCreateNodeSet(getObject<Fem::FemSetNodesObject>());
    Gui::Control().showDialog(dlg);
    return true;
}


bool ViewProviderSetNodes::setEdit(int)
{
    Gui::TaskView::TaskDialog* dlg = new TaskDlgCreateNodeSet(getObject<Fem::FemSetNodesObject>());
    Gui::Control().showDialog(dlg);
    return true;
}

void ViewProviderSetNodes::unsetEdit(int)
{}
