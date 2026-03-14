#include "WorkbenchManipulator.h"

using namespace Gui;

std::set<WorkbenchManipulator::Ptr> WorkbenchManipulator::manipulators;  // NOLINT

void WorkbenchManipulator::installManipulator(const WorkbenchManipulator::Ptr& ptr)
{
    manipulators.insert(ptr);
}

void WorkbenchManipulator::removeManipulator(const WorkbenchManipulator::Ptr& ptr)
{
    auto it = manipulators.find(ptr);
    if (it != manipulators.end()) {
        manipulators.erase(it);
    }
}

void WorkbenchManipulator::removeAll()
{
    manipulators.clear();
}

std::set<WorkbenchManipulator::Ptr> WorkbenchManipulator::getManipulators()
{
    return manipulators;
}

void WorkbenchManipulator::changeMenuBar(MenuItem* menuBar)
{
    for (auto& it : manipulators) {
        it->modifyMenuBar(menuBar);
    }
}

void WorkbenchManipulator::changeContextMenu(const char* recipient, MenuItem* menuBar)
{
    for (auto& it : manipulators) {
        it->modifyContextMenu(recipient, menuBar);
    }
}

void WorkbenchManipulator::changeToolBars(ToolBarItem* toolBar)
{
    for (auto& it : manipulators) {
        it->modifyToolBars(toolBar);
    }
}

void WorkbenchManipulator::changeDockWindows(DockWindowItems* dockWindow)
{
    for (auto& it : manipulators) {
        it->modifyDockWindows(dockWindow);
    }
}

void WorkbenchManipulator::modifyMenuBar([[maybe_unused]] MenuItem* menuBar)
{}

void WorkbenchManipulator::modifyContextMenu(
    [[maybe_unused]] const char* recipient,
    [[maybe_unused]] MenuItem* menuBar
)
{}

void WorkbenchManipulator::modifyToolBars([[maybe_unused]] ToolBarItem* toolBar)
{}

void WorkbenchManipulator::modifyDockWindows([[maybe_unused]] DockWindowItems* dockWindow)
{}
