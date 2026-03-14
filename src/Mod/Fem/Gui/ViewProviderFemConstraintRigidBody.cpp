#include <Inventor/nodes/SoTransform.h>


#include "Gui/Control.h"
#include <Mod/Fem/App/FemConstraintRigidBody.h>

#include "TaskFemConstraintRigidBody.h"
#include "ViewProviderFemConstraintRigidBody.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintRigidBody, FemGui::ViewProviderFemConstraintOnBoundary)


ViewProviderFemConstraintRigidBody::ViewProviderFemConstraintRigidBody()
{
    sPixmap = "FEM_ConstraintRigidBody";
    loadSymbol((resourceSymbolDir + "ConstraintRigidBody.iv").c_str());
    ShapeAppearance.setDiffuseColor(0.0f, 0.5f, 0.0f);
}

ViewProviderFemConstraintRigidBody::~ViewProviderFemConstraintRigidBody() = default;

bool ViewProviderFemConstraintRigidBody::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintRigidBody(this));

        return true;
    }
    else {
        return ViewProviderFemConstraintOnBoundary::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintRigidBody::updateData(const App::Property* prop)
{
    auto obj = this->getObject<Fem::ConstraintRigidBody>();

    if (prop == &obj->ReferenceNode) {
        updateSymbol();
    }

    ViewProviderFemConstraint::updateData(prop);
}

void ViewProviderFemConstraintRigidBody::transformExtraSymbol() const
{
    SoTransform* symTrans = getExtraSymbolTransform();
    if (symTrans) {
        auto obj = this->getObject<const Fem::ConstraintRigidBody>();
        float s = obj->getScaleFactor();
        const Base::Vector3d& refNode = obj->ReferenceNode.getValue();
        SbVec3f tra(refNode.x, refNode.y, refNode.z);
        SbVec3f sca(s, s, s);
        SbRotation rot(SbVec3f(0, 0, 1), 0);

        SbMatrix mat;
        mat.setTransform(tra, rot, sca);

        symTrans->setMatrix(mat);
    }
}
