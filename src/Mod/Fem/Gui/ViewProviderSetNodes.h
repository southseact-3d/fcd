#ifndef FEM_ViewProviderSetNodes_H
#define FEM_ViewProviderSetNodes_H

#include <Gui/ViewProviderGeometryObject.h>

namespace FemGui
{

class ViewProviderSetNodes: public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderSetNodes);

public:
    bool doubleClicked() override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // FEM_ViewProviderSetNodes_H
