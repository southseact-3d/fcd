#ifndef ROBOTGUI_TaskDlgTrajectoryDressUp_H
#define ROBOTGUI_TaskDlgTrajectoryDressUp_H

#include <Gui/TaskView/TaskDialog.h>
#include <Mod/Robot/App/TrajectoryDressUpObject.h>

#include <Mod/Robot/RobotGlobal.h>

#include "TaskTrajectoryDressUpParameter.h"


// forward
namespace Gui
{
namespace TaskView
{
class TaskSelectLinkProperty;
}
}  // namespace Gui

namespace RobotGui
{

/// simulation dialog for the TaskView
class RobotGuiExport TaskDlgTrajectoryDressUp: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgTrajectoryDressUp(Robot::TrajectoryDressUpObject*);

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
        return QDialogButtonBox::Ok | QDialogButtonBox::Apply | QDialogButtonBox::Cancel;
    }

protected:
    TaskTrajectoryDressUpParameter* param;
    Robot::TrajectoryDressUpObject* pcObject;
};


}  // namespace RobotGui

#endif  // ROBOTGUI_TASKDLGSIMULATE_H
