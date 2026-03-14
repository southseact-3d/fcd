#ifndef MESHGUI_VIEWPROVIDERMESHFACESET_H
#define MESHGUI_VIEWPROVIDERMESHFACESET_H

#include <Mod/Mesh/Gui/ViewProvider.h>

namespace MeshGui
{
class SoFCIndexedFaceSet;

/**
 * The ViewProviderMeshFaceSet class creates nodes for representing the mesh
 * data structure. Depending on the size of the mesh it uses two ways to
 * render it:
 * - For huge meshes it renders directly the data structure. Rendering directly
 *   the data structure has the advantage to save memory by not creating the
 *   according OpenInventor nodes which would more or less duplicate the
 *   memory for a mesh. Especially for huge with several hundred thousands or
 *   even millions of triangles, the amount of saved memory is considerable.
 * - For all other meshes it uses the appropriate OpenInventor nodes. Although
 *   this needs more memory its usage is much more flexible. It offers several
 *   nice features like a smooth-shaded appearance of a mesh whereas the
 *   OpenInventor nodes are already capable to do everything automatically,
 *   or the usage with textures.
 *
 * For more details @see SoFCMeshNode and SoFCMeshFaceSet.
 * @author Werner Mayer
 */
class MeshGuiExport ViewProviderMeshFaceSet: public ViewProviderMesh
{
    PROPERTY_HEADER_WITH_OVERRIDE(MeshGui::ViewProviderMeshFaceSet);

public:
    ViewProviderMeshFaceSet();
    ~ViewProviderMeshFaceSet() override;

    void attach(App::DocumentObject* obj) override;
    void updateData(const App::Property* prop) override;

protected:
    void showOpenEdges(bool show) override;
    SoShape* getShapeNode() const override;
    SoNode* getCoordNode() const override;

private:
    bool directRendering;
    unsigned long triangleCount;
    SoCoordinate3* pcMeshCoord;
    SoFCIndexedFaceSet* pcMeshFaces;
    SoFCMeshObjectNode* pcMeshNode;
    SoFCMeshObjectShape* pcMeshShape;

    FC_DISABLE_COPY_MOVE(ViewProviderMeshFaceSet)
};

}  // namespace MeshGui


#endif  // MESHGUI_VIEWPROVIDERMESHFACESET_H
