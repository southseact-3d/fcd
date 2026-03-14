#include "ViewProviderSetElements.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderSetElements, Gui::ViewProviderGeometryObject)

bool ViewProviderSetElements::doubleClicked()
{
    // Gui::TaskView::TaskDialog* dlg = new TaskDlgCreateNodeSet(getObject<Fem::FemSetNodesObject
    // >()); Gui::Control().showDialog(dlg);
    return true;
}


bool ViewProviderSetElements::setEdit(int)
{
    // Gui::TaskView::TaskDialog* dlg = new TaskDlgCreateNodeSet(getObject<Fem::FemSetNodesObject
    // >()); Gui::Control().showDialog(dlg);
    return true;
}

void ViewProviderSetElements::unsetEdit(int)
{}
