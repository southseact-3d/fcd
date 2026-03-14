#ifndef GUI_TASKVIEW_TaskFemConstraintContact_H
#define GUI_TASKVIEW_TaskFemConstraintContact_H

#include <QObject>
#include <memory>

#include "TaskFemConstraint.h"
#include "ViewProviderFemConstraintContact.h"


class Ui_TaskFemConstraintContact;

namespace FemGui
{
class TaskFemConstraintContact: public TaskFemConstraint
{
    Q_OBJECT

public:
    explicit TaskFemConstraintContact(
        ViewProviderFemConstraintContact* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintContact() override;
    const std::string getReferences() const override;
    const std::string getAdjust() const;
    const std::string getSlope() const;
    bool getFriction() const;
    const std::string getStickSlope() const;
    double getFrictionCoeff() const;

private Q_SLOTS:
    void onReferenceDeletedSlave();
    void onReferenceDeletedMaster();
    void addToSelectionSlave();
    void removeFromSelectionSlave();
    void addToSelectionMaster();
    void removeFromSelectionMaster();
    void onFrictionChanged(bool);

protected:
    void changeEvent(QEvent* e) override;

private:
    void updateUI();
    std::unique_ptr<Ui_TaskFemConstraintContact> ui;
};

class TaskDlgFemConstraintContact: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintContact(ViewProviderFemConstraintContact* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintContact_H
