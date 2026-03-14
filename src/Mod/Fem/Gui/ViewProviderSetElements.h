#ifndef FEM_ViewProviderSetElements_H
#define FEM_ViewProviderSetElements_H

#include <Gui/ViewProviderGeometryObject.h>

namespace FemGui
{

class ViewProviderSetElements: public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderSetElements);

public:
    bool doubleClicked() override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // FEM_ViewProviderSetElements_H
