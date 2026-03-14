#ifndef GUI_TASKVIEW_TaskMirroredParameters_H
#define GUI_TASKVIEW_TaskMirroredParameters_H

#include "TaskTransformedParameters.h"
#include "ViewProviderMirrored.h"


class Ui_TaskMirroredParameters;

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

class TaskMirroredParameters: public TaskTransformedParameters
{
    Q_OBJECT

public:
    /// Constructor for task with ViewProvider
    explicit TaskMirroredParameters(ViewProviderTransformed* TransformedView, QWidget* parent = nullptr);
    /// Constructor for task with parent task (MultiTransform mode)
    TaskMirroredParameters(TaskMultiTransformParameters* parentTask, QWidget* parameterWidget);

    ~TaskMirroredParameters() override;

    void apply() override;

protected:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private Q_SLOTS:
    void onPlaneChanged(int num);
    void onUpdateView(bool /*unused*/) override;

private:
    void setupParameterUI(QWidget* widget) override;
    void retranslateParameterUI(QWidget* widget) override;
    void updateUI();
    void getMirrorPlane(App::DocumentObject*& obj, std::vector<std::string>& sub) const;

private:
    Gui::ComboLinks planeLinks;
    std::unique_ptr<Ui_TaskMirroredParameters> ui;
};


/// simulation dialog for the TaskView
class TaskDlgMirroredParameters: public TaskDlgTransformedParameters
{
    Q_OBJECT

public:
    explicit TaskDlgMirroredParameters(ViewProviderMirrored* MirroredView);
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
