#ifndef ROBOTGUI_TaskDlgEdge2Trac_H
#define ROBOTGUI_TaskDlgEdge2Trac_H

#include <Gui/TaskView/TaskDialog.h>
#include <Mod/Robot/App/Edge2TracObject.h>

#include <Mod/Robot/RobotGlobal.h>

#include "TaskEdge2TracParameter.h"


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
class RobotGuiExport TaskDlgEdge2Trac: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgEdge2Trac(Robot::Edge2TracObject*);

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
    TaskEdge2TracParameter* param;
    Gui::TaskView::TaskSelectLinkProperty* select;
    Robot::Edge2TracObject* Edge2TaskObject;
};


}  // namespace RobotGui

#endif  // ROBOTGUI_TASKDLGSIMULATE_H
