#include <QMenu>


#include "TaskRevolutionParameters.h"
#include "ViewProviderRevolution.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderRevolution, PartDesignGui::ViewProviderSketchBased)

ViewProviderRevolution::ViewProviderRevolution()
{
    sPixmap = "PartDesign_Revolution.svg";
}

ViewProviderRevolution::~ViewProviderRevolution() = default;

void ViewProviderRevolution::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Revolution"));
    PartDesignGui::ViewProviderSketchBased::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderRevolution::getEditDialog()
{
    return new TaskDlgRevolutionParameters(this);
}
