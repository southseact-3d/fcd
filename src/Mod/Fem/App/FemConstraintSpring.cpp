#include "FemConstraintSpring.h"


static const char* Stiffnesses[] = {"Normal Stiffness", "Tangential Stiffness", nullptr};

using namespace Fem;

PROPERTY_SOURCE(Fem::ConstraintSpring, Fem::Constraint)

ConstraintSpring::ConstraintSpring()
{
    ADD_PROPERTY(NormalStiffness, (0.0));
    ADD_PROPERTY(TangentialStiffness, (0.0));
    ADD_PROPERTY(ElmerStiffness, (1));

    ElmerStiffness.setEnums(Stiffnesses);
}

App::DocumentObjectExecReturn* ConstraintSpring::execute()
{
    return Constraint::execute();
}

const char* ConstraintSpring::getViewProviderName() const
{
    return "FemGui::ViewProviderFemConstraintSpring";
}

void ConstraintSpring::onChanged(const App::Property* prop)
{
    Constraint::onChanged(prop);
}
