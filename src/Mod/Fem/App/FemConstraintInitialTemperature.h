#ifndef FEM_CONSTRAINTINITIALTEMPERATURE_H
#define FEM_CONSTRAINTINITIALTEMPERATURE_H

#include "FemConstraint.h"

namespace Fem
{

class FemExport ConstraintInitialTemperature: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintInitialTemperature);

public:
    /// Constructor
    ConstraintInitialTemperature();

    // Temperature parameters
    App::PropertyTemperature initialTemperature;
    App::PropertyBool EnableFinalTemperature;
    App::PropertyTemperature FinalTemperature;

    App::PropertyBool EnableAmplitude;
    App::PropertyStringList AmplitudeValues;

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


#endif  // FEM_CONSTRAINTINITIALTEMPERATURE_H
