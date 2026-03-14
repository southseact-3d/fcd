#ifndef GUI_TASKVIEW_TaskFemConstraintPlaneRotation_H
#define GUI_TASKVIEW_TaskFemConstraintPlaneRotation_H

#include <QObject>
#include <memory>

#include "TaskFemConstraint.h"
#include "ViewProviderFemConstraintPlaneRotation.h"


class Ui_TaskFemConstraintPlaneRotation;

namespace FemGui
{
class TaskFemConstraintPlaneRotation: public TaskFemConstraint
{
    Q_OBJECT

public:
    explicit TaskFemConstraintPlaneRotation(
        ViewProviderFemConstraintPlaneRotation* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintPlaneRotation() override;
    const std::string getReferences() const override;

private Q_SLOTS:
    void onReferenceDeleted();
    void addToSelection();
    void removeFromSelection();

protected:
    void changeEvent(QEvent* e) override;

private:
    void updateUI();
    std::unique_ptr<Ui_TaskFemConstraintPlaneRotation> ui;
};

class TaskDlgFemConstraintPlaneRotation: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintPlaneRotation(ViewProviderFemConstraintPlaneRotation* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintPlaneRotation_H
