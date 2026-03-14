#include <QApplication>
#include <QMessageBox>
#include <QTextStream>


#include <Gui/Document.h>
#include <Gui/MainWindow.h>

#include "ViewProviderAnalysis.h"
#include "ViewProviderSolver.h"


using namespace FemGui;

PROPERTY_SOURCE(FemGui::ViewProviderSolver, Gui::ViewProviderDocumentObject)

ViewProviderSolver::ViewProviderSolver()
{
    setToggleVisibility(ToggleVisibilityMode::NoToggleVisibility);
    sPixmap = "FEM_SolverStandard";
}

ViewProviderSolver::~ViewProviderSolver() = default;

bool ViewProviderSolver::onDelete(const std::vector<std::string>&)
{
    // warn the user if the object has unselected children
    auto objs = claimChildren();
    return ViewProviderFemAnalysis::checkSelectedChildren(objs, this->getDocument(), "solver");
}

bool ViewProviderSolver::canDelete(App::DocumentObject* obj) const
{
    // deletions of objects from a FemSolver don't necessarily destroy anything
    // thus we can pass this action
    // we can warn the user if necessary in the object's ViewProvider in the onDelete() function
    Q_UNUSED(obj)
    return true;
}


// Python feature -----------------------------------------------------------------------

namespace Gui
{
/// @cond DOXERR
PROPERTY_SOURCE_TEMPLATE(FemGui::ViewProviderSolverPython, FemGui::ViewProviderSolver)
/// @endcond

// explicit template instantiation
template class FemGuiExport ViewProviderFeaturePythonT<ViewProviderSolver>;
}  // namespace Gui
