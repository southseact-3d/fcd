#include "Gui/Control.h"
#include <Mod/Fem/App/FemConstraintFixed.h>

#include "TaskFemConstraintFixed.h"
#include "ViewProviderFemConstraintFixed.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintFixed, FemGui::ViewProviderFemConstraintOnBoundary)


ViewProviderFemConstraintFixed::ViewProviderFemConstraintFixed()
{
    sPixmap = "FEM_ConstraintFixed";
    loadSymbol((resourceSymbolDir + "ConstraintFixed.iv").c_str());
}

ViewProviderFemConstraintFixed::~ViewProviderFemConstraintFixed() = default;

bool ViewProviderFemConstraintFixed::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintFixed(this));

        return true;
    }
    else {
        return ViewProviderFemConstraintOnBoundary::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintFixed::updateData(const App::Property* prop)
{
    ViewProviderFemConstraint::updateData(prop);
}
