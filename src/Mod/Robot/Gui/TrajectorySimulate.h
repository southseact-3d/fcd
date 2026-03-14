#ifndef GUI_TASKVIEW_TrajectorySimulate_H
#define GUI_TASKVIEW_TrajectorySimulate_H

#include <QDialog>
#include <memory>

#include <Mod/Robot/App/RobotObject.h>
#include <Mod/Robot/App/Simulation.h>
#include <Mod/Robot/App/TrajectoryObject.h>

#include "ViewProviderRobotObject.h"


namespace RobotGui
{

class Ui_DlgTrajectorySimulate;

class TrajectorySimulate: public QDialog
{
    Q_OBJECT

public:
    TrajectorySimulate(
        Robot::RobotObject* pcRobotObject,
        Robot::TrajectoryObject* pcTrajectoryObject,
        QWidget* parent = nullptr
    );
    ~TrajectorySimulate() override;

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

protected:
    void setTo();

    QTimer* timer;

    Robot::Simulation sim;

    ViewProviderRobotObject* ViewProv;

    bool Run;
    bool block;

    float timePos;
    float duration;

private:
    std::unique_ptr<Ui_DlgTrajectorySimulate> ui;
};

}  // namespace RobotGui

#endif  // GUI_TASKVIEW_TrajectorySimulate_H
