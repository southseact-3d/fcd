#ifndef GUI_TASKVIEW_TaskCreateNodeSet_H
#define GUI_TASKVIEW_TaskCreateNodeSet_H

#include <Gui/TaskView/TaskView.h>
#include <Mod/Fem/App/FemSetNodesObject.h>


class Ui_TaskCreateNodeSet;
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

namespace FemGui
{

class ViewProviderFemMesh;


class TaskCreateNodeSet: public Gui::TaskView::TaskBox, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit TaskCreateNodeSet(Fem::FemSetNodesObject* pcObject, QWidget* parent = nullptr);
    ~TaskCreateNodeSet() override;

    std::set<long> tempSet;
    ViewProviderFemMesh* MeshViewProvider;

private Q_SLOTS:
    void Poly();
    void Pick();
    void SwitchMethod(int Value);

protected:
    Fem::FemSetNodesObject* pcObject;
    static void DefineNodesCallback(void* ud, SoEventCallback* n);
    void DefineNodes(const Base::Polygon2d& polygon, const Gui::ViewVolumeProjection& proj, bool);

protected:
    void onSelectionChanged(const Gui::SelectionChanges& msg) override;
    enum selectionModes
    {
        none,
        PickElement
    } selectionMode;

private:
    QWidget* proxy;
    std::unique_ptr<Ui_TaskCreateNodeSet> ui;
};

}  // namespace FemGui

#endif  // GUI_TASKVIEW_TaskCreateNodeSet_H
