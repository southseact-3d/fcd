#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTCONTACT_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTCONTACT_H

#include "ViewProviderFemConstraint.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintContact: public FemGui::ViewProviderFemConstraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintContact);

public:
    ViewProviderFemConstraintContact();
    ~ViewProviderFemConstraintContact() override;
    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};
}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTContact_H
