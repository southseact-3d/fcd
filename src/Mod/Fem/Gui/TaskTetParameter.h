#ifndef FEMGUI_TaskTetParameter_H
#define FEMGUI_TaskTetParameter_H

#include <Gui/TaskView/TaskView.h>


class Ui_TaskTetParameter;
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
class FemMeshShapeNetgenObject;
}

namespace FemGui
{

class ViewProviderFemMeshShapeNetgen;


class TaskTetParameter: public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskTetParameter(Fem::FemMeshShapeNetgenObject* pcObject, QWidget* parent = nullptr);
    ~TaskTetParameter() override;

    void setInfo();
    bool touched;

private Q_SLOTS:
    void SwitchMethod(int Value);
    void maxSizeValueChanged(double Value);
    void minSizeValueChanged(double Value);
    void setQuadric(int s);
    void setGrowthRate(double v);
    void setSegsPerEdge(int v);
    void setSegsPerRadius(int v);
    void setOptimize(int v);

protected:
    Fem::FemMeshShapeNetgenObject* pcObject;

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskTetParameter> ui;
};

}  // namespace FemGui

#endif  // FEMGUI_TaskTetParameter_H
