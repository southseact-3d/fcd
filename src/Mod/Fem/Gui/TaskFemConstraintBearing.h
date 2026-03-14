#ifndef GUI_TASKVIEW_TaskFemConstraintBearing_H
#define GUI_TASKVIEW_TaskFemConstraintBearing_H

#include <QObject>
#include <memory>

#include "TaskFemConstraint.h"
#include "ViewProviderFemConstraintBearing.h"


class Ui_TaskFemConstraintBearing;

namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
}

namespace FemGui
{

class TaskFemConstraintBearing: public TaskFemConstraint
{
    Q_OBJECT

public:
    explicit TaskFemConstraintBearing(
        ViewProviderFemConstraint* ConstraintView,
        QWidget* parent = nullptr,
        const char* pixmapname = "FEM_ConstraintBearing"
    );
    ~TaskFemConstraintBearing() override;

    double getDistance() const;
    const std::string getReferences() const override;
    const std::string getLocationName() const;
    const std::string getLocationObject() const;
    bool getAxial() const;

private Q_SLOTS:
    void onReferenceDeleted();
    void onDistanceChanged(double l);
    void onButtonLocation(const bool pressed = true);
    void onCheckAxial(bool);

protected:
    void changeEvent(QEvent* e) override;
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;

protected:
    std::unique_ptr<Ui_TaskFemConstraintBearing> ui;
};

/// simulation dialog for the TaskView
class TaskDlgFemConstraintBearing: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    TaskDlgFemConstraintBearing() = default;
    explicit TaskDlgFemConstraintBearing(ViewProviderFemConstraintBearing* ConstraintView);

    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintBearing_H
