#include "TaskDlgSimulate.h"


using namespace RobotGui;

//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgSimulate::TaskDlgSimulate(
    Robot::RobotObject* pcRobotObject,
    Robot::TrajectoryObject* pcTrajectoryObject
)
    : TaskDialog()
{
    rob = new TaskRobot6Axis(pcRobotObject);
    ctr = new TaskRobotControl(pcRobotObject);

    trac = new TaskTrajectory(pcRobotObject, pcTrajectoryObject);
    msg = new TaskRobotMessages(pcRobotObject);

    QObject::connect(trac, &TaskTrajectory::axisChanged, rob, &TaskRobot6Axis::setAxis);

    Content.push_back(rob);
    Content.push_back(ctr);
    Content.push_back(trac);
    Content.push_back(msg);
}

//==== calls from the TaskView ===============================================================


void TaskDlgSimulate::open()
{
    msg->hideGroupBox();
    ctr->hideGroupBox();
}

void TaskDlgSimulate::clicked(int)
{}

bool TaskDlgSimulate::accept()
{
    return true;
}

bool TaskDlgSimulate::reject()
{
    return true;
}

void TaskDlgSimulate::helpRequested()
{}


#include "moc_TaskDlgSimulate.cpp"
