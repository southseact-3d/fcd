#include "ViewProviderResult.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderResult, Gui::ViewProviderDocumentObject)

ViewProviderResult::ViewProviderResult()
{
    sPixmap = "FEM_ResultShow";
}

ViewProviderResult::~ViewProviderResult() = default;


// Python feature -----------------------------------------------------------------------

namespace Gui
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(FemGui::ViewProviderResultPython, FemGui::ViewProviderResult)
/// @endcond

// explicit template instantiation
template class FemGuiExport ViewProviderFeaturePythonT<ViewProviderResult>;
}  // namespace Gui
