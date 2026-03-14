#ifndef ROBOT_WORKBENCH_H
#define ROBOT_WORKBENCH_H

#include <Gui/TaskView/TaskWatcher.h>
#include <Gui/Workbench.h>
#include <Mod/Robot/RobotGlobal.h>


namespace RobotGui
{

/**
 * @author Werner Mayer
 */
class RobotGuiExport Workbench: public Gui::StdWorkbench
{
    TYPESYSTEM_HEADER_WITH_OVERRIDE();

public:
    Workbench();
    ~Workbench() override;

    /** Run some actions when the workbench gets activated. */
    void activated() override;
    /** Run some actions when the workbench gets deactivated. */
    void deactivated() override;


protected:
    Gui::ToolBarItem* setupToolBars() const override;
    Gui::MenuItem* setupMenuBar() const override;

    std::vector<Gui::TaskView::TaskWatcher*> Watcher;
};

}  // namespace RobotGui


#endif  // ROBOT_WORKBENCH_H
