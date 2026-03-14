#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTPLANEROTATION_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTPLANEROTATION_H

#include "ViewProviderFemConstraint.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintPlaneRotation: public FemGui::ViewProviderFemConstraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintPlaneRotation);

public:
    ViewProviderFemConstraintPlaneRotation();
    ~ViewProviderFemConstraintPlaneRotation() override;
    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTPLANEROTATION_H
