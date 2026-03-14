#ifndef FEM_CONSTRAINTFORCE_H
#define FEM_CONSTRAINTFORCE_H

#include "FemConstraint.h"


namespace Fem
{

class FemExport ConstraintForce: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintForce);

public:
    /// Constructor
    ConstraintForce();

    App::PropertyBool EnableAmplitude;
    App::PropertyStringList AmplitudeValues;
    App::PropertyForce Force;
    App::PropertyLinkSub Direction;
    App::PropertyBool Reversed;
    App::PropertyVector DirectionVector;

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemConstraintForce";
    }

protected:
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;
    void onChanged(const App::Property* prop) override;

private:
    Base::Vector3d naturalDirectionVector;
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTFORCE_H
