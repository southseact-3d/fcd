#ifndef GUI_TASKVIEW_TaskScaledParameters_H
#define GUI_TASKVIEW_TaskScaledParameters_H

#include "TaskTransformedParameters.h"
#include "ViewProviderScaled.h"


class Ui_TaskScaledParameters;

namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
}

namespace PartDesignGui
{

class TaskMultiTransformParameters;

class TaskScaledParameters: public TaskTransformedParameters
{
    Q_OBJECT

public:
    /// Constructor for task with ViewProvider
    explicit TaskScaledParameters(ViewProviderTransformed* TransformedView, QWidget* parent = nullptr);
    /// Constructor for task with parent task (MultiTransform mode)
    TaskScaledParameters(TaskMultiTransformParameters* parentTask, QWidget* parameterWidget);

    void apply() override;

private Q_SLOTS:
    void onFactor(double factor);
    void onOccurrences(uint number);
    void onUpdateView(bool /*unused*/) override;


private:
    void setupParameterUI(QWidget* widget) override;
    void retranslateParameterUI(QWidget* widget) override;
    void updateUI();

    double getFactor() const;
    unsigned getOccurrences() const;

private:
    std::unique_ptr<Ui_TaskScaledParameters> ui;
};


/// simulation dialog for the TaskView
class TaskDlgScaledParameters: public TaskDlgTransformedParameters
{
    Q_OBJECT

public:
    explicit TaskDlgScaledParameters(ViewProviderScaled* ScaledView);
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
