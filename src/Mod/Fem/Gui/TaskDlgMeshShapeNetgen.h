#ifndef FEMGUI_TaskDlgMeshShapeNetgen_H
#define FEMGUI_TaskDlgMeshShapeNetgen_H

#include <Gui/TaskView/TaskDialog.h>

namespace Fem
{
class FemMeshShapeNetgenObject;
}


namespace FemGui
{

class TaskTetParameter;
class ViewProviderFemMeshShapeNetgen;

/// simulation dialog for the TaskView
class TaskDlgMeshShapeNetgen: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgMeshShapeNetgen(FemGui::ViewProviderFemMeshShapeNetgen*);
    ~TaskDlgMeshShapeNetgen() override;

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
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply;
    }

protected:
    TaskTetParameter* param;

    Fem::FemMeshShapeNetgenObject* FemMeshShapeNetgenObject;
    FemGui::ViewProviderFemMeshShapeNetgen* ViewProviderFemMeshShapeNetgen;
};


}  // namespace FemGui

#endif  // FEMGUI_TaskDlgMeshShapeNetgen_H
