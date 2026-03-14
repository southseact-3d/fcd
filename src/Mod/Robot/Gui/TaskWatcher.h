#ifndef ROBOTGUI_TASKWATCHER_H
#define ROBOTGUI_TASKWATCHER_H

#include <Gui/TaskView/TaskWatcher.h>

#include <Mod/Robot/RobotGlobal.h>

#include "TaskRobot6Axis.h"
#include "TaskRobotControl.h"


namespace RobotGui
{

class TaskContent;

/// Father class of watcher classes
class RobotGuiExport TaskWatcherRobot: public Gui::TaskView::TaskWatcher
{
    Q_OBJECT

public:
    TaskWatcherRobot();

    /// is called when the document or the selection changes.
    bool shouldShow() override;

protected:
    TaskRobot6Axis* rob;
    TaskRobotControl* ctr;
};


}  // namespace RobotGui

#endif  // ROBOTGUI_TASKWATCHER_H
