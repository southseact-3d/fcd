#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTTransform_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTTransform_H

#include "ViewProviderFemConstraint.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintTransform: public FemGui::ViewProviderFemConstraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintTransform);

public:
    ViewProviderFemConstraintTransform();
    ~ViewProviderFemConstraintTransform() override;
    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
    void transformSymbol(
        const Base::Vector3d& point,
        const Base::Vector3d& normal,
        SbMatrix& mat
    ) const override;
    void transformExtraSymbol() const override;
};

}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTTransform_H
