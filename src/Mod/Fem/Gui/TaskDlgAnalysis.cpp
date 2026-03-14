#include <Base/Console.h>
#include <Base/Exception.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Mod/Fem/App/FemAnalysis.h>

#include "TaskAnalysisInfo.h"
#include "TaskDlgAnalysis.h"
#include "TaskDriver.h"


using namespace FemGui;

//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgAnalysis::TaskDlgAnalysis(Fem::FemAnalysis* obj)
    : TaskDialog()
    , FemAnalysis(obj)
{
    driver = new TaskDriver(obj);
    info = new TaskAnalysisInfo(obj);

    Content.push_back(driver);
    Content.push_back(info);
}

TaskDlgAnalysis::~TaskDlgAnalysis() = default;

//==== calls from the TaskView ===============================================================


void TaskDlgAnalysis::open()
{
    // select->activate();
    // Edge2TaskObject->execute();
    // param->setEdgeAndClusterNbr(Edge2TaskObject->NbrOfEdges,Edge2TaskObject->NbrOfCluster);
}

bool TaskDlgAnalysis::accept()
{
    // try {
    //     FemSetNodesObject->Nodes.setValues(param->tempSet);
    //     FemSetNodesObject->recompute();
    //     //Gui::Document* doc = Gui::Application::Instance->activeDocument();
    //     //if(doc)
    //     //    doc->resetEdit();
    //     param->MeshViewProvider->resetHighlightNodes();
    //     FemSetNodesObject->Label.setValue(name->name);
    //     Gui::Command::doCommand(Gui::Command::Gui,"Gui.activeDocument().resetEdit()");

    //    return true;
    //}
    // catch (const Base::Exception& e) {
    //    Base::Console().warning("TaskDlgAnalysis::accept(): %s\n", e.what());
    //}

    return false;
}

bool TaskDlgAnalysis::reject()
{
    // FemSetNodesObject->execute();
    //     //Gui::Document* doc = Gui::Application::Instance->activeDocument();
    //     //if(doc)
    //     //    doc->resetEdit();
    // param->MeshViewProvider->resetHighlightNodes();
    // Gui::Command::abortCommand();
    // Gui::Command::doCommand(Gui::Command::Gui,"Gui.activeDocument().resetEdit()");

    return true;
}

void TaskDlgAnalysis::helpRequested()
{}

#include "moc_TaskDlgAnalysis.cpp"
