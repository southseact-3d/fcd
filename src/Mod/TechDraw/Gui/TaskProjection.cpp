//this file originally part of TechDraw workbench
//migrated to TechDraw workbench 2022-01-26 by Wandererfan

# include <QMessageBox>


#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/Document.h>
#include <Gui/MainWindow.h>
#include <Gui/Selection/Selection.h>
#include <Gui/View3DInventor.h>
#include <Gui/View3DInventorViewer.h>
#include <Mod/Part/App/PartFeature.h>

#include "TaskProjection.h"
#include "ui_TaskProjection.h"


using namespace TechDrawGui;

/* TRANSLATOR TechDrawGui::TaskProjection */

TaskProjection::TaskProjection() :
    ui(new Ui_TaskProjection)
{

    ui->setupUi(this);
}

TaskProjection::~TaskProjection()
{
    // automatically deleted in the sub-class
}

bool TaskProjection::accept()
{
    Gui::Document* document = Gui::Application::Instance->activeDocument();
    if (!document) {
        QMessageBox::warning(Gui::getMainWindow(), tr("No Active Document"),
            tr("There is currently no active document to complete the operation"));
        return true;
    }
    std::list<Gui::MDIView*> mdis = document->getMDIViewsOfType(Gui::View3DInventor::getClassTypeId());
    if (mdis.empty()) {
        QMessageBox::warning(Gui::getMainWindow(), tr("No Active View"),
            tr("There is currently no active view to complete the operation"));
        return false;
    }

    Gui::View3DInventorViewer* viewer = static_cast<Gui::View3DInventor*>(mdis.front())->getViewer();
    SbVec3f pnt, dir;
    viewer->getNearPlane(pnt, dir);
    float x=0, y=1, z=1;
    dir.getValue(x, y,z);

    std::vector<Part::Feature*> shapes = Gui::Selection().getObjectsOfType<Part::Feature>();
    Gui::Command::openCommand("Project shape");
    Gui::Command::addModule(Gui::Command::Doc, "TechDraw");
    for (std::vector<Part::Feature*>::iterator it = shapes.begin(); it != shapes.end(); ++it) {
        const char* object = (*it)->getNameInDocument();
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.addObject('TechDraw::FeatureProjection', '%s_proj')", object);
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.Direction=FreeCAD.Vector(%f, %f, %f)", x, y,z);
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.Source=FreeCAD.ActiveDocument.%s", object);
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.VCompound=%s", (ui->cbVisSharp->isChecked() ? "True" : "False"));
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.Rg1LineVCompound=%s", (ui->cbVisSmooth->isChecked() ? "True" : "False"));
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.RgNLineVCompound=%s", (ui->cbVisSewn->isChecked() ? "True" : "False"));
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.OutLineVCompound=%s", (ui->cbVisOutline->isChecked() ? "True" : "False"));
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.IsoLineVCompound=%s", (ui->cbVisIso->isChecked() ? "True" : "False"));
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.HCompound=%s", (ui->cbHidSharp->isChecked() ? "True" : "False"));
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.Rg1LineHCompound=%s", (ui->cbHidSmooth->isChecked() ? "True" : "False"));
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.RgNLineHCompound=%s", (ui->cbHidSewn->isChecked() ? "True" : "False"));
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.OutLineHCompound=%s", (ui->cbHidOutline->isChecked() ? "True" : "False"));
        Gui::Command::doCommand(Gui::Command::Doc,
            "FreeCAD.ActiveDocument.ActiveObject.IsoLineHCompound=%s", (ui->cbHidIso->isChecked() ? "True" : "False"));
    }
    Gui::Command::updateActive();
    Gui::Command::commitCommand();
    return true;
}

bool TaskProjection::reject()
{
    return true;
}


///////////////////////////////////////////////////////////////////////////

TaskDlgProjection::TaskDlgProjection() :
    TaskDialog()
{
    widget  = new TaskProjection();
    taskbox = new Gui::TaskView::TaskBox(Gui::BitmapFactory().pixmap("actions/TechDraw_ProjectShape"), widget->windowTitle(), true, nullptr);
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
    setAutoCloseOnTransactionChange(true);
}

TaskDlgProjection::~TaskDlgProjection()
{
}

void TaskDlgProjection::update()
{
}

//==== calls from the TaskView ===============================================================
void TaskDlgProjection::open()
{
}

void TaskDlgProjection::clicked(int i)
{
    Q_UNUSED(i);
}

bool TaskDlgProjection::accept()
{
    widget->accept();
    return true;
}

bool TaskDlgProjection::reject()
{
    widget->reject();
    return true;
}

#include "moc_TaskProjection.cpp"
