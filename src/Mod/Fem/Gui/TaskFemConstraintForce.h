#ifndef GUI_TASKVIEW_TaskFemConstraintForce_H
#define GUI_TASKVIEW_TaskFemConstraintForce_H

#include <QObject>
#include <memory>

#include "TaskFemConstraintOnBoundary.h"
#include "ViewProviderFemConstraintForce.h"


class Ui_TaskFemConstraintForce;

namespace App
{
class Property;
}

namespace Gui
{
class SelectionObject;
class ViewProvider;
}  // namespace Gui

namespace FemGui
{

class TaskFemConstraintForce: public TaskFemConstraintOnBoundary
{
    Q_OBJECT

public:
    explicit TaskFemConstraintForce(
        ViewProviderFemConstraintForce* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintForce() override;
    const std::string getForce() const;
    const std::string getReferences() const override;
    const std::string getDirectionName() const;
    const std::string getDirectionObject() const;
    bool getReverse() const;

private Q_SLOTS:
    void onReferenceDeleted();
    void onButtonDirection(const bool pressed = false);
    void onCheckReverse(bool);
    void addToSelection() override;
    void removeFromSelection() override;

protected:
    void changeEvent(QEvent* e) override;
    void clearButtons(const SelectionChangeModes notThis) override;

private:
    std::pair<App::DocumentObject*, std::string> getDirection(
        const std::vector<Gui::SelectionObject>&
    ) const;
    void updateUI();

private:
    std::unique_ptr<Ui_TaskFemConstraintForce> ui;
};

/// simulation dialog for the TaskView
class TaskDlgFemConstraintForce: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintForce(ViewProviderFemConstraintForce* ConstraintView);

    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintForce_H
