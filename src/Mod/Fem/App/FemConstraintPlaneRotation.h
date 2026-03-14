#ifndef FEM_CONSTRAINTPLANEROTATION_H
#define FEM_CONSTRAINTPLANEROTATION_H

#include "FemConstraint.h"

namespace Fem
{

class FemExport ConstraintPlaneRotation: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintPlaneRotation);

public:
    /// Constructor
    ConstraintPlaneRotation();

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override;

protected:
    void onChanged(const App::Property* prop) override;
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTPLANEROTATION_H
