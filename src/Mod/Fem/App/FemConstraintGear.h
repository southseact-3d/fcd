#ifndef FEM_CONSTRAINTGear_H
#define FEM_CONSTRAINTGear_H

#include "FemConstraintBearing.h"


namespace Fem
{

class FemExport ConstraintGear: public Fem::ConstraintBearing
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintGear);

public:
    /// Constructor
    ConstraintGear();

    App::PropertyFloat Diameter;
    App::PropertyFloat Force;
    App::PropertyFloat ForceAngle;
    App::PropertyLinkSub Direction;
    App::PropertyBool Reversed;
    // Read-only (calculated values). These trigger changes in the ViewProvider
    App::PropertyVector DirectionVector;

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemConstraintGear";
    }

protected:
    void onChanged(const App::Property* prop) override;

private:
    Base::Vector3d naturalDirectionVector;
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTGear_H
