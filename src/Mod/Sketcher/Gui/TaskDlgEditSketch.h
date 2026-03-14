#ifndef SKETCHERGUI_TaskDlgEditSketch_H
#define SKETCHERGUI_TaskDlgEditSketch_H

#include <fastsignals/signal.h>

#include <Gui/TaskView/TaskDialog.h>

#include "TaskSketcherConstraints.h"
#include "TaskSketcherElements.h"
#include "TaskSketcherMessages.h"
#include "TaskSketcherSolverAdvanced.h"
#include "TaskSketcherTool.h"
#include "ViewProviderSketch.h"


using Connection = fastsignals::connection;

namespace SketcherGui
{


/// simulation dialog for the TaskView
class SketcherGuiExport TaskDlgEditSketch: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgEditSketch(ViewProviderSketch* sketchView);
    ~TaskDlgEditSketch() override;
    ViewProviderSketch* getSketchView() const
    {
        return sketchView;
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
    bool isAllowedAlterDocument() const override
    {
        return false;
    }
    void autoClosedOnClosedView() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override;

    /** @brief Function used to register a slot to be triggered when the tool widget is changed. */
    template<typename F>
    fastsignals::connection registerToolWidgetChanged(F&& f)
    {
        return ToolSettings->registerToolWidgetChanged(std::forward<F>(f));
    }

protected:
    void slotUndoDocument(const App::Document&);
    void slotRedoDocument(const App::Document&);

private:
    void slotToolChanged(const std::string& toolname);

protected:
    ViewProviderSketch* sketchView;
    TaskSketcherConstraints* Constraints;
    TaskSketcherElements* Elements;
    TaskSketcherMessages* Messages;
    TaskSketcherSolverAdvanced* SolverAdvanced;
    TaskSketcherTool* ToolSettings;

private:
    Connection connectionToolSettings;
};


}  // namespace SketcherGui

#endif  // SKETCHERGUI_TaskDlgEditSketch_H
