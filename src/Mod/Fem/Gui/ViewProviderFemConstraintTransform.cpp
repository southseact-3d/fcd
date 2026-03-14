#include <Inventor/SbMatrix.h>
#include <Inventor/SbRotation.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSwitch.h>
#include <Inventor/nodes/SoTransform.h>


#include "Mod/Fem/App/FemConstraintTransform.h"
#include "TaskFemConstraintTransform.h"
#include "ViewProviderFemConstraintTransform.h"
#include <Gui/Control.h>


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemConstraintTransform, FemGui::ViewProviderFemConstraint)

ViewProviderFemConstraintTransform::ViewProviderFemConstraintTransform()
{
    sPixmap = "FEM_ConstraintTransform";
    loadSymbol((resourceSymbolDir + "ConstraintTransform.iv").c_str());
}

ViewProviderFemConstraintTransform::~ViewProviderFemConstraintTransform() = default;

bool ViewProviderFemConstraintTransform::setEdit(int ModNum)
{
    if (ModNum == ViewProvider::Default) {
        Gui::Control().closeDialog();
        // clear the selection (convenience)
        Gui::Selection().clearSelection();
        Gui::Control().showDialog(new TaskDlgFemConstraintTransform(this));

        return true;
    }
    else {
        return ViewProviderDocumentObject::setEdit(ModNum);
    }
}

void ViewProviderFemConstraintTransform::updateData(const App::Property* prop)
{
    auto obj = this->getObject<Fem::ConstraintTransform>();

    if (prop == &obj->Rotation) {
        updateSymbol();
    }
    else if (prop == &obj->TransformType || prop == &obj->References) {
        std::string transType = obj->TransformType.getValueAsString();
        auto sw = static_cast<SoSwitch*>(getSymbolSeparator()->getChild(0));
        auto swExtra = static_cast<SoSwitch*>(getExtraSymbolSeparator()->getChild(0));

        if (transType == "Rectangular") {
            sw->whichChild.setValue(0);
            swExtra->whichChild.setValue(-1);
        }
        else if (transType == "Cylindrical") {
            sw->whichChild.setValue(1);
            if (obj->References.getSize()) {
                swExtra->whichChild.setValue(0);
            }
            else {
                swExtra->whichChild.setValue(-1);
            }
        }
        updateSymbol();
    }
    else if (prop == &obj->BasePoint || prop == &obj->Axis) {
        updateSymbol();
    }

    ViewProviderFemConstraint::updateData(prop);
}

void ViewProviderFemConstraintTransform::transformSymbol(
    const Base::Vector3d& point,
    const Base::Vector3d& normal,
    SbMatrix& mat
) const
{
    auto obj = this->getObject<const Fem::ConstraintTransform>();

    std::string transType = obj->TransformType.getValueAsString();
    if (transType == "Rectangular") {
        Base::Rotation rot = obj->Rotation.getValue();
        Base::Vector3d axis;
        double angle;
        rot.getValue(axis, angle);
        float s = obj->getScaleFactor();

        mat.setTransform(
            SbVec3f(point.x, point.y, point.z),
            SbRotation(SbVec3f(axis.x, axis.y, axis.z), angle),
            SbVec3f(s, s, s)
        );
    }
    else if (transType == "Cylindrical") {
        float s = obj->getScaleFactor();

        mat.setTransform(
            SbVec3f(point.x, point.y, point.z),
            SbRotation(SbVec3f(0, 1, 0), SbVec3f(normal.x, normal.y, normal.z)),
            SbVec3f(s, s, s)
        );
    }
}

void ViewProviderFemConstraintTransform::transformExtraSymbol() const
{
    auto obj = this->getObject<const Fem::ConstraintTransform>();
    std::string transType = obj->TransformType.getValueAsString();
    if (transType == "Cylindrical") {
        SoTransform* trans = getExtraSymbolTransform();
        Base::Vector3d point = obj->BasePoint.getValue();
        Base::Vector3d axis = obj->Axis.getValue();
        float s = obj->getScaleFactor();

        SbMatrix mat;
        mat.setTransform(
            SbVec3f(point.x, point.y, point.z),
            SbRotation(SbVec3f(0, 1, 0), SbVec3f(axis.x, axis.y, axis.z)),
            SbVec3f(s, s, s)
        );

        trans->setMatrix(mat);
    }
}
