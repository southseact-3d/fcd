#ifndef GUI_TASKVIEW_TaskRobotControl_H
#define GUI_TASKVIEW_TaskRobotControl_H

#include <Gui/TaskView/TaskView.h>

#include <Mod/Robot/App/RobotObject.h>


namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
}

namespace RobotGui
{

class Ui_TaskRobotControl;
class TaskRobotControl: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskRobotControl(Robot::RobotObject* pcRobotObject, QWidget* parent = nullptr);
    ~TaskRobotControl() override;

    void setRobot(Robot::RobotObject* pcRobotObject);

private Q_SLOTS:


protected:
    Robot::RobotObject* pcRobot;

private:
private:
    QWidget* proxy;
    Ui_TaskRobotControl* ui;
};

}  // namespace RobotGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
