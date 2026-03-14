#ifndef GUI_VIEWPROVIDERFEMFLUIDBOUNDARY_H
#define GUI_VIEWPROVIDERFEMFLUIDBOUNDARY_H

#include "ViewProviderFemConstraintOnBoundary.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintFluidBoundary
    : public FemGui::ViewProviderFemConstraintOnBoundary
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintFluidBoundary);

public:
    /// Constructor
    ViewProviderFemConstraintFluidBoundary();
    ~ViewProviderFemConstraintFluidBoundary() override;

    void updateData(const App::Property*) override;
    // virtual void onChanged(const App::Property*); //no further property for viewProvider
protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // GUI_VIEWPROVIDERFEMConstraintFluidBoundary_H
