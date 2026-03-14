#ifndef GUI_TASKVIEW_TaskFemConstraintInitialTemperature_H
#define GUI_TASKVIEW_TaskFemConstraintInitialTemperature_H

#include <QObject>
#include <memory>

#include "TaskFemConstraint.h"
#include "ViewProviderFemConstraintInitialTemperature.h"


class Ui_TaskFemConstraintInitialTemperature;

namespace FemGui
{
class TaskFemConstraintInitialTemperature: public TaskFemConstraint
{
    Q_OBJECT

public:
    explicit TaskFemConstraintInitialTemperature(
        ViewProviderFemConstraintInitialTemperature* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintInitialTemperature() override;
    std::string get_temperature() const;

protected:
    void changeEvent(QEvent* e) override;

private:
    std::unique_ptr<Ui_TaskFemConstraintInitialTemperature> ui;
};

class TaskDlgFemConstraintInitialTemperature: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintInitialTemperature(
        ViewProviderFemConstraintInitialTemperature* ConstraintView
    );
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintInitialTemperature_H
