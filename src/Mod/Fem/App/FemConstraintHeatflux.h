#ifndef FEM_CONSTRAINTHEATFLUX_H
#define FEM_CONSTRAINTHEATFLUX_H

#include "FemConstraint.h"

namespace Fem
{

class FemExport ConstraintHeatflux: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintHeatflux);

public:
    ConstraintHeatflux();

    App::PropertyBool EnableAmplitude;
    App::PropertyStringList AmplitudeValues;

    App::PropertyTemperature AmbientTemp;
    /*App::PropertyFloat FaceTemp;*/
    App::PropertyThermalTransferCoefficient FilmCoef;
    App::PropertyFloat Emissivity;
    App::PropertyHeatFlux DFlux;
    App::PropertyEnumeration ConstraintType;
    App::PropertyBool CavityRadiation;
    App::PropertyString CavityName;

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override;

protected:
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* typeName,
        App::Property* prop
    ) override;
    void onChanged(const App::Property* prop) override;
};

}  // namespace Fem

#endif  // FEM_CONSTRAINTHEATFLUX_H
