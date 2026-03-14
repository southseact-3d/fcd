#include "ViewProviderSetGeometry.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderSetGeometry, Gui::ViewProviderGeometryObject)

bool ViewProviderSetGeometry::doubleClicked()
{
    // Gui::TaskView::TaskDialog* dlg = new TaskDlgCreateNodeSet(getObject<Fem::FemSetNodesObject
    // >()); Gui::Control().showDialog(dlg);
    return true;
}


bool ViewProviderSetGeometry::setEdit(int)
{
    // Gui::TaskView::TaskDialog* dlg = new TaskDlgCreateNodeSet(getObject<Fem::FemSetNodesObject
    // >()); Gui::Control().showDialog(dlg);
    return true;
}

void ViewProviderSetGeometry::unsetEdit(int)
{}
