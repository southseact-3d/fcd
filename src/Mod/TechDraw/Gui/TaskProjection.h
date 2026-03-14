//this file originally part of Drawing workbench
//migrated to TechDraw workbench 2022-01-26 by Wandererfan

#ifndef TECHDRAWGUI_TASKDIALOG
#define TECHDRAWGUI_TASKDIALOG

#include <QWidget>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/TechDraw/TechDrawGlobal.h>


class QCheckBox;

namespace TechDrawGui
{

/**
 * Embed the panel into a task dialog.
 */
class Ui_TaskProjection;
class TaskProjection : public QWidget
{
    Q_OBJECT

public:
    TaskProjection();
    ~TaskProjection() override;

public:
    bool accept();
    bool reject();

    virtual bool isAllowedAlterDocument() const
    { return true; }

private:
    std::unique_ptr<Ui_TaskProjection> ui;

};


class TaskDlgProjection : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDlgProjection();
    ~TaskDlgProjection() override;

public:
    /// is called the TaskView when the dialog is opened
    void open() override;
    /// is called by the framework if a button is clicked which has no accept or reject role
    void clicked(int) override;
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
    /// is called by the framework if the dialog is rejected (Cancel)
    bool reject() override;
    /// is called by the framework if the user presses the help button
    void helpRequested() override { return;}

    void update();

protected:

private:
    TaskProjection* widget;
    Gui::TaskView::TaskBox* taskbox;
};

} //namespace TechDrawGui



#endif // TECHDRAWGUI_TASKDIALOG
