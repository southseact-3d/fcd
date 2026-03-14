#include <Base/Console.h>
#include <Base/Exception.h>
#include <Gui/Application.h>
#include <Gui/Command.h>
#include <Gui/Document.h>

#include "TaskDlgCreateElementSet.h"
#include "ViewProviderFemMesh.h"


using namespace FemGui;


//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgCreateElementSet::TaskDlgCreateElementSet(Fem::FemSetElementNodesObject* obj)
    : TaskDialog()
    , FemSetElementNodesObject(obj)
{
    name = new TaskObjectName(obj);
    param = new TaskCreateElementSet(obj);

    Content.push_back(name);
    Content.push_back(param);
}

TaskDlgCreateElementSet::~TaskDlgCreateElementSet()
{}

//==== calls from the TaskView ===============================================================


void TaskDlgCreateElementSet::open()
{
    // select->activate();
    // Edge2TaskObject->execute();
    // param->setEdgeAndClusterNbr(Edge2TaskObject->NbrOfEdges,Edge2TaskObject->NbrOfCluster);
}

bool TaskDlgCreateElementSet::accept()
{
    try {
        FemSetElementNodesObject->Elements.setValues(param->elementTempSet);
        FemSetElementNodesObject->recomputeFeature();
        param->MeshViewProvider->resetHighlightNodes();
        FemSetElementNodesObject->Label.setValue(name->name);
        Gui::Command::doCommand(Gui::Command::Gui, "Gui.activeDocument().resetEdit()");

        return true;
    }
    catch (const Base::Exception& e) {
        Base::Console().warning("TaskDlgCreateElementSet::accept(): %s\n", e.what());
    }

    return false;
}

bool TaskDlgCreateElementSet::reject()
{
    FemSetElementNodesObject->execute();
    param->MeshViewProvider->resetHighlightNodes();
    Gui::Command::abortCommand();
    Gui::Command::doCommand(Gui::Command::Gui, "Gui.activeDocument().resetEdit()");

    return true;
}

void TaskDlgCreateElementSet::helpRequested()
{}

#include "moc_TaskDlgCreateElementSet.cpp"
