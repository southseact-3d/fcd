#ifndef GUI_TASKVIEW_TaskFemConstraintGear_H
#define GUI_TASKVIEW_TaskFemConstraintGear_H

#include "TaskFemConstraintBearing.h"
#include "ViewProviderFemConstraintGear.h"

namespace FemGui
{

class TaskFemConstraintGear: public TaskFemConstraintBearing
{
    Q_OBJECT

public:
    explicit TaskFemConstraintGear(
        ViewProviderFemConstraint* ConstraintView,
        QWidget* parent = nullptr,
        const char* pixmapname = "FEM_ConstraintGear"
    );

    double getDiameter() const;
    double getForce() const;
    double getForceAngle() const;
    const std::string getDirectionName() const;
    const std::string getDirectionObject() const;
    bool getReverse() const;

private Q_SLOTS:
    void onDiameterChanged(double dia);
    void onForceChanged(double force);
    void onForceAngleChanged(double angle);
    void onButtonDirection(const bool pressed = true);
    void onCheckReversed(bool);

protected:
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
};

/// simulation dialog for the TaskView
class TaskDlgFemConstraintGear: public TaskDlgFemConstraintBearing
{
    Q_OBJECT

public:
    TaskDlgFemConstraintGear() = default;
    explicit TaskDlgFemConstraintGear(ViewProviderFemConstraintGear* ConstraintView);

    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintGear_H
