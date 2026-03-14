#ifndef FEM_CONSTRAINTPSPRING_H
#define FEM_CONSTRAINTPSPRING_H

#include "FemConstraint.h"

namespace Fem
{

class FemExport ConstraintSpring: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintSpring);

public:
    ConstraintSpring();

    App::PropertyStiffness NormalStiffness;
    App::PropertyStiffness TangentialStiffness;
    App::PropertyEnumeration ElmerStiffness;

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override;

protected:
    void onChanged(const App::Property* prop) override;
};

}  // namespace Fem

#endif  // FEM_CONSTRAINTPSPRING_H
