#include "TaskWatcher.h"


using namespace RobotGui;

//**************************************************************************
//**************************************************************************
// TaskWatcher
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskWatcherRobot::TaskWatcherRobot()
    : Gui::TaskView::TaskWatcher("SELECT Robot::RobotObject COUNT 1")
{
    rob = new TaskRobot6Axis(nullptr);
    ctr = new TaskRobotControl(nullptr);

    Content.push_back(rob);
    Content.push_back(ctr);
}

//==== calls from the TaskView ===============================================================

bool TaskWatcherRobot::shouldShow()
{
    if (match()) {
        rob->setRobot((Robot::RobotObject*)Result[0][0].getObject());
        ctr->setRobot((Robot::RobotObject*)Result[0][0].getObject());
        return true;
    }
    return false;
}


#include "moc_TaskWatcher.cpp"
