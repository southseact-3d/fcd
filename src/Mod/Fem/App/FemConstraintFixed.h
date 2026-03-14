#ifndef FEM_CONSTRAINTFIXED_H
#define FEM_CONSTRAINTFIXED_H

#include "FemConstraint.h"


namespace Fem
{

class FemExport ConstraintFixed: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintFixed);

public:
    /// Constructor
    ConstraintFixed();

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemConstraintFixed";
    }

protected:
    void onChanged(const App::Property* prop) override;
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTFIXED_H
