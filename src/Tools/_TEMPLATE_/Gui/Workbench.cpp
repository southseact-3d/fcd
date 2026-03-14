#include "Workbench.h"
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

using namespace _TEMPLATE_Gui;

/// @namespace _TEMPLATE_Gui @class Workbench
TYPESYSTEM_SOURCE(_TEMPLATE_Gui::Workbench, Gui::StdWorkbench)

Workbench::Workbench()
{}

Workbench::~Workbench()
{}

Gui::MenuItem* Workbench::setupMenuBar() const
{
    Gui::MenuItem* root = StdWorkbench::setupMenuBar();
    Gui::MenuItem* item = root->findItem("&Windows");
    Gui::MenuItem* test = new Gui::MenuItem;
    root->insertItem(item, test);
    test->setCommand("_TEMPLATE_");
    *test << "_TEMPLATE__Test";
    return root;
}

Gui::ToolBarItem* Workbench::setupToolBars() const
{
    Gui::ToolBarItem* root = StdWorkbench::setupToolBars();
    Gui::ToolBarItem* test = new Gui::ToolBarItem(root);
    test->setCommand("_TEMPLATE_ Tools");
    *test << "_TEMPLATE__Test";
    return root;
}
