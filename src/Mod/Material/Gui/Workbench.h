#ifndef MATGUI_WORKBENCH_H
#define MATGUI_WORKBENCH_H

#include <Gui/Workbench.h>
#include <Mod/Material/MaterialGlobal.h>

namespace MatGui
{

/**
 * @author David Carter
 */
class MatGuiExport Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;

protected:
    Gui::MenuItem* setupMenuBar() const override;
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::ToolBarItem* setupCommandBars() const override;
};

}  // namespace MatGui


#endif  // MATGUI_WORKBENCH_H
