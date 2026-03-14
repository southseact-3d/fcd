#include "Mod/Fem/App/FemConstraintPlaneRotation.h"
#include <Gui/Control.h>

#include "TaskFemConstraintPlaneRotation.h"
#include "ViewProviderFemConstraintPlaneRotation.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintPlaneRotation, FemGui::ViewProviderFemConstraint)

ViewProviderFemConstraintPlaneRotation::ViewProviderFemConstraintPlaneRotation()
{
    sPixmap = "FEM_ConstraintPlaneRotation";
    loadSymbol((resourceSymbolDir + "ConstraintPlaneRotation.iv").c_str());
    // Note change "planerotation" in line above to new constraint name, make sure it is the same as
    // in taskFem* cpp file
    ShapeAppearance.setDiffuseColor(0.2f, 0.3f, 0.2f);
}

ViewProviderFemConstraintPlaneRotation::~ViewProviderFemConstraintPlaneRotation() = default;

bool ViewProviderFemConstraintPlaneRotation::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintPlaneRotation(this));

        return true;
    }
    else {
        return ViewProviderFemConstraint::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintPlaneRotation::updateData(const App::Property* prop)
{
    ViewProviderFemConstraint::updateData(prop);
}
