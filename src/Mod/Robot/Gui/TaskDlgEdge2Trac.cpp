#include <QApplication>


#include <Base/Console.h>
#include <Base/Exception.h>
#include <Gui/Application.h>
#include <Gui/Document.h>

#include <Gui/TaskView/TaskSelectLinkProperty.h>

#include "TaskDlgEdge2Trac.h"


using namespace RobotGui;

//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgEdge2Trac::TaskDlgEdge2Trac(Robot::Edge2TracObject* obj)
    : TaskDialog()
    , Edge2TaskObject(obj)
{
    param = new TaskEdge2TracParameter(obj);
    select = new Gui::TaskView::TaskSelectLinkProperty(
        "SELECT Part::Feature SUBELEMENT Edge COUNT 1..",
        &(obj->Source)
    );

    Content.push_back(param);
    Content.push_back(select);
}

//==== calls from the TaskView ===============================================================


void TaskDlgEdge2Trac::open()
{
    select->activate();
    Edge2TaskObject->execute();
    param->setEdgeAndClusterNbr(Edge2TaskObject->NbrOfEdges, Edge2TaskObject->NbrOfCluster);
}

void TaskDlgEdge2Trac::clicked(int button)
{
    try {
        if (QDialogButtonBox::Apply == button) {
            if (select->isSelectionValid()) {
                select->sendSelection2Property();
                // May throw an exception which we must handle here
                Edge2TaskObject->execute();
                param->setEdgeAndClusterNbr(Edge2TaskObject->NbrOfEdges, Edge2TaskObject->NbrOfCluster);
            }
            else {
                QApplication::beep();
                param->setEdgeAndClusterNbr(0, 0);
            }
        }
    }
    catch (const Base::Exception& e) {
        Base::Console().warning("TaskDlgEdge2Trac::clicked(): %s\n", e.what());
    }
}

bool TaskDlgEdge2Trac::accept()
{
    try {
        if (select->isSelectionValid()) {
            select->accept();
            Edge2TaskObject->recomputeFeature();
            Gui::Document* doc = Gui::Application::Instance->activeDocument();
            if (doc) {
                doc->resetEdit();
            }
            return true;
        }
        else {
            QApplication::beep();
        }
    }
    catch (const Base::Exception& e) {
        Base::Console().warning("TaskDlgEdge2Trac::accept(): %s\n", e.what());
    }

    return false;
}

bool TaskDlgEdge2Trac::reject()
{
    select->reject();
    Edge2TaskObject->execute();
    return true;
}

void TaskDlgEdge2Trac::helpRequested()
{}


#include "moc_TaskDlgEdge2Trac.cpp"
