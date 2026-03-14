#ifndef FEMGUI_TaskDlgAnalysis_H
#define FEMGUI_TaskDlgAnalysis_H

#include <Gui/TaskView/TaskDialog.h>


namespace Fem
{
class FemAnalysis;
}

namespace FemGui
{
class TaskAnalysisInfo;
class TaskDriver;

/// simulation dialog for the TaskView
class TaskDlgAnalysis: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgAnalysis(Fem::FemAnalysis*);
    ~TaskDlgAnalysis() override;

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
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply;
    }

protected:
    TaskAnalysisInfo* info;
    TaskDriver* driver;

    Fem::FemAnalysis* FemAnalysis;
};


}  // namespace FemGui

#endif  // FEMGUI_TaskDlgAnalysis_H
