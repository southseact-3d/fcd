#ifndef FemGui_TaskDlgCreateElementSet_H
#define FemGui_TaskDlgCreateElementSet_H

#include <Gui/TaskView/TaskDialog.h>
#include <Mod/Fem/App/FemSetElementNodesObject.h>

#include "TaskCreateElementSet.h"
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
class TaskDlgCreateElementSet: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDlgCreateElementSet(Fem::FemSetElementNodesObject*);
    ~TaskDlgCreateElementSet() override;

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
    TaskCreateElementSet* param;
    TaskObjectName* name;

    Fem::FemSetElementNodesObject* FemSetElementNodesObject;
};


}  // namespace FemGui

#endif  // FemGui_TaskDlgCreateElementSet_H
