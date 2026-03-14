#ifndef GUI_TASKVIEW_TaskFemConstraintFixed_H
#define GUI_TASKVIEW_TaskFemConstraintFixed_H

#include <QObject>
#include <memory>

#include "TaskFemConstraintOnBoundary.h"
#include "ViewProviderFemConstraintFixed.h"


class Ui_TaskFemConstraintFixed;

namespace FemGui
{
class TaskFemConstraintFixed: public TaskFemConstraintOnBoundary
{
    Q_OBJECT

public:
    explicit TaskFemConstraintFixed(
        ViewProviderFemConstraintFixed* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintFixed() override;
    const std::string getReferences() const override;

private Q_SLOTS:
    void onReferenceDeleted();
    void addToSelection() override;
    void removeFromSelection() override;

protected:
    void changeEvent(QEvent* e) override;
    void clearButtons(const SelectionChangeModes notThis) override;

private:
    void updateUI();
    std::unique_ptr<Ui_TaskFemConstraintFixed> ui;
};

class TaskDlgFemConstraintFixed: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintFixed(ViewProviderFemConstraintFixed* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintFixed_H
