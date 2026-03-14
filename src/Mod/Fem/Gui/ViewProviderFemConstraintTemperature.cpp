#include "Mod/Fem/App/FemConstraintTemperature.h"
#include "TaskFemConstraintTemperature.h"
#include "ViewProviderFemConstraintTemperature.h"
#include <Gui/Control.h>


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintTemperature, FemGui::ViewProviderFemConstraintOnBoundary)

ViewProviderFemConstraintTemperature::ViewProviderFemConstraintTemperature()
{
    sPixmap = "FEM_ConstraintTemperature";
    loadSymbol((resourceSymbolDir + "ConstraintTemperature.iv").c_str());
    ShapeAppearance.setDiffuseColor(1.0f, 0.0f, 0.0f);
}

ViewProviderFemConstraintTemperature::~ViewProviderFemConstraintTemperature() = default;

bool ViewProviderFemConstraintTemperature::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintTemperature(this));

        return true;
    }
    else {
        return ViewProviderFemConstraintOnBoundary::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintTemperature::updateData(const App::Property* prop)
{
    ViewProviderFemConstraint::updateData(prop);
}
