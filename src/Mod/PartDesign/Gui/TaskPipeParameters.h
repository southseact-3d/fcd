#ifndef GUI_TASKVIEW_TaskPipeParameters_H
#define GUI_TASKVIEW_TaskPipeParameters_H

#include "TaskSketchBasedParameters.h"
#include "ViewProviderPipe.h"
#include "TaskDressUpParameters.h"


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

class Ui_TaskPipeParameters;
class Ui_TaskPipeOrientation;
class Ui_TaskPipeScaling;

/// Convenience class to maintain states between the various task boxes for pipe
class StateHandlerTaskPipe
{
public:
    enum SelectionModes
    {
        none = 0,
        refProfile,
        refSpine,
        refSpineEdgeAdd,
        refSpineEdgeRemove,
        refAuxSpine,
        refAuxSpineEdgeAdd,
        refAuxSpineEdgeRemove,
        refSectionAdd,
        refSectionRemove
    };

public:
    StateHandlerTaskPipe()
    {
        selectionMode = SelectionModes::none;
    }
    ~StateHandlerTaskPipe() = default;

    // only keeping getter because task boxes shouldn't need to change this
    // and task dialog is already friend
    enum SelectionModes getSelectionMode()
    {
        return selectionMode;
    }

private:
    enum SelectionModes selectionMode;
    friend class TaskDlgPipeParameters;
};


class TaskPipeParameters: public TaskSketchBasedParameters
{
    Q_OBJECT

public:
    explicit TaskPipeParameters(
        ViewProviderPipe* PipeView,
        bool newObj = false,
        QWidget* parent = nullptr
    );
    ~TaskPipeParameters() override;

    bool accept();

private Q_SLOTS:
    void onTangentChanged(bool checked);
    void onTransitionChanged(int);
    void onProfileButton(bool checked);
    void onDeleteEdge();

protected:
    void removeFromListWidget(QListWidget* w, QString name);
    bool referenceSelected(const Gui::SelectionChanges& msg) const;

private:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    void updateUI();
    void clearButtons();
    void exitSelectionMode();
    void setVisibilityOfSpineAndProfile();

    bool spineShow = false;
    bool profileShow = false;
    bool auxSpineShow = false;

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskPipeParameters> ui;
    StateHandlerTaskPipe* stateHandler;
    friend class TaskDlgPipeParameters;
};

class TaskPipeOrientation: public TaskSketchBasedParameters
{
    Q_OBJECT

public:
    explicit TaskPipeOrientation(
        ViewProviderPipe* PipeView,
        bool newObj = false,
        QWidget* parent = nullptr
    );
    ~TaskPipeOrientation() override;


private Q_SLOTS:
    void onOrientationChanged(int);
    void updateUI(int idx);
    void onClearButton();
    void onCurvilinearChanged(bool checked);
    void onBinormalChanged(double);
    void onDeleteItem();

protected:
    void removeFromListWidget(QListWidget* w, QString name);
    bool referenceSelected(const Gui::SelectionChanges& msg) const;

private:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    void clearButtons();
    void exitSelectionMode();

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskPipeOrientation> ui;
    StateHandlerTaskPipe* stateHandler;
    friend class TaskDlgPipeParameters;
};


class TaskPipeScaling: public TaskSketchBasedParameters
{
    Q_OBJECT

public:
    explicit TaskPipeScaling(ViewProviderPipe* PipeView, bool newObj = false, QWidget* parent = nullptr);
    ~TaskPipeScaling() override;

private Q_SLOTS:
    void onScalingChanged(int);
    void updateUI(int idx);
    void onDeleteSection();
    void indexesMoved();

protected:
    void removeFromListWidget(QListWidget* w, QString name);
    bool referenceSelected(const Gui::SelectionChanges& msg) const;

private:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    void clearButtons();
    void exitSelectionMode();

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskPipeScaling> ui;
    StateHandlerTaskPipe* stateHandler;
    friend class TaskDlgPipeParameters;
};

/// simulation dialog for the TaskView
class TaskDlgPipeParameters: public TaskDlgSketchBasedParameters
{
    Q_OBJECT

public:
    explicit TaskDlgPipeParameters(ViewProviderPipe* PipeView, bool newObj = false);
    ~TaskDlgPipeParameters() override;

public:
    /// is called by the framework if the dialog is accepted (Ok)
    bool accept() override;
    /// is called by the framework if the dialog is rejected (Cancel)

protected Q_SLOTS:
    void onButtonToggled(QAbstractButton* button, bool checked);

protected:
    TaskPipeParameters* parameter;
    TaskPipeOrientation* orientation;
    TaskPipeScaling* scaling;

    Gui::ButtonGroup* buttonGroup;
    StateHandlerTaskPipe* stateHandler;
};

}  // namespace PartDesignGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
