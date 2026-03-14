#ifndef GUI_VIEWPROVIDERFEMCONSTRAINTRIGIDBODY_H
#define GUI_VIEWPROVIDERFEMCONSTRAINTRIGIDBODY_H

#include "ViewProviderFemConstraintOnBoundary.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemConstraintRigidBody
    : public FemGui::ViewProviderFemConstraintOnBoundary
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemConstraintRigidBody);

public:
    /// Constructor
    ViewProviderFemConstraintRigidBody();
    ~ViewProviderFemConstraintRigidBody() override;

    void updateData(const App::Property*) override;

protected:
    bool setEdit(int ModNum) override;

    void transformExtraSymbol() const override;
};

}  // namespace FemGui


#endif  // GUI_VIEWPROVIDERFEMCONSTRAINTRIGIDBODY_H
