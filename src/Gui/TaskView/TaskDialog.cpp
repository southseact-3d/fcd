#include <QMessageBox>


#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/MainWindow.h>
#include <Gui/View3DInventor.h>
#include <Gui/ViewProviderDocumentObject.h>


#include "TaskDialog.h"
#include "TaskView.h"

using namespace Gui::TaskView;


//**************************************************************************
//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDialog::TaskDialog()
    : QObject(nullptr)
    , pos(North)
    , escapeButton(true)
    , autoCloseTransaction(false)
    , autoCloseDeletedDocument(false)
    , autoCloseClosedView(false)
{}

TaskDialog::~TaskDialog()
{
    for (auto it : Content) {
        delete it;
        it = nullptr;
    }
}

//==== Slots ===============================================================

QWidget* TaskDialog::addTaskBox(QWidget* widget, bool expandable, QWidget* parent)
{
    return addTaskBox(QPixmap(), widget, expandable, parent);
}

QWidget* TaskDialog::addTaskBox(const QPixmap& icon, QWidget* widget, bool expandable, QWidget* parent)
{
    auto taskbox = new Gui::TaskView::TaskBox(icon, widget->windowTitle(), expandable, parent);
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
    return taskbox;
}

QWidget* TaskDialog::addTaskBoxWithoutHeader(QWidget* widget)
{
    auto taskbox = new Gui::TaskView::TaskBox();
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
    return taskbox;
}

const std::vector<QWidget*>& TaskDialog::getDialogContent() const
{
    return Content;
}

bool TaskDialog::canClose() const
{
    QMessageBox msgBox(Gui::getMainWindow());
    msgBox.setText(tr("A dialog is already open in the task panel"));
    msgBox.setInformativeText(QObject::tr("Close this dialog?"));
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();
    return (ret == QMessageBox::Yes);
}

void TaskDialog::associateToObject3dView(App::DocumentObject* obj)
{
    if (!obj) {
        return;
    }

    Gui::Document* guiDoc = Gui::Application::Instance->getDocument(obj->getDocument());
    auto* vp = Gui::Application::Instance->getViewProvider(obj);
    auto* vpdo = static_cast<Gui::ViewProviderDocumentObject*>(vp);
    auto* view = guiDoc->openEditingView3D(vpdo);

    if (!view) {
        return;
    }

    setAssociatedView(view);
    setAutoCloseOnClosedView(true);
}

//==== calls from the TaskView ===============================================================

void TaskDialog::open()
{}

void TaskDialog::closed()
{}

void TaskDialog::autoClosedOnTransactionChange()
{}

void TaskDialog::autoClosedOnDeletedDocument()
{}

void TaskDialog::autoClosedOnClosedView()
{}

void TaskDialog::clicked(int)
{}

bool TaskDialog::accept()
{
    return true;
}

bool TaskDialog::reject()
{
    return true;
}

void TaskDialog::helpRequested()
{}

void TaskDialog::onUndo()
{}

void TaskDialog::onRedo()
{}


#include "moc_TaskDialog.cpp"
