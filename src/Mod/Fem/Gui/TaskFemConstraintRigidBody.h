#ifndef GUI_TASKVIEW_TaskFemConstraintRigidBody_H
#define GUI_TASKVIEW_TaskFemConstraintRigidBody_H

#include <QObject>

#include "TaskFemConstraintOnBoundary.h"
#include "ViewProviderFemConstraintRigidBody.h"


class Ui_TaskFemConstraintRigidBody;

namespace FemGui
{
class TaskFemConstraintRigidBody: public TaskFemConstraintOnBoundary
{
    Q_OBJECT

public:
    explicit TaskFemConstraintRigidBody(
        ViewProviderFemConstraintRigidBody* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintRigidBody() override;

    const std::string getReferences() const override;
    Base::Vector3d getReferenceNode() const;
    Base::Vector3d getDisplacement() const;
    Base::Rotation getRotation() const;
    std::vector<std::string> getForce() const;
    std::vector<std::string> getMoment() const;
    std::vector<std::string> getTranslationalMode() const;
    std::vector<std::string> getRotationalMode() const;

private Q_SLOTS:
    void onReferenceDeleted();
    void addToSelection() override;
    void removeFromSelection() override;
    void onTransModeXChanged(int);
    void onTransModeYChanged(int);
    void onTransModeZChanged(int);
    void onRotModeXChanged(int);
    void onRotModeYChanged(int);
    void onRotModeZChanged(int);
    void onRefNodeXChanged(double);
    void onRefNodeYChanged(double);
    void onRefNodeZChanged(double);

protected:
    void changeEvent(QEvent* e) override;
    void clearButtons(const SelectionChangeModes notThis) override;

private:
    void updateUI();
    Ui_TaskFemConstraintRigidBody* ui;
};

class TaskDlgFemConstraintRigidBody: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintRigidBody(ViewProviderFemConstraintRigidBody* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintRigidBody_H
