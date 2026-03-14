#include "ViewProviderSetFaces.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderSetFaces, Gui::ViewProviderGeometryObject)

bool ViewProviderSetFaces::doubleClicked()
{
    // Gui::TaskView::TaskDialog* dlg = new TaskDlgCreateNodeSet(getObject<Fem::FemSetNodesObject
    // >()); Gui::Control().showDialog(dlg);
    return true;
}


bool ViewProviderSetFaces::setEdit(int)
{
    // Gui::TaskView::TaskDialog* dlg = new TaskDlgCreateNodeSet(getObject<Fem::FemSetNodesObject
    // >()); Gui::Control().showDialog(dlg);
    return true;
}

void ViewProviderSetFaces::unsetEdit(int)
{}
