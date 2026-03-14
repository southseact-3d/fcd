#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTDISPLACEMENT_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTDISPLACEMENT_H

#include "ViewProviderFemConstraintOnBoundary.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintDisplacement
    : public FemGui::ViewProviderFemConstraintOnBoundary
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintDisplacement);

public:
    ViewProviderFemConstraintDisplacement();
    ~ViewProviderFemConstraintDisplacement() override;
    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTDISPLACEMENT_H
