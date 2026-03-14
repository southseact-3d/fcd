#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTPulley_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTPulley_H

#include "ViewProviderFemConstraint.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintPulley: public FemGui::ViewProviderFemConstraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintPulley);

public:
    /// Constructor
    ViewProviderFemConstraintPulley();
    ~ViewProviderFemConstraintPulley() override;

    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTPulley_H
