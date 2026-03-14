#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTTemperature_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTTemperature_H

#include "ViewProviderFemConstraintOnBoundary.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintTemperature
    : public FemGui::ViewProviderFemConstraintOnBoundary
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintTemperature);

public:
    ViewProviderFemConstraintTemperature();
    ~ViewProviderFemConstraintTemperature() override;
    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTTemperature_H
