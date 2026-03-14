#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTHEATFLUX_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTHEATFLUX_H

#include "ViewProviderFemConstraintOnBoundary.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintHeatflux
    : public FemGui::ViewProviderFemConstraintOnBoundary
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintHeatflux);

public:
    ViewProviderFemConstraintHeatflux();
    ~ViewProviderFemConstraintHeatflux() override;
    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTHEATFLUX_H
