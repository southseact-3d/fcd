#ifndef FEM_ViewProviderSetGeometry_H
#define FEM_ViewProviderSetGeometry_H

#include <Gui/ViewProviderGeometryObject.h>

namespace FemGui
{

class ViewProviderSetGeometry: public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderSetGeometry);

public:
    bool doubleClicked() override;

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // FEM_ViewProviderSetGeometry_H
