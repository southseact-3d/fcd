#ifndef FEM_CONSTRAINTPRESSURE_H
#define FEM_CONSTRAINTPRESSURE_H

#include "FemConstraint.h"

namespace Fem
{

class FemExport ConstraintPressure: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintPressure);

public:
    ConstraintPressure();

    App::PropertyBool EnableAmplitude;
    App::PropertyStringList AmplitudeValues;
    App::PropertyPressure Pressure;
    App::PropertyBool Reversed;

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
};

}  // namespace Fem

#endif  // FEM_CONSTRAINTPRESSURE_H
