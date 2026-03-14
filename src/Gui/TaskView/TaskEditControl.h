#ifndef GUI_TASKVIEW_TASKEDITCONTROL_H
#define GUI_TASKVIEW_TASKEDITCONTROL_H

#include "TaskView.h"


class Ui_TaskEditControl;
class QHBoxLayout;
class QDialogButtonBox;

namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
namespace TaskView
{


class TaskEditControl: public TaskWidget
{
    Q_OBJECT

public:
    explicit TaskEditControl(QWidget* parent = nullptr);
    ~TaskEditControl() override;
    QDialogButtonBox* standardButtons() const;
    friend class TaskView;

protected:
    QHBoxLayout* hboxLayout;
    QDialogButtonBox* buttonBox;
};

}  // namespace TaskView
}  // namespace Gui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
