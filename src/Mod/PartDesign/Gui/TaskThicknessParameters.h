#ifndef GUI_TASKVIEW_TaskThicknessParameters_H
#define GUI_TASKVIEW_TaskThicknessParameters_H

#include <Gui/Inventor/Draggers/Gizmo.h>

#include "TaskDressUpParameters.h"
#include "ViewProviderThickness.h"

class Ui_TaskThicknessParameters;

namespace Gui
{
class LinearGizmo;
class GizmoContainer;
}  // namespace Gui

namespace PartDesign
{
class Thickness;
}

namespace PartDesignGui
{

class TaskThicknessParameters: public TaskDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskThicknessParameters(ViewProviderDressUp* DressUpView, QWidget* parent = nullptr);
    ~TaskThicknessParameters() override;

    void apply() override;

    double getValue() const;
    bool getReversed() const;
    bool getIntersection() const;
    int getMode() const;
    int getJoinType() const;

private Q_SLOTS:
    void onValueChanged(double angle);
    void onModeChanged(int mode);
    void onJoinTypeChanged(int join);
    void onReversedChanged(bool on);
    void onIntersectionChanged(bool on);
    void onRefDeleted() override;

protected:
    void setButtons(const selectionModes mode) override;
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

private:
    void addContainerWidget();
    void initControls();
    void setupConnections();
    PartDesign::Thickness* onBeforeChange();
    void onAfterChange(PartDesign::Thickness* obj);

private:
    std::unique_ptr<Ui_TaskThicknessParameters> ui;

    std::unique_ptr<Gui::GizmoContainer> gizmoContainer;
    Gui::LinearGizmo* linearGizmo = nullptr;
    void setupGizmos(ViewProviderDressUp* vp);
    void setGizmoPositions();
};

/// simulation dialog for the TaskView
class TaskDlgThicknessParameters: public TaskDlgDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskDlgThicknessParameters(ViewProviderThickness* ThicknessView);
    ~TaskDlgThicknessParameters() override;

public:
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
