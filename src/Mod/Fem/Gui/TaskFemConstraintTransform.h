#ifndef GUI_TASKVIEW_TaskFemConstraintTransform_H
#define GUI_TASKVIEW_TaskFemConstraintTransform_H

#include <QObject>
#include <memory>

#include "TaskFemConstraint.h"
#include "ViewProviderFemConstraintTransform.h"


class Ui_TaskFemConstraintTransform;

namespace FemGui
{
class TaskFemConstraintTransform: public TaskFemConstraint
{
    Q_OBJECT

public:
    explicit TaskFemConstraintTransform(
        ViewProviderFemConstraintTransform* ConstraintView,
        QWidget* parent = nullptr
    );
    ~TaskFemConstraintTransform() override;
    const std::string getReferences() const override;
    Base::Rotation getRotation() const;
    std::string get_transform_type() const;
    static std::string getSurfaceReferences(const std::string showConstr);

private Q_SLOTS:
    void onReferenceDeleted();
    void Rect();
    void Cyl();
    void addToSelection();
    void removeFromSelection();
    void xAxisChanged(double x);
    void yAxisChanged(double y);
    void zAxisChanged(double z);
    void angleChanged(double a);

protected:
    void changeEvent(QEvent* e) override;
    const QString makeText(const App::DocumentObject* obj) const;

private:
    void updateUI();
    std::unique_ptr<Ui_TaskFemConstraintTransform> ui;
};

class TaskDlgFemConstraintTransform: public TaskDlgFemConstraint
{
    Q_OBJECT

public:
    explicit TaskDlgFemConstraintTransform(ViewProviderFemConstraintTransform* ConstraintView);
    bool accept() override;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintTransform_H
