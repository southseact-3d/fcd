#ifndef GUI_TASKVIEW_TaskEdge2TracParameter_H
#define GUI_TASKVIEW_TaskEdge2TracParameter_H

#include <Gui/TaskView/TaskView.h>
#include <Mod/Robot/App/Edge2TracObject.h>


namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
}

namespace RobotGui
{

class Ui_TaskEdge2TracParameter;
class TaskEdge2TracParameter: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskEdge2TracParameter(Robot::Edge2TracObject* pcObject, QWidget* parent = nullptr);
    ~TaskEdge2TracParameter() override;

    void setEdgeAndClusterNbr(int NbrEdges, int NbrClusters);

private Q_SLOTS:
    void hideShow();
    void sizingValueChanged(double Value);
    void orientationToggled(bool Value);

protected:
    Robot::Edge2TracObject* pcObject;
    App::DocumentObject* HideShowObj;

    void setHideShowObject();

private:
private:
    QWidget* proxy;
    Ui_TaskEdge2TracParameter* ui;
};

}  // namespace RobotGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
