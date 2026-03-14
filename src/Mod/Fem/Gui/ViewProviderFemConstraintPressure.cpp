#include <Inventor/SbMatrix.h>
#include <Inventor/SbRotation.h>
#include <Inventor/SbVec3f.h>


#include "Mod/Fem/App/FemConstraintPressure.h"
#include <Gui/Control.h>

#include "TaskFemConstraintPressure.h"
#include "ViewProviderFemConstraintPressure.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintPressure, FemGui::ViewProviderFemConstraintOnBoundary)

ViewProviderFemConstraintPressure::ViewProviderFemConstraintPressure()
{
    sPixmap = "FEM_ConstraintPressure";
    loadSymbol((resourceSymbolDir + "ConstraintPressure.iv").c_str());
    ShapeAppearance.setDiffuseColor(0.0f, 0.2f, 0.8f);
}

ViewProviderFemConstraintPressure::~ViewProviderFemConstraintPressure() = default;

bool ViewProviderFemConstraintPressure::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintPressure(this));

        return true;
    }
    else {
        return ViewProviderFemConstraintOnBoundary::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintPressure::updateData(const App::Property* prop)
{
    auto pcConstraint = this->getObject<Fem::ConstraintPressure>();

    if (prop == &pcConstraint->Reversed) {
        updateSymbol();
    }
    else {
        ViewProviderFemConstraint::updateData(prop);
    }
}

void ViewProviderFemConstraintPressure::transformSymbol(
    const Base::Vector3d& point,
    const Base::Vector3d& normal,
    SbMatrix& mat
) const
{
    auto obj = this->getObject<const Fem::ConstraintPressure>();
    float rotAngle = obj->Reversed.getValue() ? std::numbers::pi_v<float> : 0.0f;
    float s = obj->getScaleFactor();
    // Symbol length from .iv file
    float symLen = 4.0f;
    SbMatrix mat0, mat1;
    mat0.setTransform(
        SbVec3f(0, 0, 0),
        SbRotation(SbVec3f(0, 0, 1), rotAngle),
        SbVec3f(1, 1, 1),
        SbRotation(SbVec3f(0, 0, 1), 0),
        SbVec3f(0, symLen / 2.0f, 0)
    );

    mat1.setTransform(
        SbVec3f(point.x, point.y, point.z),
        SbRotation(SbVec3f(0, 1, 0), SbVec3f(normal.x, normal.y, normal.z)),
        SbVec3f(s, s, s)
    );

    mat = mat0 * mat1;
}
