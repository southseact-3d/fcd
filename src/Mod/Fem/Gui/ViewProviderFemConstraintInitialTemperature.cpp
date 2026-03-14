#include "Mod/Fem/App/FemConstraintInitialTemperature.h"
#include <Gui/Control.h>

#include "TaskFemConstraintInitialTemperature.h"
#include "ViewProviderFemConstraintInitialTemperature.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintInitialTemperature, FemGui::ViewProviderFemConstraint)

ViewProviderFemConstraintInitialTemperature::ViewProviderFemConstraintInitialTemperature()
{
    sPixmap = "FEM_ConstraintInitialTemperature";
    ShapeAppearance.setDiffuseColor(0.2f, 0.3f, 0.2f);
}

ViewProviderFemConstraintInitialTemperature::~ViewProviderFemConstraintInitialTemperature() = default;

bool ViewProviderFemConstraintInitialTemperature::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintInitialTemperature(this));

        return true;
    }
    else {
        return ViewProviderFemConstraint::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintInitialTemperature::updateData(const App::Property* prop)
{
    // Gets called whenever a property of the attached object changes
    ViewProviderFemConstraint::updateData(prop);
}
