#ifndef GUI_TASKDLGRELOCATE_H
#define GUI_TASKDLGRELOCATE_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

#include <FCGlobal.h>

class Ui_Position;
class Ui_Angle;

namespace Gui
{

class TaskBoxPosition: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskBoxPosition(QWidget* parent = nullptr);
    ~TaskBoxPosition() override;


private Q_SLOTS:

protected:
private:
    QWidget* proxy;
    Ui_Position* ui;
};

class TaskBoxAngle: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskBoxAngle(QWidget* parent = nullptr);
    ~TaskBoxAngle() override;


private Q_SLOTS:

protected:
private:
    QWidget* proxy;
    Ui_Angle* ui;
};


///
class GuiExport TaskDlgRelocation: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDlgRelocation();
    ~TaskDlgRelocation() override;

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
};


}  // namespace Gui

#endif  // ROBOTGUI_TASKDLGSIMULATE_H
