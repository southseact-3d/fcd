#include "WorkbenchManipulator.h"
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

using namespace MatGui;

void WorkbenchManipulator::modifyMenuBar([[maybe_unused]] Gui::MenuItem* menuBar)
{
    addCommands(menuBar, "Std_ToggleNavigation");
}

void WorkbenchManipulator::modifyContextMenu(const char* recipient, Gui::MenuItem* menuBar)
{
    if (strcmp(recipient, "View") == 0) {
        addCommands(menuBar, "Std_TreeSelection");
    }
    else if (strcmp(recipient, "Tree") == 0) {
        addCommandsToTree(menuBar);
    }
}

void WorkbenchManipulator::addCommands(Gui::MenuItem* menuBar, const char* reference)
{
    auto par = menuBar->findParentOf(reference);
    if (par) {
        auto item = par->findItem(reference);
        item = par->afterItem(item);

        auto cmd1 = new Gui::MenuItem();
        cmd1->setCommand("Std_SetMaterial");
        par->insertItem(item, cmd1);
        auto cmd2 = new Gui::MenuItem();
        cmd2->setCommand("Std_SetAppearance");
        par->insertItem(item, cmd2);
    }
}

void WorkbenchManipulator::addCommandsToTree(Gui::MenuItem* menuBar)
{
    const char* randomColor = "Std_RandomColor";
    auto par = menuBar->findParentOf(randomColor);
    if (par) {
        auto item = par->findItem(randomColor);

        auto cmd1 = new Gui::MenuItem();
        cmd1->setCommand("Std_SetMaterial");
        par->insertItem(item, cmd1);
        auto cmd2 = new Gui::MenuItem();
        cmd2->setCommand("Std_SetAppearance");
        par->insertItem(item, cmd2);
    }
}
