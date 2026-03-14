#ifndef POINTS_WORKBENCH_H
#define POINTS_WORKBENCH_H

#include <Gui/Workbench.h>
#include <Mod/Points/PointsGlobal.h>

namespace PointsGui
{

/**
 * @author Werner Mayer
 */
class PointsGuiExport Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;

protected:
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::ToolBarItem* setupCommandBars() const override;
    Gui::MenuItem* setupMenuBar() const override;
};

}  // namespace PointsGui


#endif  // POINTS_WORKBENCH_H
