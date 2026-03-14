#ifndef GUI_TASKVIEW_TaskFemConstraintSpring_H
#define GUI_TASKVIEW_TaskFemConstraintSpring_H

#include <QObject>
#include <memory>

#include "TaskFemConstraintOnBoundary.h"
#include "ViewProviderFemConstraintSpring.h"


class Ui_TaskFemConstraintSpring;

namespace FemGui
{
class TaskFemConstraintSpring: public TaskFemConstraintOnBoundary
{
    Q_OBJECT

public:
    explicit TaskFemConstraintSpring(
        ViewProviderFemConstraintSpring* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintSpring() override;
    const std::string getReferences() const override;
    std::string getNormalStiffness() const;
    std::string getTangentialStiffness() const;
    std::string getElmerStiffness() const;

private Q_SLOTS:
    void onReferenceDeleted();
    void addToSelection() override;
    void removeFromSelection() override;

protected:
    void changeEvent(QEvent* e) override;
    void clearButtons(const SelectionChangeModes notThis) override;

private:
    void updateUI();
    std::unique_ptr<Ui_TaskFemConstraintSpring> ui;
};

class TaskDlgFemConstraintSpring: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintSpring(ViewProviderFemConstraintSpring* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintSpring_H
