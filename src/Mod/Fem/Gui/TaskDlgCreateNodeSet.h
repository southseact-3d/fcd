#ifndef ROBOTGUI_TaskDlgCreateNodeSet_H
#define ROBOTGUI_TaskDlgCreateNodeSet_H

#include <Gui/TaskView/TaskDialog.h>
#include <Mod/Fem/App/FemSetNodesObject.h>

#include "TaskCreateNodeSet.h"
#include "TaskObjectName.h"


// forward
namespace Gui
{
namespace TaskView
{
class TaskSelectLinkProperty;
}
}  // namespace Gui


namespace FemGui
{


/// simulation dialog for the TaskView
class TaskDlgCreateNodeSet: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgCreateNodeSet(Fem::FemSetNodesObject*);
    ~TaskDlgCreateNodeSet() override;

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
    TaskCreateNodeSet* param;
    TaskObjectName* name;

    Fem::FemSetNodesObject* FemSetNodesObject;
};


}  // namespace FemGui

#endif  // ROBOTGUI_TASKDLGSIMULATE_H
