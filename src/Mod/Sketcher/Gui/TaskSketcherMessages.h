#ifndef GUI_TASKVIEW_TaskSketcherMessages_H
#define GUI_TASKVIEW_TaskSketcherMessages_H

#include <Gui/TaskView/TaskSolverMessages.h>

namespace SketcherGui
{

class ViewProviderSketch;

class TaskSketcherMessages: public Gui::TaskSolverMessages
{
    Q_OBJECT

public:
    explicit TaskSketcherMessages(ViewProviderSketch* sketchView);
    ~TaskSketcherMessages() override;

private:
    void createSettingsButtonActions() override;
    void onLabelStatusLinkClicked(const QString&) override;

    void updateToolTip(const QString& link) override;

protected:
    ViewProviderSketch* sketchView;
};

}  // namespace SketcherGui

#endif  // GUI_TASKVIEW_TaskSketcherMessages_H
