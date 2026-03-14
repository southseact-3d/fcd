#include <Gui/BitmapFactory.h>

#include "TaskRobotMessages.h"
#include "ui_TaskRobotMessages.h"


using namespace RobotGui;
using namespace Gui;

TaskRobotMessages::TaskRobotMessages(Robot::RobotObject* pcRobotObject, QWidget* parent)
    : TaskBox(Gui::BitmapFactory().pixmap("document-new"), tr("TaskRobotMessages"), true, parent)
    , pcRobot(pcRobotObject)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_TaskRobotMessages();
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);
}

TaskRobotMessages::~TaskRobotMessages()
{
    delete ui;
}


#include "moc_TaskRobotMessages.cpp"
