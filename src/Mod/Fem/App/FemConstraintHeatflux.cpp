#include "FemConstraintHeatflux.h"


using namespace Fem;

PROPERTY_SOURCE(Fem::ConstraintHeatflux, Fem::Constraint)

static const char* ConstraintTypes[] = {"DFlux", "Convection", "Radiation", nullptr};

ConstraintHeatflux::ConstraintHeatflux()
{
    ADD_PROPERTY_TYPE(AmbientTemp, (0.0), "ConstraintHeatflux", App::Prop_None, "Ambient temperature");
    /*ADD_PROPERTY(FaceTemp,(0.0));*/
    ADD_PROPERTY_TYPE(FilmCoef, (0.0), "ConstraintHeatflux", App::Prop_None, "Film coefficient");
    ADD_PROPERTY_TYPE(Emissivity, (0.0), "ConstraintHeatflux", App::Prop_None, "Emissivity");
    ADD_PROPERTY_TYPE(DFlux, (0.0), "ConstraintHeatflux", App::Prop_None, "Distributed heat flux");
    ADD_PROPERTY_TYPE(
        ConstraintType,
        (1),
        "ConstraintHeatflux",
        App::Prop_None,
        "Type of constraint, surface convection, radiation or surface heat flux"
    );
    ConstraintType.setEnums(ConstraintTypes);
    ADD_PROPERTY_TYPE(CavityRadiation, (false), "ConstraintHeatflux", App::Prop_None, "Cavity radiation");
    ADD_PROPERTY_TYPE(CavityName, ("cav"), "ConstraintHeatflux", App::Prop_None, "Cavity name");
    ADD_PROPERTY_TYPE(
        EnableAmplitude,
        (false),
        "ConstraintHeatflux",
        (App::PropertyType)(App::Prop_None),
        "Amplitude of the heat flux load"
    );
    ADD_PROPERTY_TYPE(
        AmplitudeValues,
        (std::vector<std::string> {"0, 0", "1, 1"}),
        "ConstraintHeatflux",
        (App::PropertyType)(App::Prop_None),
        "Amplitude values"
    );
}

App::DocumentObjectExecReturn* ConstraintHeatflux::execute()
{
    return Constraint::execute();
}

const char* ConstraintHeatflux::getViewProviderName() const
{
    return "FemGui::ViewProviderFemConstraintHeatflux";
}

void ConstraintHeatflux::handleChangedPropertyType(
    Base::XMLReader& reader,
    const char* typeName,
    App::Property* prop
)
{
    if (prop == &FilmCoef && strcmp(typeName, "App::PropertyFloat") == 0) {
        App::PropertyFloat filmCoefProperty;
        filmCoefProperty.Restore(reader);
        FilmCoef.setValue(filmCoefProperty.getValue());
    }
    else if (prop == &DFlux && strcmp(typeName, "App::PropertyFloat") == 0) {
        App::PropertyFloat dFluxProperty;
        dFluxProperty.Restore(reader);
        DFlux.setValue(dFluxProperty.getValue());
    }
    else if (prop == &AmbientTemp && strcmp(typeName, "App::PropertyFloat") == 0) {
        App::PropertyFloat tempProperty;
        tempProperty.Restore(reader);
        AmbientTemp.setValue(tempProperty.getValue());
    }
    else {
        Constraint::handleChangedPropertyType(reader, typeName, prop);
    }
}

void ConstraintHeatflux::onChanged(const App::Property* prop)
{
    Constraint::onChanged(prop);
}
