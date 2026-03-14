#ifndef FEM_ViewProviderSetElementNodes_H
#define FEM_ViewProviderSetElementNodes_H

#include <Gui/ViewProviderGeometryObject.h>

namespace FemGui
{

class ViewProviderSetElementNodes: public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderSetElementNodes);

public:
    bool doubleClicked() override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // FEM_ViewProviderSetElementNodes_H
