#ifndef FEM_CONSTRAINTRIGIDBODY_H
#define FEM_CONSTRAINTRIGIDBODY_H

#include "FemConstraint.h"


namespace Fem
{

class FemExport ConstraintRigidBody: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintRigidBody);

public:
    /// Constructor
    ConstraintRigidBody();

    App::PropertyBool EnableAmplitude;
    App::PropertyStringList AmplitudeValues;

    // Rigid Body parameters
    App::PropertyPosition ReferenceNode;
    App::PropertyPosition Displacement;
    App::PropertyRotation Rotation;
    App::PropertyForce ForceX;
    App::PropertyForce ForceY;
    App::PropertyForce ForceZ;
    App::PropertyMoment MomentX;
    App::PropertyMoment MomentY;
    App::PropertyMoment MomentZ;
    App::PropertyEnumeration TranslationalModeX;
    App::PropertyEnumeration TranslationalModeY;
    App::PropertyEnumeration TranslationalModeZ;
    App::PropertyEnumeration RotationalModeX;
    App::PropertyEnumeration RotationalModeY;
    App::PropertyEnumeration RotationalModeZ;

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemConstraintRigidBody";
    }

protected:
    void onChanged(const App::Property* prop) override;

private:
    static const char* boundaryModeEnum[];
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTRIGIDBODY_H
