#include "Workbench.h"
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>


using namespace CloudGui;

/// @namespace CloudGui @class Workbench
TYPESYSTEM_SOURCE(CloudGui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{}

Workbench::~Workbench()
{}
/*
Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem( "&Windows" );
    Gui::MenuItem* test = new Gui::MenuItem;
    root->insertItem( item, test );
    test->setCommand("Cloud");
    *test << "Cloud_Test";
    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* test = new Gui::ToolBarItem(root);
    test->setCommand( "Cloud Tools" );
    *test << "Cloud_Test";
    return root;
}
*/
