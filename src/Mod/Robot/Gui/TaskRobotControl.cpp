#include <Gui/BitmapFactory.h>

#include "TaskRobotControl.h"
#include "ui_TaskRobotControl.h"


using namespace RobotGui;
using namespace Gui;

TaskRobotControl::TaskRobotControl(Robot::RobotObject* pcRobotObject, QWidget* parent)
    : TaskBox(Gui::BitmapFactory().pixmap("Robot_CreateRobot"), tr("TaskRobotControl"), true, parent)
    , pcRobot(pcRobotObject)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_TaskRobotControl();
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);

    if (pcRobotObject) {
        setRobot(pcRobotObject);
    }
}

void TaskRobotControl::setRobot(Robot::RobotObject* pcRobotObject)
{
    pcRobot = pcRobotObject;
}

TaskRobotControl::~TaskRobotControl()
{
    delete ui;
}


#include "moc_TaskRobotControl.cpp"
