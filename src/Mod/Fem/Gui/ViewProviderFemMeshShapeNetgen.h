#ifndef FEM_ViewProviderFemMeshShapeNetgen_H
#define FEM_ViewProviderFemMeshShapeNetgen_H

#include "ViewProviderFemMeshShape.h"

namespace FemGui
{

class FemGuiExport ViewProviderFemMeshShapeNetgen: public ViewProviderFemMeshShape
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemMeshShapeNetgen);

public:
    /// constructor.
    ViewProviderFemMeshShapeNetgen();

    /// destructor.
    ~ViewProviderFemMeshShapeNetgen() override;

    void updateData(const App::Property*) override;

protected:
    void setupContextMenu(QMenu* menu, QObject* receiver, const char* member) override;
    bool setEdit(int ModNum) override;
};

}  // namespace FemGui


#endif  // FEM_ViewProviderFemMeshShapeNetgen_H
