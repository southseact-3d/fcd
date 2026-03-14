#ifndef GUI_TASKVIEW_TaskTrajectory_H
#define GUI_TASKVIEW_TaskTrajectory_H

#include <Gui/Selection/Selection.h>
#include <Gui/TaskView/TaskView.h>
#include <Mod/Robot/App/RobotObject.h>
#include <Mod/Robot/App/Simulation.h>
#include <Mod/Robot/App/TrajectoryObject.h>

#include "ViewProviderRobotObject.h"


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


class Ui_TaskTrajectory;
class TaskTrajectory: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    TaskTrajectory(
        Robot::RobotObject* pcRobotObject,
        Robot::TrajectoryObject* pcTrajectoryObject,
        QWidget* parent = nullptr
    );
    ~TaskTrajectory() override;
    /// Observer message from the Selection
    void OnChange(
        Gui::SelectionSingleton::SubjectType& rCaller,
        Gui::SelectionSingleton::MessageType Reason
    );

private Q_SLOTS:
    void start();
    void stop();
    void run();
    void back();
    void forward();
    void end();

    void timerDone();
    void valueChanged(int value);
    void valueChanged(double d);

Q_SIGNALS:
    void axisChanged(float A1, float A2, float A3, float A4, float A5, float A6, const Base::Placement& Tcp);

protected:
    void setTo();
    void viewTool(const Base::Placement& pos);

    QTimer* timer;

    Robot::Simulation sim;
    Robot::RobotObject* pcRobot;
    ViewProviderRobotObject* ViewProv;

    bool Run;
    bool block;

    float timePos;
    float duration;

private:
    QWidget* proxy;
    Ui_TaskTrajectory* ui;
};

}  // namespace RobotGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
