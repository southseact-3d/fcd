#include "FemConstraintPressure.h"


using namespace Fem;

PROPERTY_SOURCE(Fem::ConstraintPressure, Fem::Constraint)

ConstraintPressure::ConstraintPressure()
{
    ADD_PROPERTY(Pressure, (0.0));
    ADD_PROPERTY(Reversed, (0));
    ADD_PROPERTY_TYPE(
        EnableAmplitude,
        (false),
        "ConstraintPressure",
        (App::PropertyType)(App::Prop_None),
        "Amplitude of the pressure load"
    );
    ADD_PROPERTY_TYPE(
        AmplitudeValues,
        (std::vector<std::string> {"0, 0", "1, 1"}),
        "ConstraintPressure",
        (App::PropertyType)(App::Prop_None),
        "Amplitude values"
    );
}

App::DocumentObjectExecReturn* ConstraintPressure::execute()
{
    return Constraint::execute();
}

const char* ConstraintPressure::getViewProviderName() const
{
    return "FemGui::ViewProviderFemConstraintPressure";
}

void ConstraintPressure::handleChangedPropertyType(
    Base::XMLReader& reader,
    const char* TypeName,
    App::Property* prop
)
{
    // property Pressure had App::PropertyFloat and was changed to App::PropertyPressure
    if (prop == &Pressure && strcmp(TypeName, "App::PropertyFloat") == 0) {
        App::PropertyFloat PressureProperty;
        // restore the PropertyFloat to be able to set its value
        PressureProperty.Restore(reader);
        // the old implementation or pressure stored the value as MPa
        // therefore we must convert the value with a factor 1000
        Pressure.setValue(PressureProperty.getValue() * 1000.0);
    }
    else {
        Constraint::handleChangedPropertyType(reader, TypeName, prop);
    }
}
