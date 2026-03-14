#ifndef FemGui_TaskCreateElementSet_H
#define FemGui_TaskCreateElementSet_H

#include <Gui/TaskView/TaskView.h>
#include <Mod/Fem/App/FemSetElementNodesObject.h>
#include <QMessageBox>


class Ui_TaskCreateElementSet;
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


class TaskCreateElementSet: public Gui::TaskView::TaskBox, public Gui::SelectionObserver
{
    Q_OBJECT

public:
    explicit TaskCreateElementSet(Fem::FemSetElementNodesObject* pcObject, QWidget* parent = nullptr);
    ~TaskCreateElementSet() override;

    std::set<long> elementTempSet;
    ViewProviderFemMesh* MeshViewProvider;
    static std::string currentProject;

private Q_SLOTS:
    void Poly();
    void Restore();
    void CopyResultsMesh();

protected:
    Fem::FemSetElementNodesObject* pcObject;
    static void DefineElementsCallback(void* ud, SoEventCallback* n);
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
    Ui_TaskCreateElementSet* ui;
};

}  // namespace FemGui

#endif  // FemGui_TaskCreateElementSet_H
