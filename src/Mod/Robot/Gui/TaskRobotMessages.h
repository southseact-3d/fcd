#ifndef GUI_TASKVIEW_TaskRobotMessages_H
#define GUI_TASKVIEW_TaskRobotMessages_H

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


class Ui_TaskRobotMessages;
class TaskRobotMessages: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskRobotMessages(Robot::RobotObject* pcRobotObject, QWidget* parent = nullptr);
    ~TaskRobotMessages() override;

private Q_SLOTS:


protected:
    Robot::RobotObject* pcRobot;

private:
private:
    QWidget* proxy;
    Ui_TaskRobotMessages* ui;
};

}  // namespace RobotGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
