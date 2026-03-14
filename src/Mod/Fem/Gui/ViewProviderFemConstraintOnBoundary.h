#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTONBOUNDARY_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTONBOUNDARY_H

#include "ViewProviderFemConstraint.h"


namespace Part
{
class Feature;
}

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintOnBoundary: public FemGui::ViewProviderFemConstraint
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintOnBoundary);

public:
    ViewProviderFemConstraintOnBoundary();
    ~ViewProviderFemConstraintOnBoundary() override;

    void highlightReferences(const bool on) override;

private:
    std::map<Part::Feature*, std::vector<Base::Color>> originalPointColors;
    std::map<Part::Feature*, std::vector<Base::Color>> originalLineColors;
    std::map<Part::Feature*, std::vector<Base::Color>> originalFaceColors;
};

}  // namespace FemGui

#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTONBOUNDARY_H
