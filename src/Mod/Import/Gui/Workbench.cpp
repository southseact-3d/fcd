#include <Gui/ToolBarManager.h>

#include "Workbench.h"

using namespace ImportGui;

/// @namespace ImportGui @class Workbench
TYPESYSTEM_SOURCE(ImportGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench() = default;

Workbench::~Workbench() = default;

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    return root;
}

Gui::ToolBarItem* Workbench::setupCommandBars() const
{
    Gui::ToolBarItem* root = new Gui::ToolBarItem;
    return root;
}
