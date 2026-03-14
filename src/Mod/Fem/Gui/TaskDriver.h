#ifndef FEMGUI_TaskDriver_H
#define FEMGUI_TaskDriver_H

#include <Gui/TaskView/TaskView.h>

class Ui_TaskDriver;
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


class TaskDriver: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskDriver(Fem::FemAnalysis* pcObject, QWidget* parent = nullptr);
    ~TaskDriver() override;

protected:
    Fem::FemAnalysis* pcObject;

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskDriver> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_TaskDriver_H
