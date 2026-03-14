#ifndef FEM_CONSTRAINTPulley_H
#define FEM_CONSTRAINTPulley_H

#include "FemConstraintGear.h"


namespace Fem
{

class FemExport ConstraintPulley: public Fem::ConstraintGear
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintPulley);

public:
    /// Constructor
    ConstraintPulley();

    /// Other pulley diameter
    App::PropertyFloat OtherDiameter;
    /// Center distance between the pulleys
    App::PropertyFloat CenterDistance;
    /// Driven pulley or driving pulley?
    App::PropertyBool IsDriven;
    /// Belt tension force
    App::PropertyFloat TensionForce;
    // Read-only (calculated values). These trigger changes in the ViewProvider
    App::PropertyFloat BeltAngle;
    App::PropertyFloat BeltForce1;
    App::PropertyFloat BeltForce2;

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemConstraintPulley";
    }

protected:
    void onChanged(const App::Property* prop) override;
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTPulley_H
