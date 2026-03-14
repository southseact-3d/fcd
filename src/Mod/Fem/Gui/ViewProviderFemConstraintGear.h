#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTGear_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTGear_H

#include "ViewProviderFemConstraint.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintGear: public FemGui::ViewProviderFemConstraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintGear);

public:
    /// Constructor
    ViewProviderFemConstraintGear();
    ~ViewProviderFemConstraintGear() override;

    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTGear_H
