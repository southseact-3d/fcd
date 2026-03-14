#ifndef FEM_ViewProviderFemMeshShape_H
#define FEM_ViewProviderFemMeshShape_H

#include <Gui/ViewProviderFeaturePython.h>

#include "ViewProviderFemMesh.h"


namespace FemGui
{

class FemGuiExport ViewProviderFemMeshShapeBase: public ViewProviderFemMesh
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemMeshShapeBase);

public:
    /// constructor.
    ViewProviderFemMeshShapeBase();

    /// destructor.
    ~ViewProviderFemMeshShapeBase() override;
};


class FemGuiExport ViewProviderFemMeshShape: public ViewProviderFemMeshShapeBase
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemMeshShape);

public:
    /// constructor.
    ViewProviderFemMeshShape();

    /// destructor.
    ~ViewProviderFemMeshShape() override;
};

using ViewProviderFemMeshShapeBasePython
    = Gui::ViewProviderFeaturePythonT<ViewProviderFemMeshShapeBase>;

}  // namespace FemGui


#endif  // FEM_ViewProviderFemMeshShape_H
