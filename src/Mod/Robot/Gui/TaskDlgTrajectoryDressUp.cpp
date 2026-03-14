#include <Gui/Application.h>
#include <Gui/Document.h>

#include "TaskDlgTrajectoryDressUp.h"


using namespace RobotGui;

//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgTrajectoryDressUp::TaskDlgTrajectoryDressUp(Robot::TrajectoryDressUpObject* obj)
    : TaskDialog()
    , pcObject(obj)
{
    param = new TaskTrajectoryDressUpParameter(obj);

    Content.push_back(param);
}

//==== calls from the TaskView ===============================================================


void TaskDlgTrajectoryDressUp::open()
{}

void TaskDlgTrajectoryDressUp::clicked(int button)
{
    if (QDialogButtonBox::Apply == button) {
        // transfer the values to the object
        param->writeValues();
        // May throw an exception which we must handle here
        pcObject->recomputeFeature();
    }
}

bool TaskDlgTrajectoryDressUp::accept()
{
    param->writeValues();
    pcObject->recomputeFeature();

    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    if (doc) {
        doc->resetEdit();
    }
    return true;
}

bool TaskDlgTrajectoryDressUp::reject()
{
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    if (doc) {
        doc->resetEdit();
    }
    return true;
}

void TaskDlgTrajectoryDressUp::helpRequested()
{}


#include "moc_TaskDlgTrajectoryDressUp.cpp"
