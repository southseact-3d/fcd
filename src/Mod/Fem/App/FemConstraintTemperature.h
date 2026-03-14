#ifndef FEM_CONSTRAINTTEMPERATURE_H
#define FEM_CONSTRAINTTEMPERATURE_H

#include "FemConstraint.h"

namespace Fem
{

class FemExport ConstraintTemperature: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintTemperature);

public:
    /// Constructor
    ConstraintTemperature();

    App::PropertyBool EnableAmplitude;
    App::PropertyStringList AmplitudeValues;

    // Temperature parameters
    App::PropertyTemperature Temperature;
    App::PropertyPower CFlux;
    App::PropertyEnumeration ConstraintType;


    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override;

protected:
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    void onChanged(const App::Property* prop) override;
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTTEMPERATURE_H
