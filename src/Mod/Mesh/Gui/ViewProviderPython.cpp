#include "ViewProviderPython.h"
// clang-format on

namespace Gui
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(MeshGui::ViewProviderPython, MeshGui::ViewProviderMeshFaceSet)
/// @endcond

// explicit template instantiation
template class MeshGuiExport ViewProviderFeaturePythonT<MeshGui::ViewProviderMeshFaceSet>;
}  // namespace Gui
