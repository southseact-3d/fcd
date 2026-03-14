#ifndef FEM_CONSTRAINTDISPLACEMENT_H
#define FEM_CONSTRAINTDISPLACEMENT_H

#include "FemConstraint.h"


namespace Fem
{

class FemExport ConstraintDisplacement: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintDisplacement);

public:
    /// Constructor
    ConstraintDisplacement();

    App::PropertyBool EnableAmplitude;
    App::PropertyStringList AmplitudeValues;

    // Displacement parameters
    App::PropertyDistance xDisplacement;
    App::PropertyDistance yDisplacement;
    App::PropertyDistance zDisplacement;
    App::PropertyAngle xRotation;
    App::PropertyAngle yRotation;
    App::PropertyAngle zRotation;
    App::PropertyString xDisplacementFormula;
    App::PropertyString yDisplacementFormula;
    App::PropertyString zDisplacementFormula;
    App::PropertyBool xFree;
    App::PropertyBool yFree;
    App::PropertyBool zFree;
    App::PropertyBool rotxFree;
    App::PropertyBool rotyFree;
    App::PropertyBool rotzFree;
    App::PropertyBool hasXFormula;
    App::PropertyBool hasYFormula;
    App::PropertyBool hasZFormula;
    App::PropertyBool useFlowSurfaceForce;

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


#endif  // FEM_CONSTRAINTDISPLACEMENT_H
