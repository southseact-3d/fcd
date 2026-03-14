#include <QMenu>


#include "TaskPadParameters.h"
#include "ViewProviderPad.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderPad, PartDesignGui::ViewProviderExtrude)

ViewProviderPad::ViewProviderPad()
{
    sPixmap = "PartDesign_Pad.svg";
}

ViewProviderPad::~ViewProviderPad() = default;

void ViewProviderPad::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Pad"));
    PartDesignGui::ViewProviderSketchBased::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderPad::getEditDialog()
{
    // TODO fix setting values from the history: now it doesn't work neither in
    //      the master and in the migrated branch  (2015-07-26, Fat-Zer)
    return new TaskDlgPadParameters(this);
}
