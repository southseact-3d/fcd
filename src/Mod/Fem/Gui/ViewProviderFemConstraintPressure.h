#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTPRESSURE_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTPRESSURE_H

#include "ViewProviderFemConstraintOnBoundary.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintPressure
    : public FemGui::ViewProviderFemConstraintOnBoundary
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintPressure);

public:
    ViewProviderFemConstraintPressure();
    ~ViewProviderFemConstraintPressure() override;
    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
    void transformSymbol(
        const Base::Vector3d& point,
        const Base::Vector3d& normal,
        SbMatrix& mat
    ) const override;
};

}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTPRESSURE_H
