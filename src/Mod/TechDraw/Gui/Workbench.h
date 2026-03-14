#ifndef TECHDRAW_WORKBENCH_H
#define TECHDRAW_WORKBENCH_H

#include <Mod/TechDraw/TechDrawGlobal.h>

#include <Gui/Workbench.h>

namespace TechDrawGui {

/**
 * @author Werner Mayer
 */
class TechDrawGuiExport Workbench : public Gui::StdWorkbench
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

} // namespace TechDrawGui


#endif // TECHDRAW_WORKBENCH_H
