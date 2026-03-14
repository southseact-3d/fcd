#include <QApplication>


#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/TaskView/TaskSelectLinkProperty.h>

#include "TaskDlgTrajectoryCompound.h"


using namespace RobotGui;

//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgTrajectoryCompound::TaskDlgTrajectoryCompound(Robot::TrajectoryCompound* obj)
    : TaskDialog()
    , TrajectoryCompound(obj)
{
    select = new Gui::TaskView::TaskSelectLinkProperty(
        "SELECT Robot::TrajectoryObject COUNT 1..",
        &(obj->Source)
    );

    Content.push_back(select);
}

//==== calls from the TaskView ===============================================================


void TaskDlgTrajectoryCompound::open()
{
    select->activate();
}


bool TaskDlgTrajectoryCompound::accept()
{
    if (select->isSelectionValid()) {
        select->accept();
        TrajectoryCompound->execute();
        Gui::Document* doc = Gui::Application::Instance->activeDocument();
        if (doc) {
            doc->resetEdit();
        }
        return true;
    }
    else {
        QApplication::beep();
    }

    return false;
}

bool TaskDlgTrajectoryCompound::reject()
{
    select->reject();
    TrajectoryCompound->execute();
    Gui::Document* doc = Gui::Application::Instance->activeDocument();
    if (doc) {
        doc->resetEdit();
    }
    return true;
}

void TaskDlgTrajectoryCompound::helpRequested()
{}


#include "moc_TaskDlgTrajectoryCompound.cpp"
