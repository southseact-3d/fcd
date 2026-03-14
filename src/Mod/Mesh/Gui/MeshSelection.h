#ifndef MESHGUI_MESHSELECTION_H
#define MESHGUI_MESHSELECTION_H

#include <vector>
#include <QWidget>

#include <Gui/Selection/SelectionObject.h>
#include <Inventor/nodes/SoEventCallback.h>
#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif


namespace Gui
{
class View3DInventorViewer;
}

namespace MeshGui
{

class ViewProviderMesh;

class MeshGuiExport MeshSelection
{
public:
    MeshSelection();
    ~MeshSelection();

    MeshSelection(const MeshSelection&) = delete;
    MeshSelection(MeshSelection&&) = delete;
    MeshSelection& operator=(const MeshSelection&) = delete;
    MeshSelection& operator=(MeshSelection&&) = delete;

    void setEnabledViewerSelection(bool);

    void startSelection();
    void startDeselection();
    void stopSelection();
    bool deleteSelection();
    bool deleteSelectionBorder();
    void fullSelection();
    void clearSelection();
    void invertSelection();

    void selectComponent(int);
    void deselectComponent(int);
    void selectTriangle();
    void deselectTriangle();

    void setCheckOnlyPointToUserTriangles(bool);
    bool isCheckedOnlyPointToUserTriangles() const;
    void setCheckOnlyVisibleTriangles(bool);
    bool isCheckedOnlyVisibleTriangles() const;
    void setAddComponentOnClick(bool);
    void setRemoveComponentOnClick(bool);
    void setObjects(const std::vector<Gui::SelectionObject>&);
    std::vector<App::DocumentObject*> getObjects() const;
    void setViewer(Gui::View3DInventorViewer* v);

protected:
    void setCallback(SoEventCallbackCB* cb);
    std::list<ViewProviderMesh*> getViewProviders() const;
    Gui::View3DInventorViewer* getViewer() const;
    void prepareFreehandSelection(bool, SoEventCallbackCB* cb);
    void startInteractiveCallback(Gui::View3DInventorViewer* viewer, SoEventCallbackCB* cb);
    void stopInteractiveCallback(Gui::View3DInventorViewer* viewer);

private:
    static void selectGLCallback(void* ud, SoEventCallback* n);
    static void pickFaceCallback(void* ud, SoEventCallback* n);

private:
    bool onlyPointToUserTriangles {false};
    bool onlyVisibleTriangles {false};
    bool addToSelection {false};
    bool addComponent {false};
    bool removeComponent {false};
    SoEventCallbackCB* activeCB {nullptr};
    SoEventCallbackCB* selectionCB {nullptr};
    Gui::View3DInventorViewer* ivViewer {nullptr};
    mutable std::vector<Gui::SelectionObject> meshObjects;

    static unsigned char cross_bitmap[];
    static unsigned char cross_mask_bitmap[];
};

}  // namespace MeshGui

#endif  // MESHGUI_MESHSELECTION_H
