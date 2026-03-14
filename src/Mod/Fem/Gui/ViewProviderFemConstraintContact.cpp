#include "Mod/Fem/App/FemConstraintContact.h"
#include "TaskFemConstraintContact.h"
#include "ViewProviderFemConstraintContact.h"
#include <Gui/Control.h>


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintContact, FemGui::ViewProviderFemConstraint)

ViewProviderFemConstraintContact::ViewProviderFemConstraintContact()
{
    sPixmap = "FEM_ConstraintContact";
    loadSymbol((resourceSymbolDir + "ConstraintContact.iv").c_str());
    ShapeAppearance.setDiffuseColor(0.2f, 0.3f, 0.2f);
}

ViewProviderFemConstraintContact::~ViewProviderFemConstraintContact() = default;

bool ViewProviderFemConstraintContact::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintContact(this));

        return true;
    }
    else {
        return ViewProviderFemConstraint::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintContact::updateData(const App::Property* prop)
{
    ViewProviderFemConstraint::updateData(prop);
}
