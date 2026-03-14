#ifndef GUI_TASKVIEW_TaskFemConstraintPressure_H
#define GUI_TASKVIEW_TaskFemConstraintPressure_H

#include <QObject>
#include <memory>

#include "TaskFemConstraintOnBoundary.h"
#include "ViewProviderFemConstraintPressure.h"


class Ui_TaskFemConstraintPressure;

namespace FemGui
{
class TaskFemConstraintPressure: public TaskFemConstraintOnBoundary
{
    Q_OBJECT

public:
    explicit TaskFemConstraintPressure(
        ViewProviderFemConstraintPressure* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintPressure() override;
    const std::string getReferences() const override;
    std::string getPressure() const;
    bool getReverse() const;

private Q_SLOTS:
    void onReferenceDeleted();
    void onCheckReverse(bool);
    void addToSelection() override;
    void removeFromSelection() override;

protected:
    void changeEvent(QEvent* e) override;
    void clearButtons(const SelectionChangeModes notThis) override;

private:
    void updateUI();
    std::unique_ptr<Ui_TaskFemConstraintPressure> ui;
};

class TaskDlgFemConstraintPressure: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintPressure(ViewProviderFemConstraintPressure* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintPressure_H
