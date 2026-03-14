#include <Precision.hxx>


#include "FemConstraintGear.h"


using namespace Fem;

PROPERTY_SOURCE(Fem::ConstraintGear, Fem::ConstraintBearing)

ConstraintGear::ConstraintGear()
{
    ADD_PROPERTY(Diameter, (100.0));
    ADD_PROPERTY(Force, (1000.0));
    ADD_PROPERTY(ForceAngle, (0.0));
    ADD_PROPERTY_TYPE(
        Direction,
        (nullptr),
        "ConstraintGear",
        (App::PropertyType)(App::Prop_None),
        "Element giving direction of gear force"
    );
    ADD_PROPERTY(Reversed, (0));
    ADD_PROPERTY_TYPE(
        DirectionVector,
        (Base::Vector3d(1, 1, 1).Normalize()),
        "ConstraintGear",
        App::PropertyType(App::Prop_ReadOnly | App::Prop_Output),
        "Direction of gear force"
    );
    naturalDirectionVector = Base::Vector3d(1, 1, 1).Normalize();
}

App::DocumentObjectExecReturn* ConstraintGear::execute()
{
    return ConstraintBearing::execute();
}

void ConstraintGear::onChanged(const App::Property* prop)
{
    ConstraintBearing::onChanged(prop);

    if (prop == &Direction) {
        Base::Vector3d direction = getDirection(Direction);
        if (direction.Length() < Precision::Confusion()) {
            return;
        }
        naturalDirectionVector = direction;
        if (Reversed.getValue()) {
            direction = -direction;
        }
        DirectionVector.setValue(direction);
        DirectionVector.touch();
    }
    else if (prop == &Reversed) {
        if (Reversed.getValue() && (DirectionVector.getValue() == naturalDirectionVector)) {
            DirectionVector.setValue(-naturalDirectionVector);
            DirectionVector.touch();
        }
        else if (!Reversed.getValue() && (DirectionVector.getValue() != naturalDirectionVector)) {
            DirectionVector.setValue(naturalDirectionVector);
            DirectionVector.touch();
        }
    }
    // The computation for the force angle is simpler in the ViewProvider directly
}
