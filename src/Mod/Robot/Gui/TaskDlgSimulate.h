#ifndef ROBOTGUI_TASKDLGSIMULATE_H
#define ROBOTGUI_TASKDLGSIMULATE_H

#include <Gui/TaskView/TaskDialog.h>
#include <Mod/Robot/App/RobotObject.h>
#include <Mod/Robot/App/TrajectoryObject.h>
#include <Mod/Robot/RobotGlobal.h>

#include "TaskRobot6Axis.h"
#include "TaskRobotControl.h"
#include "TaskRobotMessages.h"
#include "TaskTrajectory.h"


namespace RobotGui
{

/// simulation dialog for the TaskView
class RobotGuiExport TaskDlgSimulate: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDlgSimulate(Robot::RobotObject* pcRobotObject, Robot::TrajectoryObject* pcTrajectoryObject);

public:
    /// is called the TaskView when the dialog is opened
    void open() override;
    /// is called by the framework if an button is clicked which has no accept or rject role
    void clicked(int) override;
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
    /// is called by the framework if the dialog is rejected (Cancel)
    bool reject() override;
    /// is called by the framework if the user press the help button
    void helpRequested() override;

    /// returns for Close and Help button
    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Close | QDialogButtonBox::Help;
    }

protected:
    TaskRobot6Axis* rob;
    TaskRobotControl* ctr;
    TaskTrajectory* trac;
    TaskRobotMessages* msg;
};


}  // namespace RobotGui

#endif  // ROBOTGUI_TASKDLGSIMULATE_H
