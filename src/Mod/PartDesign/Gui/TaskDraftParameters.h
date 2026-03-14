#ifndef GUI_TASKVIEW_TaskDraftParameters_H
#define GUI_TASKVIEW_TaskDraftParameters_H

#include "TaskDressUpParameters.h"
#include "ViewProviderDraft.h"

class Ui_TaskDraftParameters;

namespace Gui
{
class RotationGizmo;
class GizmoContainer;
}  // namespace Gui

namespace PartDesignGui
{

class TaskDraftParameters: public TaskDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskDraftParameters(ViewProviderDressUp* DressUpView, QWidget* parent = nullptr);
    ~TaskDraftParameters() override;

    void apply() override;

    double getAngle() const;
    bool getReversed() const;
    const std::vector<std::string> getFaces() const;
    void getPlane(App::DocumentObject*& obj, std::vector<std::string>& sub) const;
    void getLine(App::DocumentObject*& obj, std::vector<std::string>& sub) const;

private Q_SLOTS:
    void onAngleChanged(double angle);
    void onReversedChanged(bool reversed);
    void onButtonPlane(bool checked);
    void onButtonLine(bool checked);
    void onRefDeleted() override;

protected:
    void setButtons(selectionModes mode) override;
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    std::unique_ptr<Ui_TaskDraftParameters> ui;

    std::unique_ptr<Gui::GizmoContainer> gizmoContainer;
    Gui::RotationGizmo* angleGizmo = nullptr;
    void setupGizmos(ViewProvider* vp);
    void setGizmoPositions();
};

/// simulation dialog for the TaskView
class TaskDlgDraftParameters: public TaskDlgDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskDlgDraftParameters(ViewProviderDraft* DraftView);
    ~TaskDlgDraftParameters() override;

public:
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
