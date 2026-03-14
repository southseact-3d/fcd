#ifndef GUI_TASKVIEW_TaskFemConstraintPulley_H
#define GUI_TASKVIEW_TaskFemConstraintPulley_H

#include "TaskFemConstraintGear.h"
#include "ViewProviderFemConstraintPulley.h"

namespace FemGui
{

class TaskFemConstraintPulley: public TaskFemConstraintGear
{
    Q_OBJECT

public:
    explicit TaskFemConstraintPulley(
        ViewProviderFemConstraintPulley* ConstraintView,
        QWidget* parent = nullptr
    );

    double getOtherDiameter() const;
    double getCenterDistance() const;
    double getTensionForce() const;
    double getTorque() const;
    bool getIsDriven() const;

private Q_SLOTS:
    void onOtherDiameterChanged(double dia);
    void onCenterDistanceChanged(double dia);
    void onTensionForceChanged(double force);
    void onCheckIsDriven(bool);

protected:
    void changeEvent(QEvent* e) override;
};

/// simulation dialog for the TaskView
class TaskDlgFemConstraintPulley: public TaskDlgFemConstraintGear
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintPulley(ViewProviderFemConstraintPulley* ConstraintView);

    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintPulley_H
