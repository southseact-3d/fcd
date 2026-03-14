#ifndef MATGUI_WORKBENCHMANIPULATOR_H
#define MATGUI_WORKBENCHMANIPULATOR_H

#include <Gui/WorkbenchManipulator.h>

namespace MatGui {

class WorkbenchManipulator: public Gui::WorkbenchManipulator
{
protected:
    /*!
     * \brief modifyMenuBar
     * Adds the commands Std_SetMaterial and Std_SetAppearance to the View menu
     */
    void modifyMenuBar(Gui::MenuItem* menuBar) override;
    /*!
     * \brief modifyContextMenu
     * Adds the commands Std_SetMaterial and Std_SetAppearance to the contex-menu
     */
    void modifyContextMenu(const char* recipient, Gui::MenuItem* menuBar) override;

private:
    static void addCommands(Gui::MenuItem* menuBar, const char* reference);
    static void addCommandsToTree(Gui::MenuItem* menuBar);
};

} // namespace MatGui


#endif // MATGUI_WORKBENCHMANIPULATOR_H
