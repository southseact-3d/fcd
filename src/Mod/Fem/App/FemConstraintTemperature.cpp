#include "FemConstraintTemperature.h"


using namespace Fem;

PROPERTY_SOURCE(Fem::ConstraintTemperature, Fem::Constraint)

static const char* ConstraintTypes[] = {"CFlux", "Temperature", nullptr};

ConstraintTemperature::ConstraintTemperature()
{
    ADD_PROPERTY(Temperature, (300.0));
    ADD_PROPERTY(CFlux, (0.0));
    ADD_PROPERTY_TYPE(
        ConstraintType,
        (1),
        "ConstraintTemperature",
        (App::PropertyType)(App::Prop_None),
        "Type of constraint, temperature or concentrated heat flux"
    );
    ConstraintType.setEnums(ConstraintTypes);
    ADD_PROPERTY_TYPE(
        EnableAmplitude,
        (false),
        "ConstraintTemperature",
        (App::PropertyType)(App::Prop_None),
        "Amplitude of the temperature boundary condition"
    );
    ADD_PROPERTY_TYPE(
        AmplitudeValues,
        (std::vector<std::string> {"0, 0", "1, 1"}),
        "ConstraintTemperature",
        (App::PropertyType)(App::Prop_None),
        "Amplitude values"
    );
}

App::DocumentObjectExecReturn* ConstraintTemperature::execute()
{
    return Constraint::execute();
}

const char* ConstraintTemperature::getViewProviderName() const
{
    return "FemGui::ViewProviderFemConstraintTemperature";
}

void ConstraintTemperature::handleChangedPropertyType(
    Base::XMLReader& reader,
    const char* TypeName,
    App::Property* prop
)
{
    // property Temperature had App::PropertyFloat and was changed to App::PropertyTemperature
    if (prop == &Temperature && strcmp(TypeName, "App::PropertyFloat") == 0) {
        App::PropertyFloat TemperatureProperty;
        // restore the PropertyFloat to be able to set its value
        TemperatureProperty.Restore(reader);
        Temperature.setValue(TemperatureProperty.getValue());
    }
    // property CFlux had App::PropertyFloat and was changed to App::PropertyPower
    else if (prop == &CFlux && strcmp(TypeName, "App::PropertyFloat") == 0) {
        App::PropertyFloat CFluxProperty;
        CFluxProperty.Restore(reader);
        CFlux.setValue(CFluxProperty.getValue());
    }
    else {
        Constraint::handleChangedPropertyType(reader, TypeName, prop);
    }
}

void ConstraintTemperature::onChanged(const App::Property* prop)
{
    Constraint::onChanged(prop);
}
