#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTBEARING_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTBEARING_H

#include "ViewProviderFemConstraint.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintBearing: public FemGui::ViewProviderFemConstraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintBearing);

public:
    /// Constructor
    ViewProviderFemConstraintBearing();
    ~ViewProviderFemConstraintBearing() override;

    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTBEARING_H
