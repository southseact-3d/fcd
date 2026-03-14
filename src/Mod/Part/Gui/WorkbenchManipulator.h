#ifndef PARTGUI_WORKBENCHMANIPULATOR_H
#define PARTGUI_WORKBENCHMANIPULATOR_H

#include <Gui/WorkbenchManipulator.h>

namespace PartGui
{

class WorkbenchManipulator: public Gui::WorkbenchManipulator
{
protected:
    /*!
     * \brief modifyMenuBar
     * Method to manipulate the menu structure of a workbench.
     * The default implementation doesn't change anything.
     */
    void modifyMenuBar(Gui::MenuItem* menuBar) override;
    /*!
     * \brief modifyToolBars
     * Method to manipulate the toolbar structure of a workbench
     * The default implementation doesn't change anything.
     */
    void modifyToolBars([[maybe_unused]] Gui::ToolBarItem* toolBar) override;
    /*!
     * \brief modifyDockWindows
     * Method to manipulate the dock window structure of a workbench
     * The default implementation doesn't change anything.
     */
    void modifyDockWindows([[maybe_unused]] Gui::DockWindowItems* dockWindow) override;

private:
    static void addSelectionFilter(Gui::ToolBarItem* toolBar);
    static void addDatums(Gui::ToolBarItem* toolBar);
};

}  // namespace PartGui


#endif  // PARTGUI_WORKBENCHMANIPULATOR_H
