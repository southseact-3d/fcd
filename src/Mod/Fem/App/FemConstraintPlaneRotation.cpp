#include "FemConstraintPlaneRotation.h"


using namespace Fem;

PROPERTY_SOURCE(Fem::ConstraintPlaneRotation, Fem::Constraint)

ConstraintPlaneRotation::ConstraintPlaneRotation()
{}

App::DocumentObjectExecReturn* ConstraintPlaneRotation::execute()
{
    return Constraint::execute();
}

const char* ConstraintPlaneRotation::getViewProviderName() const
{
    return "FemGui::ViewProviderFemConstraintPlaneRotation";
}

void ConstraintPlaneRotation::onChanged(const App::Property* prop)
{
    Constraint::onChanged(prop);
}
