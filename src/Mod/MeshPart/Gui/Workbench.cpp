#include <qobject.h>


#include <Gui/ToolBarManager.h>

#include "Workbench.h"


using namespace MeshPartGui;

#if 0  // needed for Qt's lupdate utility
    qApp->translate("Workbench", "MeshPart");
#endif

/// @namespace MeshPartGui @class Workbench
TYPESYSTEM_SOURCE(MeshPartGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench() = default;

Workbench::~Workbench() = default;

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* part = new Gui::ToolBarItem(root);
    part->setCommand("MeshPart");
    *part << "MeshPart_Mesher";
    return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    // Part tools
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    return root;
}
