#ifndef FEM_FLUIDBOUNDARY_H
#define FEM_FLUIDBOUNDARY_H

#include "FemConstraint.h"


namespace Fem
{

class FemExport ConstraintFluidBoundary: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintFluidBoundary);

public:
    /// Constructor
    ConstraintFluidBoundary();
    //
    App::PropertyEnumeration BoundaryType;
    App::PropertyEnumeration Subtype;
    App::PropertyFloat BoundaryValue;
    App::PropertyLinkSub Direction;

    App::PropertyEnumeration TurbulenceSpecification;
    App::PropertyFloat TurbulentIntensityValue;
    App::PropertyFloat TurbulentLengthValue;

    App::PropertyEnumeration ThermalBoundaryType;
    App::PropertyFloat TemperatureValue;
    App::PropertyFloat HeatFluxValue;
    App::PropertyFloat HTCoeffValue;

    App::PropertyBool Reversed;
    // Read-only (calculated values). These trigger changes in the ViewProvider
    App::PropertyVector DirectionVector;

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemConstraintFluidBoundary";
    }

protected:
    void onChanged(const App::Property* prop) override;

private:
    Base::Vector3d naturalDirectionVector;
};

}  // namespace Fem


#endif  // FEM_FLUIDBOUNDARY_H
