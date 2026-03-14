#ifndef FEM_WORKBENCH_H
#define FEM_WORKBENCH_H

#include <Gui/Workbench.h>
#include <Mod/Fem/FemGlobal.h>

namespace FemGui
{

/**
 * @author Werner Mayer
 */
class FemGuiExport Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;
    void setupContextMenu(const char* recipient, Gui::MenuItem*) const override;

protected:
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::MenuItem* setupMenuBar() const override;
};

}  // namespace FemGui


#endif  // FEM_WORKBENCH_H
