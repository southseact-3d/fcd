#include "Mod/Fem/App/FemConstraintHeatflux.h"
#include <Gui/Control.h>

#include "TaskFemConstraintHeatflux.h"
#include "ViewProviderFemConstraintHeatflux.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintHeatflux, FemGui::ViewProviderFemConstraintOnBoundary)

ViewProviderFemConstraintHeatflux::ViewProviderFemConstraintHeatflux()
{
    sPixmap = "FEM_ConstraintHeatflux";
    loadSymbol((resourceSymbolDir + "ConstraintHeatFlux.iv").c_str());
    ShapeAppearance.setDiffuseColor(1.0f, 0.0f, 0.0f);
}

ViewProviderFemConstraintHeatflux::~ViewProviderFemConstraintHeatflux() = default;

bool ViewProviderFemConstraintHeatflux::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintHeatflux(this));

        return true;
    }
    else {
        return ViewProviderFemConstraintOnBoundary::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintHeatflux::updateData(const App::Property* prop)
{
    ViewProviderFemConstraint::updateData(prop);
}
