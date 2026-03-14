#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTSPRING_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTSPRING_H

#include "ViewProviderFemConstraintOnBoundary.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintSpring: public FemGui::ViewProviderFemConstraintOnBoundary
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintSpring);

public:
    ViewProviderFemConstraintSpring();
    ~ViewProviderFemConstraintSpring() override;
    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTSPRING_H
