#include "ViewProviderFemMeshShape.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderFemMeshShapeBase, FemGui::ViewProviderFemMesh)

ViewProviderFemMeshShapeBase::ViewProviderFemMeshShapeBase() = default;

ViewProviderFemMeshShapeBase::~ViewProviderFemMeshShapeBase() = default;

// ------------------------------------------------------------------------

PROPERTY_SOURCE(FemGui::ViewProviderFemMeshShape, FemGui::ViewProviderFemMeshShapeBase)

ViewProviderFemMeshShape::ViewProviderFemMeshShape() = default;

ViewProviderFemMeshShape::~ViewProviderFemMeshShape() = default;


// Python feature ---------------------------------------------------------

namespace Gui
{

PROPERTY_SOURCE_TEMPLATE(FemGui::ViewProviderFemMeshShapeBasePython, FemGui::ViewProviderFemMeshShapeBase)

// explicit template instantiation
template class FemGuiExport ViewProviderFeaturePythonT<ViewProviderFemMeshShapeBase>;

}  // namespace Gui
