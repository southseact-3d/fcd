#ifndef GUI_TASKVIEW_TaskSketcherTool_H
#define GUI_TASKVIEW_TaskSketcherTool_H

#include <Gui/TaskView/TaskView.h>
#include <Gui/TaskView/TaskDialog.h>
#include <Gui/Selection/Selection.h>
#include <fastsignals/signal.h>

namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
}

namespace SketcherGui
{

class ViewProviderSketch;


class TaskSketcherTool: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    TaskSketcherTool(ViewProviderSketch* sketchView);
    ~TaskSketcherTool();

    QWidget* getWidget()
    {
        return widget.get();
    }

    void toolChanged(const std::string& toolname);

    template<typename F>
    fastsignals::connection registerToolWidgetChanged(F&& f)
    {
        return signalToolWidgetChanged.connect(std::forward<F>(f));
    }

private:
    ViewProviderSketch* sketchView;
    std::unique_ptr<QWidget> widget;
    fastsignals::scoped_connection changedSketchView;

    fastsignals::signal<void(QWidget* newwidget)> signalToolWidgetChanged;
};

}  // namespace SketcherGui

#endif  // GUI_TASKVIEW_TaskSketcherTool_H
