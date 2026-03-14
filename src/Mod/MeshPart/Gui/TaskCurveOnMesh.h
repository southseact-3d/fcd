#ifndef MESHPART_GUI_TASKCURVEONMESH_H
#define MESHPART_GUI_TASKCURVEONMESH_H

#include <QPointer>

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>


namespace Gui
{
class View3DInventor;
}

namespace MeshPartGui
{

class Ui_TaskCurveOnMesh;
class CurveOnMeshHandler;

class CurveOnMeshWidget: public QWidget
{
    Q_OBJECT

public:
    explicit CurveOnMeshWidget(Gui::View3DInventor* view, QWidget* parent = nullptr);
    ~CurveOnMeshWidget() override;

    void reject();

protected:
    void changeEvent(QEvent* e) override;
    void setup();

private:
    void onStartButtonClicked();

private:
    Ui_TaskCurveOnMesh* ui;
    CurveOnMeshHandler* myCurveHandler;
    QPointer<Gui::View3DInventor> myView;
};

class TaskCurveOnMesh: public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    explicit TaskCurveOnMesh(Gui::View3DInventor* view);

public:
    bool reject() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Close;
    }

private:
    CurveOnMeshWidget* widget;
};

}  // namespace MeshPartGui

#endif  // MESHPART_GUI_TASKCURVEONMESH_H
