#include <Gui/Control.h>
#include <Mod/Fem/App/FemSetElementNodesObject.h>
#include <Mod/Fem/Gui/TaskDlgCreateElementSet.h>

#include "ViewProviderSetElementNodes.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderSetElementNodes, Gui::ViewProviderGeometryObject)

bool ViewProviderSetElementNodes::doubleClicked()
{
    Gui::TaskView::TaskDialog* dlg = new TaskDlgCreateElementSet(
        getObject<Fem::FemSetElementNodesObject>()
    );
    Gui::Control().showDialog(dlg);
    return true;
}


bool ViewProviderSetElementNodes::setEdit(int)
{
    doubleClicked();
    return true;
}

void ViewProviderSetElementNodes::unsetEdit(int)
{}
