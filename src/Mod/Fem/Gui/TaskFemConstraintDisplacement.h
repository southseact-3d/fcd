#ifndef GUI_TASKVIEW_TaskFemConstraintDisplacement_H
#define GUI_TASKVIEW_TaskFemConstraintDisplacement_H

#include <QObject>
#include <memory>

#include <Gui/Selection/Selection.h>
#include <Gui/TaskView/TaskView.h>

#include "TaskFemConstraint.h"
#include "TaskFemConstraintOnBoundary.h"
#include "ViewProviderFemConstraintDisplacement.h"


class Ui_TaskFemConstraintDisplacement;

namespace FemGui
{
class TaskFemConstraintDisplacement: public TaskFemConstraintOnBoundary
{
    Q_OBJECT

public:
    explicit TaskFemConstraintDisplacement(
        ViewProviderFemConstraintDisplacement* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintDisplacement() override;

    const std::string getReferences() const override;
    std::string get_spinxDisplacement() const;
    std::string get_spinyDisplacement() const;
    std::string get_spinzDisplacement() const;
    std::string get_spinxRotation() const;
    std::string get_spinyRotation() const;
    std::string get_spinzRotation() const;
    std::string get_xFormula() const;
    std::string get_yFormula() const;
    std::string get_zFormula() const;
    bool get_dispxfree() const;
    bool get_hasDispXFormula() const;
    bool get_dispyfree() const;
    bool get_hasDispYFormula() const;
    bool get_dispzfree() const;
    bool get_hasDispZFormula() const;
    bool get_rotxfree() const;
    bool get_rotyfree() const;
    bool get_rotzfree() const;
    bool get_useFlowSurfaceForce() const;

private Q_SLOTS:
    void onReferenceDeleted();
    void formulaX(bool);
    void formulaY(bool);
    void formulaZ(bool);
    void flowForce(bool);
    void formulaRotx(bool);
    void formulaRoty(bool);
    void formulaRotz(bool);

    void addToSelection() override;
    void removeFromSelection() override;

protected:
    void changeEvent(QEvent* e) override;
    void clearButtons(const SelectionChangeModes notThis) override;

private:
    void updateUI();
    std::unique_ptr<Ui_TaskFemConstraintDisplacement> ui;
};

class TaskDlgFemConstraintDisplacement: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintDisplacement(ViewProviderFemConstraintDisplacement* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintDisplacement_H
