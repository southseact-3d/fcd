#include <QObject>
#include <App/Application.h>
#include <App/Document.h>
#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/Command.h>
#include <Gui/TaskView/TaskView.h>

#include "TaskWatcher.h"


using namespace Gui::TaskView;


//**************************************************************************
//**************************************************************************
// TaskWatcher
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskWatcher::TaskWatcher(const char* Filter)
    : QObject(nullptr)
    , SelectionFilter(Filter)
{}

TaskWatcher::~TaskWatcher()
{
    for (auto it : Content) {
        delete it;
        it = nullptr;
    }
}

//==== implementer ===========================================================================

QWidget* TaskWatcher::addTaskBox(QWidget* widget, bool expandable, QWidget* parent)
{
    return addTaskBox(QPixmap(), widget, expandable, parent);
}

QWidget* TaskWatcher::addTaskBox(const QPixmap& icon, QWidget* widget, bool expandable, QWidget* parent)
{
    auto taskbox = new Gui::TaskView::TaskBox(icon, widget->windowTitle(), expandable, parent);
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
    return taskbox;
}

QWidget* TaskWatcher::addTaskBoxWithoutHeader(QWidget* widget)
{
    auto taskbox = new Gui::TaskView::TaskBox();
    taskbox->groupLayout()->addWidget(widget);
    Content.push_back(taskbox);
    return taskbox;
}

std::vector<QWidget*>& TaskWatcher::getWatcherContent()
{
    return Content;
}

//==== calls from the TaskView ===============================================================

bool TaskWatcher::shouldShow()
{
    return false;
}

//**************************************************************************
//**************************************************************************
// TaskWatcherCommands
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskWatcherCommands::TaskWatcherCommands(
    const char* Filter,
    const char* commands[],
    const char* name,
    const char* pixmap
)
    : TaskWatcher(Filter)
{
    if (commands) {
        CommandManager& mgr = Gui::Application::Instance->commandManager();
        auto tb = new Gui::TaskView::TaskBox(BitmapFactory().pixmap(pixmap), tr(name), true, nullptr);

        for (const char** i = commands; *i; i++) {
            Command* c = mgr.getCommandByName(*i);
            if (c) {
                // handled in TaskBox::actionEvent()
                c->addTo(tb);
            }
        }

        Content.push_back(tb);
    }
}


//==== implementer ===========================================================================


//==== calls from the TaskView ===============================================================

bool TaskWatcherCommands::shouldShow()
{
    return match();
}

//**************************************************************************
//**************************************************************************
// TaskWatcherCommandsEmptyDoc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskWatcherCommandsEmptyDoc::TaskWatcherCommandsEmptyDoc(
    const char* commands[],
    const char* name,
    const char* pixmap
)
    : TaskWatcherCommands(nullptr, commands, name, pixmap)
{}


//==== implementer ===========================================================================


//==== calls from the TaskView ===============================================================

bool TaskWatcherCommandsEmptyDoc::shouldShow()
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    return doc && doc->countObjects() == 0;
}

//**************************************************************************
//**************************************************************************
// TaskWatcherCommandsEmptySelection
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskWatcherCommandsEmptySelection::TaskWatcherCommandsEmptySelection(
    const char* commands[],
    const char* name,
    const char* pixmap
)
    : TaskWatcherCommands(nullptr, commands, name, pixmap)
{}

TaskWatcherCommandsEmptySelection::~TaskWatcherCommandsEmptySelection() = default;

//==== implementer ===========================================================================


//==== calls from the TaskView ===============================================================

bool TaskWatcherCommandsEmptySelection::shouldShow()
{
    return (App::GetApplication().getActiveDocument() && Gui::Selection().size() == 0);
}


#include "moc_TaskWatcher.cpp"
