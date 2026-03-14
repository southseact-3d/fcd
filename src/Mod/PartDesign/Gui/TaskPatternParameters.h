#ifndef GUI_TASKVIEW_TaskPatternParameters_H
#define GUI_TASKVIEW_TaskPatternParameters_H

#include "TaskTransformedParameters.h"
#include "ViewProviderTransformed.h"


class QTimer;
class Ui_TaskPatternParameters;

namespace PartGui
{
class PatternParametersWidget;
}

namespace PartDesignGui
{

class TaskMultiTransformParameters;

class TaskPatternParameters: public TaskTransformedParameters
{
    Q_OBJECT

public:
    /// Constructor for task with ViewProvider
    explicit TaskPatternParameters(ViewProviderTransformed* TransformedView, QWidget* parent = nullptr);
    /// Constructor for task with parent task (MultiTransform mode)
    TaskPatternParameters(TaskMultiTransformParameters* parentTask, QWidget* parameterWidget);
    ~TaskPatternParameters() override;

    void apply() override;

protected:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private Q_SLOTS:
    void onUpdateViewTimer();
    // Slot to handle reference selection request from the widget
    void onParameterWidgetRequestReferenceSelection();
    void onParameterWidgetRequestReferenceSelection2();
    // Slot to handle parameter changes from the widget
    void onParameterWidgetParametersChanged();
    // Update view signal (might be redundant now)
    void onUpdateView(bool on) override;


private:
    void setupParameterUI(QWidget* widget) override;
    void retranslateParameterUI(QWidget* widget) override;

    void updateUI();
    void kickUpdateViewTimer() const;

    void bindProperties();

    // Task-specific logic remains
    void showOriginAxes(bool show);
    void enterReferenceSelectionMode();
    void exitReferenceSelectionMode();  // Ensure this clears gates etc.

    PartGui::PatternParametersWidget* parametersWidget = nullptr;
    PartGui::PatternParametersWidget* parametersWidget2 = nullptr;

    PartGui::PatternParametersWidget* activeDirectionWidget = nullptr;

    std::unique_ptr<Ui_TaskPatternParameters> ui;
    QTimer* updateViewTimer = nullptr;
};


/// simulation dialog for the TaskView
class TaskDlgLinearPatternParameters: public TaskDlgTransformedParameters
{
    Q_OBJECT

public:
    explicit TaskDlgLinearPatternParameters(ViewProviderTransformed* LinearPatternView);
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TaskPatternParameters_H
