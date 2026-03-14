#ifndef FEM_CONSTRAINTCONTACT_H
#define FEM_CONSTRAINTCONTACT_H

#include "FemConstraint.h"

namespace Fem
{

class FemExport ConstraintContact: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintContact);

public:
    /// Constructor
    ConstraintContact();

    /*Note*/
    // Constraint parameters
    /******
     * Add the constraint parameters here, the variables or data
     * that needs to be eventually send over to the calculix input file.
     * This is only the definitions of the variables
     ******/
    // ex.
    App::PropertyStiffnessDensity Slope;
    App::PropertyLength Adjust;
    App::PropertyBool Friction;
    App::PropertyFloat FrictionCoefficient;
    App::PropertyStiffnessDensity StickSlope;
    App::PropertyBool EnableThermalContact;
    App::PropertyStringList ThermalContactConductance;
    App::PropertyEnumeration SurfaceBehavior;

    // etc
    /* */

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override;

protected:
    void onChanged(const App::Property* prop) override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* typeName,
        App::Property* prop
    ) override;
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTCONTACT_H
