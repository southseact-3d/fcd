#include <QMenu>


#include "TaskPocketParameters.h"
#include "ViewProviderPocket.h"


using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderPocket, PartDesignGui::ViewProviderExtrude)

ViewProviderPocket::ViewProviderPocket()
{
    sPixmap = "PartDesign_Pocket.svg";
}

ViewProviderPocket::~ViewProviderPocket() = default;


void ViewProviderPocket::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Pocket"));
    PartDesignGui::ViewProviderSketchBased::setupContextMenu(menu, receiver, member);
}


TaskDlgFeatureParameters* ViewProviderPocket::getEditDialog()
{
    return new TaskDlgPocketParameters(this);
}
