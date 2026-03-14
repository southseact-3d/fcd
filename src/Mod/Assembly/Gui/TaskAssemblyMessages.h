#ifndef GUI_TASKVIEW_TaskAssemblyMessages_H
#define GUI_TASKVIEW_TaskAssemblyMessages_H

#include <Gui/TaskView/TaskSolverMessages.h>


namespace AssemblyGui
{

class ViewProviderAssembly;

class TaskAssemblyMessages: public Gui::TaskSolverMessages
{
    Q_OBJECT

public:
    explicit TaskAssemblyMessages(ViewProviderAssembly* vp);
    ~TaskAssemblyMessages() override;

private:
    void onLabelStatusLinkClicked(const QString&) override;

    void updateToolTip(const QString& link) override;

protected:
    ViewProviderAssembly* vp;
};

}  // namespace AssemblyGui

#endif  // GUI_TASKVIEW_TaskAssemblyMessages_H
