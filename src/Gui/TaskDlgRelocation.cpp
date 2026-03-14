#include "Dialogs/ui_DlgLocationAngle.h"
#include "Dialogs/ui_DlgLocationPos.h"

#include "TaskDlgRelocation.h"
#include "BitmapFactory.h"


using namespace Gui;

//**************************************************************************
//**************************************************************************
// TaskBoxPosition
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/* TRANSLATOR Gui::TaskBoxPosition */

TaskBoxPosition::TaskBoxPosition(QWidget* parent)
    : TaskBox(Gui::BitmapFactory().pixmap("Robot_CreateRobot"), tr("Position"), true, parent)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_Position();
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);
}

TaskBoxPosition::~TaskBoxPosition()
{
    delete ui;
}


//**************************************************************************
//**************************************************************************
// TaskBoxAngle
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/* TRANSLATOR Gui::TaskBoxAngle */

TaskBoxAngle::TaskBoxAngle(QWidget* parent)
    : TaskBox(Gui::BitmapFactory().pixmap("Robot_CreateRobot"), tr("Angle"), true, parent)
{
    // we need a separate container widget to add all controls to
    proxy = new QWidget(this);
    ui = new Ui_Angle();
    ui->setupUi(proxy);
    QMetaObject::connectSlotsByName(this);

    this->groupLayout()->addWidget(proxy);
}

TaskBoxAngle::~TaskBoxAngle()
{
    delete ui;
}


//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/* TRANSLATOR Gui::TaskDlgRelocation */

TaskDlgRelocation::TaskDlgRelocation()
    : TaskDialog()
{}

TaskDlgRelocation::~TaskDlgRelocation() = default;

//==== calls from the TaskView ===============================================================


void TaskDlgRelocation::open()
{}

void TaskDlgRelocation::clicked(int)
{}

bool TaskDlgRelocation::accept()
{
    return true;
}

bool TaskDlgRelocation::reject()
{
    return true;
}

void TaskDlgRelocation::helpRequested()
{}


#include "moc_TaskDlgRelocation.cpp"
