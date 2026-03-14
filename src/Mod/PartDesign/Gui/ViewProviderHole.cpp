#include <QMenu>
#include <QMessageBox>


#include <Gui/Application.h>
#include <Gui/Control.h>
#include <Gui/Command.h>
#include <Gui/MainWindow.h>
#include <Mod/PartDesign/App/FeatureHole.h>
#include <Mod/Sketcher/App/SketchObject.h>

#include "ViewProviderHole.h"
#include "TaskHoleParameters.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderHole, PartDesignGui::ViewProvider)

ViewProviderHole::ViewProviderHole()
{
    sPixmap = "PartDesign_Hole.svg";
}

ViewProviderHole::~ViewProviderHole() = default;

std::vector<App::DocumentObject*> ViewProviderHole::claimChildren() const
{
    std::vector<App::DocumentObject*> temp;

    if (App::DocumentObject* profile = getObject<PartDesign::Hole>()->Profile.getValue();
        profile && !profile->isDerivedFrom<PartDesign::Feature>()) {
        temp.push_back(profile);
    }

    return temp;
}

void ViewProviderHole::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Hole"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderHole::getEditDialog()
{
    return new TaskDlgHoleParameters(this);
}
