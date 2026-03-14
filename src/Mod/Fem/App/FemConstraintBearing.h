#ifndef FEM_CONSTRAINTBEARING_H
#define FEM_CONSTRAINTBEARING_H

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>

#include "FemConstraint.h"


namespace Fem
{

class FemExport ConstraintBearing: public Fem::Constraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(Fem::ConstraintBearing);

public:
    /// Constructor
    ConstraintBearing();

    /// Location reference
    App::PropertyLinkSub Location;
    /// Distance from location reference
    App::PropertyFloat Dist;
    /// Is the bearing free to move in axial direction?
    App::PropertyBool AxialFree;
    // Read-only (calculated values). These trigger changes in the ViewProvider
    App::PropertyFloat Radius;
    App::PropertyFloat Height;
    App::PropertyVector BasePoint;
    App::PropertyVector Axis;

    /// recalculate the object
    App::DocumentObjectExecReturn* execute() override;

    /// returns the type name of the ViewProvider
    const char* getViewProviderName() const override
    {
        return "FemGui::ViewProviderFemConstraintBearing";
    }

protected:
    void onChanged(const App::Property* prop) override;
};

}  // namespace Fem


#endif  // FEM_CONSTRAINTBEARING_H
