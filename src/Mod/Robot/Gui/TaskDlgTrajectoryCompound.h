#ifndef ROBOTGUI_TaskDlgTrajectoryCompound_H
#define ROBOTGUI_TaskDlgTrajectoryCompound_H

#include <Gui/TaskView/TaskDialog.h>

#include <Mod/Robot/RobotGlobal.h>
#include <Mod/Robot/App/TrajectoryCompound.h>


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
class RobotGuiExport TaskDlgTrajectoryCompound: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDlgTrajectoryCompound(Robot::TrajectoryCompound*);

public:
    /// is called the TaskView when the dialog is opened
    void open() override;
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
    /// is called by the framework if the dialog is rejected (Cancel)
    bool reject() override;
    /// is called by the framework if the user press the help button
    void helpRequested() override;

    /// returns for Close and Help button
    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

protected:
    Gui::TaskView::TaskSelectLinkProperty* select;
    Robot::TrajectoryCompound* TrajectoryCompound;
};


}  // namespace RobotGui

#endif  // ROBOTGUI_TASKDLGSIMULATE_H
