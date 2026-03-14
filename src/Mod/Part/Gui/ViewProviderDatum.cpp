#include <App/Document.h>
#include <App/DocumentObjectGroup.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Control.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>

#include "ViewProviderDatum.h"


using namespace PartGui;

PROPERTY_SOURCE_WITH_EXTENSIONS(PartGui::ViewProviderLine, Gui::ViewProviderLine)

ViewProviderLine::ViewProviderLine()
{
    PartGui::ViewProviderAttachExtension::initExtension(this);
}

bool ViewProviderLine::doubleClicked()
{
    showAttachmentEditor();
    return true;
}

PROPERTY_SOURCE_WITH_EXTENSIONS(PartGui::ViewProviderPlane, Gui::ViewProviderPlane)

ViewProviderPlane::ViewProviderPlane()
{
    PartGui::ViewProviderAttachExtension::initExtension(this);
}

bool ViewProviderPlane::doubleClicked()
{
    showAttachmentEditor();
    return true;
}


PROPERTY_SOURCE_WITH_EXTENSIONS(PartGui::ViewProviderPoint, Gui::ViewProviderPoint)

ViewProviderPoint::ViewProviderPoint()
{
    PartGui::ViewProviderAttachExtension::initExtension(this);
}

bool ViewProviderPoint::doubleClicked()
{
    showAttachmentEditor();
    return true;
}


PROPERTY_SOURCE_WITH_EXTENSIONS(PartGui::ViewProviderLCS, Gui::ViewProviderCoordinateSystem)

ViewProviderLCS::ViewProviderLCS()
{
    PartGui::ViewProviderAttachExtension::initExtension(this);
}

bool ViewProviderLCS::doubleClicked()
{
    showAttachmentEditor();
    return true;
}
