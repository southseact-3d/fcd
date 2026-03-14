#ifndef FEMGUI_TaskAnalysisInfo_H
#define FEMGUI_TaskAnalysisInfo_H

#include <Gui/TaskView/TaskView.h>


class Ui_TaskAnalysisInfo;
class SoEventCallback;

namespace Base
{
class Polygon2d;
}
namespace App
{
class Property;
}

namespace Gui
{
class ViewProvider;
class ViewVolumeProjection;
}  // namespace Gui

namespace Fem
{
class FemAnalysis;
}

namespace FemGui
{

class ViewProviderFemMesh;


class TaskAnalysisInfo: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskAnalysisInfo(Fem::FemAnalysis* pcObject, QWidget* parent = nullptr);
    ~TaskAnalysisInfo() override;

protected:
    Fem::FemAnalysis* pcObject;

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskAnalysisInfo> ui;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskAnalysisInfo_H
