#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTFIXED_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTFIXED_H

#include "ViewProviderFemConstraintOnBoundary.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintFixed: public FemGui::ViewProviderFemConstraintOnBoundary
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintFixed);

public:
    /// Constructor
    ViewProviderFemConstraintFixed();
    ~ViewProviderFemConstraintFixed() override;

    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTFIXED_H
