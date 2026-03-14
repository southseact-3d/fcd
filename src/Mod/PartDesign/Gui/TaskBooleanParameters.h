#ifndef GUI_TASKVIEW_TaskBooleanParameters_H
#define GUI_TASKVIEW_TaskBooleanParameters_H

#include "TaskFeatureParameters.h"


#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

#include "ViewProviderBoolean.h"


class Ui_TaskBooleanParameters;

namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
}


namespace PartDesignGui
{

class TaskBooleanParameters: public Gui::TaskView::TaskBox, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit TaskBooleanParameters(ViewProviderBoolean* BooleanView, QWidget* parent = nullptr);
    ~TaskBooleanParameters() override;

    const std::vector<std::string> getBodies() const;
    int getType() const;

private Q_SLOTS:
    void onButtonBodyAdd(const bool checked);
    void onButtonBodyRemove(const bool checked);
    void onBodyDeleted();
    void onTypeChanged(int index);

protected:
    void exitSelectionMode();

protected:
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskBooleanParameters> ui;
    ViewProviderBoolean* BooleanView;

    enum selectionModes
    {
        none,
        bodyAdd,
        bodyRemove
    };
    selectionModes selectionMode;
};

/// simulation dialog for the TaskView
class TaskDlgBooleanParameters: public TaskDlgFeatureParameters
{
    Q_OBJECT

public:
    explicit TaskDlgBooleanParameters(ViewProviderBoolean* BooleanView);
    ~TaskDlgBooleanParameters() override;

    ViewProviderBoolean* getBooleanView() const
    {
        return BooleanView;
    }


public:
    /// is called the TaskView when the dialog is opened
    void open() override;
    /// is called by the framework if an button is clicked which has no accept or reject role
    void clicked(int) override;
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
    /// is called by the framework if the dialog is rejected (Cancel)
    bool reject() override;
    /// is called by the framework if the user presses the help button
    bool isAllowedAlterDocument() const override
    {
        return false;
    }

    /// returns for Close and Help button
    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel;
    }

protected:
    ViewProviderBoolean* BooleanView;

    TaskBooleanParameters* parameter;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
