#include "FemConstraintFixed.h"


using namespace Fem;

PROPERTY_SOURCE(Fem::ConstraintFixed, Fem::Constraint)

ConstraintFixed::ConstraintFixed()
{}

App::DocumentObjectExecReturn* ConstraintFixed::execute()
{
    return Constraint::execute();
}

void ConstraintFixed::onChanged(const App::Property* prop)
{
    Constraint::onChanged(prop);
}
