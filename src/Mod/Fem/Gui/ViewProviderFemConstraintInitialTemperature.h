#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTInitialTemperature_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTInitialTemperature_H

#include "ViewProviderFemConstraint.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintInitialTemperature
    : public FemGui::ViewProviderFemConstraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintInitialTemperature);

public:
    ViewProviderFemConstraintInitialTemperature();
    ~ViewProviderFemConstraintInitialTemperature() override;
    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTInitialTemperature_H
