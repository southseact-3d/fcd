#ifndef MESH_WORKBENCH_H
#define MESH_WORKBENCH_H

#include <Gui/Workbench.h>
#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif


namespace MeshGui
{

/**
 * @author Werner Mayer
 */
class MeshGuiExport Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();

    void activated() override;
    void deactivated() override;
    void setupContextMenu(const char* recipient, Gui::MenuItem*) const override;

protected:
    Gui::MenuItem* setupMenuBar() const override;
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::ToolBarItem* setupCommandBars() const override;
};

}  // namespace MeshGui


#endif  // MESH_WORKBENCH_H
