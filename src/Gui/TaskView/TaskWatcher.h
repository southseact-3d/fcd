#ifndef GUI_TASKVIEW_TASKWATCHER_H
#define GUI_TASKVIEW_TASKWATCHER_H

#include <vector>
#include <QObject>

#include <Gui/Selection/SelectionFilter.h>


namespace Gui
{
namespace TaskView
{

class TaskContent;

/// Father class of watcher classes
class GuiExport TaskWatcher: public QObject, public Gui::SelectionFilter
{
    Q_OBJECT

public:
    explicit TaskWatcher(const char* Filter);
    ~TaskWatcher() override;

    QWidget* addTaskBox(QWidget* widget, bool expandable = true, QWidget* parent = nullptr);
    QWidget* addTaskBox(
        const QPixmap& icon,
        QWidget* widget,
        bool expandable = true,
        QWidget* parent = nullptr
    );
    QWidget* addTaskBoxWithoutHeader(QWidget* widget);

    std::vector<QWidget*>& getWatcherContent();

public:
    /// is called wenn the document or the Selection changes.
    virtual bool shouldShow();

protected:
    /// List of TaskBoxes of that dialog
    std::vector<QWidget*> Content;
};

// --------------------------------------------------------------------------

/// Special watcher class for showing commands dependene on the selection
class GuiExport TaskWatcherCommands: public TaskWatcher
{
    Q_OBJECT

public:
    TaskWatcherCommands(const char* Filter, const char* commands[], const char* name, const char* pixmap);

public:
    /// is called wenn the document or the Selection changes.
    bool shouldShow() override;
};

// --------------------------------------------------------------------------

/// Special watcher class for showing commands when active document is empty
class GuiExport TaskWatcherCommandsEmptyDoc: public TaskWatcherCommands
{
    Q_OBJECT

public:
    TaskWatcherCommandsEmptyDoc(const char* commands[], const char* name, const char* pixmap);

public:
    /// is called wenn the document or the Selection changes.
    bool shouldShow() override;
};

// --------------------------------------------------------------------------

/// Special watcher class for showing commands when there is nothing selected
class GuiExport TaskWatcherCommandsEmptySelection: public TaskWatcherCommands
{
    Q_OBJECT

public:
    TaskWatcherCommandsEmptySelection(const char* commands[], const char* name, const char* pixmap);
    ~TaskWatcherCommandsEmptySelection() override;

public:
    /// is called wenn the document or the Selection changes.
    bool shouldShow() override;
};


}  // namespace TaskView
}  // namespace Gui

#endif  // GUI_TASKVIEW_TASKWATCHER_H
