#include <memory>


#include <App/VarSet.h>

#include "MainWindow.h"
#include "ViewProviderVarSet.h"

using namespace Gui;
using namespace Gui::Dialog;

PROPERTY_SOURCE(Gui::ViewProviderVarSet, Gui::ViewProviderDocumentObject)

ViewProviderVarSet::ViewProviderVarSet()
{
    setToggleVisibility(ToggleVisibilityMode::NoToggleVisibility);
    sPixmap = "VarSet";
}

bool ViewProviderVarSet::doubleClicked()
{
    if (!dialog) {
        dialog = std::make_unique<DlgAddProperty>(getMainWindow(), this);
    }

    // Do not use exec() here because it blocks and prevents command Std_VarSet
    // to commit the autotransaction.  This in turn prevents the dialog to
    // handle transactions well.
    dialog->setWindowModality(Qt::ApplicationModal);
    dialog->show();
    dialog->raise();
    dialog->activateWindow();

    return true;
}

void ViewProviderVarSet::onFinished(int /*result*/)
{
    dialog = nullptr;
}
