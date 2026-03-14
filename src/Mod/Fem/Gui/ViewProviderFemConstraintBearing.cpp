#include <Inventor/SbRotation.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/nodes/SoSeparator.h>


#include "Gui/Control.h"
#include "FemGuiTools.h"
#include "TaskFemConstraintBearing.h"
#include "ViewProviderFemConstraintBearing.h"
#include <Base/Console.h>
#include <Mod/Fem/App/FemConstraintBearing.h>


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintBearing, FemGui::ViewProviderFemConstraint)


ViewProviderFemConstraintBearing::ViewProviderFemConstraintBearing()
{
    sPixmap = "FEM_ConstraintBearing";
}

ViewProviderFemConstraintBearing::~ViewProviderFemConstraintBearing() = default;

bool ViewProviderFemConstraintBearing::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintBearing(this));

        return true;
    }
    else {
        return ViewProviderFemConstraint::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintBearing::updateData(const App::Property* prop)
{
    // Gets called whenever a property of the attached object changes
    Fem::ConstraintBearing* pcConstraint = this->getObject<Fem::ConstraintBearing>();

    if (prop == &pcConstraint->References) {
        Base::Console().error("\n");  // enable a breakpoint here
    }

    if (prop == &pcConstraint->BasePoint) {
        // Remove and recreate the symbol
        Gui::coinRemoveAllChildren(pShapeSep);

        // This should always point outside of the cylinder
        Base::Vector3d normal = pcConstraint->NormalDirection.getValue();
        Base::Vector3d base = pcConstraint->BasePoint.getValue();
        double radius = pcConstraint->Radius.getValue();
        base = base + radius * normal;

        SbVec3f b(base.x, base.y, base.z);
        SbVec3f dir(normal.x, normal.y, normal.z);
        SbRotation rot(SbVec3f(0, -1, 0), dir);

        GuiTools::createPlacement(pShapeSep, b, rot);
        pShapeSep->addChild(
            GuiTools::createFixed(radius / 2, radius / 2 * 1.5, pcConstraint->AxialFree.getValue())
        );
    }
    else if (prop == &pcConstraint->AxialFree) {
        if (pShapeSep->getNumChildren() > 0) {
            // Change the symbol
            Base::Vector3d normal = pcConstraint->NormalDirection.getValue();
            Base::Vector3d base = pcConstraint->BasePoint.getValue();
            double radius = pcConstraint->Radius.getValue();
            base = base + radius * normal;

            SbVec3f b(base.x, base.y, base.z);
            SbVec3f dir(normal.x, normal.y, normal.z);
            SbRotation rot(SbVec3f(0, -1, 0), dir);

            GuiTools::updatePlacement(pShapeSep, 0, b, rot);
            const SoSeparator* sep = static_cast<SoSeparator*>(pShapeSep->getChild(2));
            GuiTools::updateFixed(
                sep,
                0,
                radius / 2,
                radius / 2 * 1.5,
                pcConstraint->AxialFree.getValue()
            );
        }
    }

    ViewProviderFemConstraint::updateData(prop);
}
