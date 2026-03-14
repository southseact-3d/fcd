#ifndef MESHGUI_VIEWPROVIDERMESHTRANSFORM_H
#define MESHGUI_VIEWPROVIDERMESHTRANSFORM_H

#include "ViewProvider.h"

class SoSeparator;
class SbVec3f;
class SoSwitch;
class SoCoordinate3;
class SoNormal;
class SoIndexedFaceSet;
class SoFaceSet;
class SoPath;
class SoLocateHighlight;
class SoTransformerManip;

namespace Gui
{
class View3DInventorViewer;
}

namespace MeshGui
{

/** Like Mesh viewprovider but with manipulator
 */
class ViewProviderMeshTransform: public ViewProviderMesh
{
    PROPERTY_HEADER_WITH_OVERRIDE(MeshGui::ViewProviderMeshTransform);

public:
    ViewProviderMeshTransform();
    ~ViewProviderMeshTransform() override;


    /**
     * Extracts the mesh data from the feature \a pcFeature and creates
     * an Inventor node \a SoNode with these data.
     */
    void attach(App::DocumentObject* obj) override;

    /// set the viewing mode
    void setDisplayMode(const char* ModeName) override;
    /// get the default display mode
    const char* getDefaultDisplayMode() const override;
    /// returns a list of all possible modes
    std::vector<std::string> getDisplayModes() const override;
    /// Update the Mesh representation
    void updateData(const App::Property*) override;

private:
    SoTransformerManip* pcTransformerDragger;

    FC_DISABLE_COPY_MOVE(ViewProviderMeshTransform)
};

}  // namespace MeshGui


#endif  // MESHGUI_VIEWPROVIDERMESHTRANSFORM_H
