#ifndef GUI_TASKVIEW_TaskSketcherCreateCommands_H
#define GUI_TASKVIEW_TaskSketcherCreateCommands_H

#include <Gui/Selection/Selection.h>
#include <Gui/TaskView/TaskView.h>


namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
namespace TaskView
{


class TaskSketcherCreateCommands: public TaskBox, public Gui::SelectionSingleton::ObserverType
{
    Q_OBJECT

public:
    explicit TaskSketcherCreateCommands(QWidget* parent = nullptr);
    ~TaskSketcherCreateCommands() override;
    /// Observer message from the Selection
    void OnChange(
        Gui::SelectionSingleton::SubjectType& rCaller,
        Gui::SelectionSingleton::MessageType Reason
    ) override;

private Q_SLOTS:

protected:
    void changeEvent(QEvent* e) override;


private:
    // QWidget* proxy;
};

}  // namespace TaskView
}  // namespace Gui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
