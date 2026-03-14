#ifndef GUI_TASKVIEW_TaskTrajectoryDressUpParameter_h
#define GUI_TASKVIEW_TaskTrajectoryDressUpParameter_h

#include <Gui/TaskView/TaskView.h>
#include <Mod/Robot/App/TrajectoryDressUpObject.h>


namespace RobotGui
{

class Ui_TaskTrajectoryDressUpParameter;
class TaskTrajectoryDressUpParameter: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskTrajectoryDressUpParameter(
        Robot::TrajectoryDressUpObject* obj,
        QWidget* parent = nullptr
    );
    ~TaskTrajectoryDressUpParameter() override;

    /// this methode write the values from the Gui to the object, usually in accept()
    void writeValues();

private Q_SLOTS:
    /// edit the placement
    void createPlacementDlg();


protected:
    Base::Placement PosAdd;

    void viewPlacement();

private:
    QWidget* proxy;
    Ui_TaskTrajectoryDressUpParameter* ui;
    Robot::TrajectoryDressUpObject* pcObject;
};

}  // namespace RobotGui

#endif  // GUI_TASKVIEW_TASKAPPERANCE_H
