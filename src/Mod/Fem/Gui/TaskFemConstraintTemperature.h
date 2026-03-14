#ifndef GUI_TASKVIEW_TaskFemConstraintTemperature_H
#define GUI_TASKVIEW_TaskFemConstraintTemperature_H

#include <QObject>
#include <memory>

#include <Gui/Selection/Selection.h>
#include <Gui/TaskView/TaskView.h>

#include "TaskFemConstraintOnBoundary.h"
#include "ViewProviderFemConstraintTemperature.h"


class Ui_TaskFemConstraintTemperature;

namespace FemGui
{
class TaskFemConstraintTemperature: public TaskFemConstraintOnBoundary
{
    Q_OBJECT

public:
    explicit TaskFemConstraintTemperature(
        ViewProviderFemConstraintTemperature* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintTemperature() override;
    const std::string getReferences() const override;
    std::string get_temperature() const;
    std::string get_cflux() const;
    std::string get_constraint_type() const;

private Q_SLOTS:
    void onReferenceDeleted();
    void onConstrTypeChanged(int item);
    void onCFluxChanged(double);
    void onTempChanged(double);
    void addToSelection() override;
    void removeFromSelection() override;

protected:
    void changeEvent(QEvent* e) override;
    void clearButtons(const SelectionChangeModes notThis) override;

private:
    void updateUI();
    std::unique_ptr<Ui_TaskFemConstraintTemperature> ui;
};

class TaskDlgFemConstraintTemperature: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintTemperature(ViewProviderFemConstraintTemperature* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintTemperature_H
