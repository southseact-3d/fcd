#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTFORCE_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTFORCE_H

#include "ViewProviderFemConstraintOnBoundary.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintForce: public FemGui::ViewProviderFemConstraintOnBoundary
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintForce);

public:
    /// Constructor
    ViewProviderFemConstraintForce();
    ~ViewProviderFemConstraintForce() override;

    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
    void transformSymbol(
        const Base::Vector3d& point,
        const Base::Vector3d& normal,
        SbMatrix& mat
    ) const override;

private:
    /// Direction of the force
    Base::Vector3f forceDirection;
};

}  // namespace FemGui


#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTFORCE_H
