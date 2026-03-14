#ifndef GUI_TASKVIEW_TaskDatumShapeBinder_H
#define GUI_TASKVIEW_TaskDatumShapeBinder_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>
#include <Gui/DocumentObserver.h>

#include "ViewProviderShapeBinder.h"

class Ui_TaskShapeBinder;
class QListWidget;

namespace App
{
class Property;
}

namespace Gui
{
class ButtonGroup;
class ViewProvider;
}  // namespace Gui

namespace PartDesignGui
{


class TaskShapeBinder: public Gui::TaskView::TaskBox, Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit TaskShapeBinder(
        ViewProviderShapeBinder* view,
        bool newObj = false,
        QWidget* parent = nullptr
    );
    ~TaskShapeBinder() override;

    void accept();

protected:
    enum selectionModes
    {
        none,
        refAdd,
        refRemove,
        refObjAdd
    };
    void changeEvent(QEvent* e) override;
    selectionModes selectionMode = none;

    void removeFromListWidget(QListWidget* w, QString name);
    bool referenceSelected(const Gui::SelectionChanges& msg) const;

private:
    void setupButtonGroup();
    void setupContextMenu();
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    void onButtonToggled(QAbstractButton* button, bool checked);
    void updateUI();
    void supportChanged(const QString&);
    void clearButtons();
    void deleteItem();
    void exitSelectionMode();

    bool supportShow = false;

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskShapeBinder> ui;
    Gui::ButtonGroup* buttonGroup;
    Gui::WeakPtrT<ViewProviderShapeBinder> vp;
};


/// simulation dialog for the TaskView
class TaskDlgShapeBinder: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskDlgShapeBinder(ViewProviderShapeBinder* view, bool newObj = false);
    ~TaskDlgShapeBinder() override;

public:
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
    /// is called by the framework if the dialog is rejected (Cancel)
    bool reject() override;

protected:
    TaskShapeBinder* parameter;
    Gui::WeakPtrT<ViewProviderShapeBinder> vp;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
