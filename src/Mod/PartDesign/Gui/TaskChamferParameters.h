#ifndef GUI_TASKVIEW_TaskChamferParameters_H
#define GUI_TASKVIEW_TaskChamferParameters_H

#include <Gui/Inventor/Draggers/Gizmo.h>

#include "TaskDressUpParameters.h"
#include "ViewProviderChamfer.h"

class Ui_TaskChamferParameters;
namespace PartDesign
{
class Chamfer;
}

namespace Gui
{
class LinearGizmo;
class RotationalGizmo;
class GizmoContainer;
}  // namespace Gui

namespace PartDesignGui
{

class TaskChamferParameters: public TaskDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskChamferParameters(ViewProviderDressUp* DressUpView, QWidget* parent = nullptr);
    ~TaskChamferParameters() override;

    void apply() override;

private Q_SLOTS:
    void onTypeChanged(int);
    void onSizeChanged(double);
    void onSize2Changed(double);
    void onAngleChanged(double);
    void onFlipDirection(bool);
    void onRefDeleted() override;
    void onAddAllEdges();
    void onCheckBoxUseAllEdgesToggled(bool checked);

protected:
    void setButtons(const selectionModes mode) override;
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

    int getType() const;
    double getSize() const;
    double getSize2() const;
    double getAngle() const;
    bool getFlipDirection() const;

private:
    std::unique_ptr<Ui_TaskChamferParameters> ui;

    void setUpUI(PartDesign::Chamfer* pcChamfer);

    std::unique_ptr<Gui::GizmoContainer> gizmoContainer;
    Gui::LinearGizmo* distanceGizmo = nullptr;
    Gui::LinearGizmo* secondDistanceGizmo = nullptr;
    Gui::RotationGizmo* angleGizmo = nullptr;
    void setupGizmos(ViewProviderDressUp* vp);
    void setGizmoPositions();
};

/// simulation dialog for the TaskView
class TaskDlgChamferParameters: public TaskDlgDressUpParameters
{
    Q_OBJECT

public:
    explicit TaskDlgChamferParameters(ViewProviderChamfer* DressUpView);
    ~TaskDlgChamferParameters() override;

public:
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TaskChamferParameters_H
