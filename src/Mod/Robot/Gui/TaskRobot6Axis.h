#ifndef GUI_TASKVIEW_TaskRobot6Axis_H
#define GUI_TASKVIEW_TaskRobot6Axis_H

#include <Gui/TaskView/TaskView.h>
#include <Mod/Robot/App/RobotObject.h>


class QLineEdit;

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


class Ui_TaskRobot6Axis;
class TaskRobot6Axis: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskRobot6Axis(Robot::RobotObject* pcRobotObject, QWidget* parent = nullptr);
    ~TaskRobot6Axis() override;

    void setRobot(Robot::RobotObject* pcRobotObject);

public Q_SLOTS:
    void setAxis(float A1, float A2, float A3, float A4, float A5, float A6, const Base::Placement& Tcp);
    void changeSliderA1(int value);
    void changeSliderA2(int value);
    void changeSliderA3(int value);
    void changeSliderA4(int value);
    void changeSliderA5(int value);
    void changeSliderA6(int value);
    void createPlacementDlg();

protected:
    Robot::RobotObject* pcRobot;
    void viewTcp(const Base::Placement& pos);
    void viewTool(const Base::Placement& pos);
    void setColor(int i, float angle, QLineEdit& lineEdit);

private:
private:
    QWidget* proxy;
    Ui_TaskRobot6Axis* ui;
    Robot::Robot6Axis* Rob;
};

}  // namespace RobotGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
