#ifndef PARTGUI_TASKSWEEP_H
#define PARTGUI_TASKSWEEP_H

#include <Gui/TaskView/TaskView.h>
#include <Gui/TaskView/TaskDialog.h>

class QTreeWidgetItem;

namespace Gui
{
class SelectionObject;
class StatusWidget;
}  // namespace Gui
namespace PartGui
{

class SweepWidget: public QWidget
{
    Q_OBJECT

public:
    explicit SweepWidget(QWidget* parent = nullptr);
    ~SweepWidget() override;

    bool accept();
    bool reject();

private:
    void onCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*);
    void onButtonPathToggled(bool);

private:
    void changeEvent(QEvent* e) override;
    void findShapes();
    bool isPathValid(const Gui::SelectionObject& sel) const;

private:
    class Private;
    Private* d;
};

class TaskSweep: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskSweep();
    ~TaskSweep() override;

public:
    void open() override;
    bool accept() override;
    bool reject() override;
    void clicked(int) override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help;
    }

private:
    SweepWidget* widget;
    Gui::StatusWidget* label;
};

}  // namespace PartGui

#endif  // PARTGUI_TASKSWEEP_H
