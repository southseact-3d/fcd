#include <QMenu>


#include "TaskRevolutionParameters.h"
#include "ViewProviderGroove.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderGroove, PartDesignGui::ViewProviderSketchBased)

ViewProviderGroove::ViewProviderGroove()
{
    sPixmap = "PartDesign_Groove.svg";
}

ViewProviderGroove::~ViewProviderGroove() = default;

void ViewProviderGroove::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Groove"));
    PartDesignGui::ViewProviderSketchBased::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderGroove::getEditDialog()
{
    return new TaskDlgGrooveParameters(this);
}
