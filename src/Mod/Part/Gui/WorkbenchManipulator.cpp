#include "WorkbenchManipulator.h"
#include <Gui/MenuManager.h>
#include <Gui/ToolBarManager.h>

using namespace PartGui;

void WorkbenchManipulator::modifyMenuBar([[maybe_unused]] Gui::MenuItem* menuBar)
{}

void WorkbenchManipulator::modifyToolBars(Gui::ToolBarItem* toolBar)
{
    addSelectionFilter(toolBar);
    addDatums(toolBar);
}

void WorkbenchManipulator::modifyDockWindows([[maybe_unused]] Gui::DockWindowItems* dockWindow)
{}

void WorkbenchManipulator::addSelectionFilter(Gui::ToolBarItem* toolBar)
{
    if (auto view = toolBar->findItem("View")) {
        auto add = new Gui::ToolBarItem();  // NOLINT
        add->setCommand("Part_SelectFilter");
        auto item = view->findItem("Std_TreeViewActions");
        if (item) {
            view->insertItem(item, add);
        }
        else {
            view->appendItem(add);
        }
    }
}

void WorkbenchManipulator::addDatums(Gui::ToolBarItem* toolBar)
{
    if (auto view = toolBar->findItem("Structure")) {
        auto add = new Gui::ToolBarItem();  // NOLINT
        add->setCommand("Part_Datums");
        auto item = view->findItem("Std_Group");
        if (item) {
            view->insertItem(item, add);
        }
        else {
            view->appendItem(add);
        }
    }
}
