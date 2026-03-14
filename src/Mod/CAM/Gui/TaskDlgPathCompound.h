#ifndef PATHGUI_TaskDlgPathCompound_H
#define PATHGUI_TaskDlgPathCompound_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/CAM/App/FeaturePathCompound.h>

#include "ViewProviderPathCompound.h"


class Ui_TaskDlgPathCompound;

namespace PathGui
{

/// Widget
class TaskWidgetPathCompound: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskWidgetPathCompound(ViewProviderPathCompound* CompoundView, QWidget* parent = nullptr);
    ~TaskWidgetPathCompound() override;

    std::vector<std::string> getList() const;

protected:
    void changeEvent(QEvent* e) override;

private:
    QWidget* proxy;
    Ui_TaskDlgPathCompound* ui;
};

/// Task Dialog
class PathGuiExport TaskDlgPathCompound: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDlgPathCompound(PathGui::ViewProviderPathCompound*);
    ~TaskDlgPathCompound() override;

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
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

protected:
    PathGui::ViewProviderPathCompound* CompoundView;
    TaskWidgetPathCompound* parameter;
};


}  // namespace PathGui

#endif  // PATHGUI_TASKDLGPATHCOMPOUND_H
