#include <QAction>
#include <QCoreApplication>
#include <QMenu>
#include <QMessageBox>


#include <Gui/Control.h>
#include <Gui/MainWindow.h>
#include <Gui/Selection/Selection.h>

#include "TaskDlgMeshShapeNetgen.h"
#include "ViewProviderFemMeshShapeNetgen.h"


using namespace FemGui;

/* TRANSLATOR FemGui::ViewProviderFemMeshShapeNetgen */

PROPERTY_SOURCE(FemGui::ViewProviderFemMeshShapeNetgen, FemGui::ViewProviderFemMeshShape)


ViewProviderFemMeshShapeNetgen::ViewProviderFemMeshShapeNetgen()
{
    sPixmap = "FEM_MeshNetgenFromShape";
}

ViewProviderFemMeshShapeNetgen::~ViewProviderFemMeshShapeNetgen() = default;

void ViewProviderFemMeshShapeNetgen::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    QAction* act = menu->addAction(QObject::tr("Meshing"), receiver, member);
    act->setData(QVariant((int)ViewProvider::Default));
}

bool ViewProviderFemMeshShapeNetgen::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
#ifdef FCWithNetgen
        Gui::Control().showDialog(new TaskDlgMeshShapeNetgen(this));
        return true;
#else
        QMessageBox::critical(
            Gui::getMainWindow(),
            QCoreApplication::translate("FemGui::ViewProviderFemMeshShapeNetgen", "Meshing failure"),
            QCoreApplication::translate(
                "FemGui::ViewProviderFemMeshShapeNetgen",
                "The FEM module is built without NETGEN support. Meshing will not work!!!"
            )
        );
        return false;
#endif
    }
    else {
        return ViewProviderDocumentObject::setEdit(ModNum);
    }
}

void ViewProviderFemMeshShapeNetgen::updateData(const App::Property* prop)
{
    ViewProviderFemMeshShape::updateData(prop);
}
