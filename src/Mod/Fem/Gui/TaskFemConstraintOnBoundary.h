#ifndef GUI_TASKVIEW_TaskFemConstraintOnBoundary_H
#define GUI_TASKVIEW_TaskFemConstraintOnBoundary_H

#include <QObject>

#include <Gui/Selection/Selection.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/Widgets.h>
#include <Mod/Fem/FemGlobal.h>

#include "TaskFemConstraint.h"


namespace FemGui
{

/** @brief Taskbox for FEM constraints that apply on subsets of the domain boundary
 *
 *  @detail Convenience superclass for taskboxes setting certain constraints
 *  that apply on subsets of the boundary (faces/edges/vertices), where one or
 *  more boundary entities need to be selected.
 */
class TaskFemConstraintOnBoundary: public TaskFemConstraint
{
    Q_OBJECT

public:
    explicit TaskFemConstraintOnBoundary(
        ViewProviderFemConstraint* ConstraintView,
        QWidget* parent = nullptr,
        const char* pixmapname = ""
    );
    ~TaskFemConstraintOnBoundary() override;

protected Q_SLOTS:
    void onButtonToggled(QAbstractButton* button, bool checked);
    virtual void addToSelection() = 0;
    virtual void removeFromSelection() = 0;

protected:
    enum class SelectionChangeModes
    {
        none,
        refAdd,
        refRemove
    };
    void onSelectionChanged(const Gui::SelectionChanges&) override;
    virtual void clearButtons(const SelectionChangeModes notThis) = 0;

protected:
    enum SelectionChangeModes selChangeMode;
    Gui::ButtonGroup* buttonGroup;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskFemConstraintOnBoundary_H
