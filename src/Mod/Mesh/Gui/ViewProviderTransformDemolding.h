#ifndef MESGUI_VIEWPROVIDERMESHTRANSFORMDEMOLDING_H
#define MESGUI_VIEWPROVIDERMESHTRANSFORMDEMOLDING_H

#include "ViewProvider.h"


class SoSeparator;
class SbVec3f;
class SoSwitch;
class SoCoordinate3;
class SoDragger;
class SoNormal;
class SoIndexedFaceSet;
class SoFaceSet;
class SoPath;
class SoLocateHighlight;
class SbRotation;
class SoTrackballDragger;
class SoTransformerManip;

namespace Gui
{
class View3DInventorViewer;
}

namespace MeshGui
{

/** Like Mesh viewprovider but with manipulator
 */
class ViewProviderMeshTransformDemolding: public ViewProviderMesh
{
    PROPERTY_HEADER_WITH_OVERRIDE(MeshGui::ViewProviderMeshTransformDemolding);

public:
    ViewProviderMeshTransformDemolding();
    ~ViewProviderMeshTransformDemolding() override;


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

protected:
    void setCenterPoint();
    void calcMaterialIndex(const SbRotation& rot);
    void calcNormalVector();

    static void sValueChangedCallback(void*, SoDragger*);
    void valueChangedCallback();

    static void sDragEndCallback(void*, SoDragger*);
    void DragEndCallback();

private:
    SoTrackballDragger* pcTrackballDragger;
    SoTransform* pcTransformDrag;
    SoMaterial* pcColorMat;
    std::vector<SbVec3f> normalVector;
    Base::Vector3f center;

    FC_DISABLE_COPY_MOVE(ViewProviderMeshTransformDemolding)
};

}  // namespace MeshGui


#endif  // MESGUI_VIEWPROVIDERMESHTRANSFORMDEMOLDING_H
