#ifndef GUI_TASKVIEW_TaskSketcherSolverAdvanced_H
#define GUI_TASKVIEW_TaskSketcherSolverAdvanced_H

#include <Gui/TaskView/TaskView.h>


class Ui_TaskSketcherSolverAdvanced;

namespace App
{
class Property;
}

namespace SketcherGui
{

class ViewProviderSketch;

class TaskSketcherSolverAdvanced: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskSketcherSolverAdvanced(ViewProviderSketch* sketchView);
    ~TaskSketcherSolverAdvanced() override;

private:
    void setupConnections();
    void onComboBoxDefaultSolverCurrentIndexChanged(int index);
    void onComboBoxDogLegGaussStepCurrentIndexChanged(int index);
    void onSpinBoxMaxIterValueChanged(int i);
    void onSpinBoxAutoQRAlgoChanged(int i);
    void onCheckBoxAutoQRAlgoStateChanged(int state);
    void onCheckBoxSketchSizeMultiplierStateChanged(int state);
    void onLineEditConvergenceEditingFinished();
    void onComboBoxQRMethodCurrentIndexChanged(int index);
    void onLineEditQRPivotThresholdEditingFinished();
    void onComboBoxRedundantDefaultSolverCurrentIndexChanged(int index);
    void onLineEditRedundantConvergenceEditingFinished();
    void onSpinBoxRedundantSolverMaxIterationsValueChanged(int i);
    void onCheckBoxRedundantSketchSizeMultiplierStateChanged(int state);
    void onComboBoxDebugModeCurrentIndexChanged(int index);
    void onLineEditSolverParam1EditingFinished();
    void onLineEditRedundantSolverParam1EditingFinished();
    void onLineEditSolverParam2EditingFinished();
    void onLineEditRedundantSolverParam2EditingFinished();
    void onLineEditSolverParam3EditingFinished();
    void onLineEditRedundantSolverParam3EditingFinished();
    void onPushButtonDefaultsClicked(bool checked = false);
    void onPushButtonSolveClicked(bool checked = false);

protected:
    void updateDefaultMethodParameters();
    void updateRedundantMethodParameters();
    void updateSketchObject();

protected:
    ViewProviderSketch* sketchView;

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskSketcherSolverAdvanced> ui;
};

}  // namespace SketcherGui

#endif  // GUI_TASKVIEW_TaskSketcherSolverAdvanced_H
