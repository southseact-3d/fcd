#ifndef GUI_TASKVIEW_TASKAPPERANCE_H
#define GUI_TASKVIEW_TASKAPPERANCE_H

#include <Gui/Selection/Selection.h>
#include "TaskView.h"


namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;

namespace TaskView
{

using TaskAppearance_Connection = fastsignals::connection;
class Ui_TaskAppearance;

class TaskAppearance: public TaskBox, public Gui::SelectionSingleton::ObserverType
{
    Q_OBJECT

public:
    explicit TaskAppearance(QWidget* parent = nullptr);
    ~TaskAppearance() override;
    /// Observer message from the Selection
    void OnChange(
        Gui::SelectionSingleton::SubjectType& rCaller,
        Gui::SelectionSingleton::MessageType Reason
    ) override;

private Q_SLOTS:
    void setupConnections();
    void onChangeModeActivated(const QString&);
    void onChangePlotActivated(const QString&);
    void onTransparencyValueChanged(int);
    void onPointSizeValueChanged(int);
    void onLineWidthValueChanged(int);

protected:
    void changeEvent(QEvent* e) override;

private:
    void slotChangedObject(const Gui::ViewProvider&, const App::Property& Prop);
    void setDisplayModes(const std::vector<Gui::ViewProvider*>&);
    void setPointSize(const std::vector<Gui::ViewProvider*>&);
    void setLineWidth(const std::vector<Gui::ViewProvider*>&);
    void setTransparency(const std::vector<Gui::ViewProvider*>&);
    std::vector<Gui::ViewProvider*> getSelection() const;

private:
    QWidget* proxy;
    Ui_TaskAppearance* ui;
    TaskAppearance_Connection connectChangedObject;
};

}  // namespace TaskView
}  // namespace Gui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
