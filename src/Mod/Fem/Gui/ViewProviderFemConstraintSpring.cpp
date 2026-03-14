#include "Mod/Fem/App/FemConstraintSpring.h"
#include <Gui/Control.h>

#include "TaskFemConstraintSpring.h"
#include "ViewProviderFemConstraintSpring.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintSpring, FemGui::ViewProviderFemConstraintOnBoundary)

ViewProviderFemConstraintSpring::ViewProviderFemConstraintSpring()
{
    sPixmap = "FEM_ConstraintSpring";
    loadSymbol((resourceSymbolDir + "ConstraintSpring.iv").c_str());
    ShapeAppearance.setDiffuseColor(0.0f, 0.2f, 0.8f);
}

ViewProviderFemConstraintSpring::~ViewProviderFemConstraintSpring() = default;

bool ViewProviderFemConstraintSpring::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintSpring(this));

        return true;
    }
    else {
        return ViewProviderFemConstraintOnBoundary::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintSpring::updateData(const App::Property* prop)
{
    ViewProviderFemConstraint::updateData(prop);
}
