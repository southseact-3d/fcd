#ifndef GUI_CONTROL_H
#define GUI_CONTROL_H

// Std. configurations

#include <QObject>
#include <bitset>
#include <stack>

#include <Gui/TaskView/TaskDialog.h>

class QDockWidget;
class QTabBar;

namespace App
{
class DocumentObject;
class Document;
}  // namespace App

namespace Gui
{
namespace TaskView
{
class TaskDialog;
class TaskView;
}  // namespace TaskView


/** The control class
 */
class GuiExport ControlSingleton: public QObject
{
    Q_OBJECT

public:
    static ControlSingleton& instance();
    static void destruct();

    /** @name dialog handling
     *  These methods are used to control the TaskDialog stuff.
     */
    //@{
    /// This method starts a task dialog in the task view
    void showDialog(Gui::TaskView::TaskDialog* dlg);
    Gui::TaskView::TaskDialog* activeDialog() const;
    // void closeDialog();
    //@}

    /** @name task view handling
     */
    //@{
    Gui::TaskView::TaskView* taskPanel() const;
    /// raising the model view
    void showModelView();
    //@}

    /*!
      If a task dialog is open then it indicates whether this task dialog allows other commands to
      modify the document while it is open. If no task dialog is open true is returned.
     */
    bool isAllowedAlterDocument() const;
    /*!
      If a task dialog is open then it indicates whether this task dialog allows other commands to
      modify the 3d view while it is open. If no task dialog is open true is returned.
     */
    bool isAllowedAlterView() const;
    /*!
      If a task dialog is open then it indicates whether this task dialog allows other commands to
      modify the selection while it is open. If no task dialog is open true is returned.
     */
    bool isAllowedAlterSelection() const;

public Q_SLOTS:
    void accept();
    void reject();
    void closeDialog();
    /// raises the task view panel
    void showTaskView();

private Q_SLOTS:
    /// This get called by the TaskView when the Dialog is finished
    void closedDialog();

private:
    struct status
    {
        std::bitset<32> StatusBits;
    } CurrentStatus;

    std::stack<status> StatusStack;

    Gui::TaskView::TaskDialog* ActiveDialog;
    int oldTabIndex;

private:
    /// Construction
    ControlSingleton();
    /// Destruction
    ~ControlSingleton() override;
    void showDockWidget(QWidget*);
    QTabBar* findTabBar(QDockWidget*) const;
    void aboutToShowDialog(QDockWidget* widget);
    void aboutToHideDialog(QDockWidget* widget);

    static ControlSingleton* _pcSingleton;
};

/// Get the global instance
inline ControlSingleton& Control()
{
    return ControlSingleton::instance();
}

}  // namespace Gui

#endif  // GUI_CONTROL_H
