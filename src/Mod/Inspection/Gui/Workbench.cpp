#include "Workbench.h"

#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>


using namespace InspectionGui;

/// @namespace InspectionGui @class Workbench
TYPESYSTEM_SOURCE(InspectionGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench() = default;

Workbench::~Workbench() = default;

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");
    Gui::MenuItem* insp = new Gui::MenuItem;
    root->insertItem(item, insp);
    insp->setCommand("Inspection");
    *insp << "Inspection_VisualInspection"
          << "Inspection_InspectElement";
    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    // Gui::ToolBarItem* insp = new Gui::ToolBarItem(root);
    // insp->setCommand( "Inspection Tools" );
    //*insp << "Inspection_VisualInspection";
    return root;
}
