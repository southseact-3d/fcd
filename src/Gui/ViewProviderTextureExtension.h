#ifndef GUI_VIEWPROVIDERTEXTUREEXT_H
#define GUI_VIEWPROVIDERTEXTUREEXT_H

#include <Gui/ViewProviderExtensionPython.h>

class SoCoordinate3;
class SoGroup;
class SoIndexedFaceSet;
class SoMaterial;
class SoSwitch;
class SoTexture2;
class SoTexture3;

namespace App
{
class Material;
}

namespace Gui
{

class GuiExport ViewProviderTextureExtension: public Gui::ViewProviderExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderTextureExtension);

public:
    /// Constructor
    ViewProviderTextureExtension();
    ~ViewProviderTextureExtension() override;
    void setup(SoMaterial*);
    void setCoinAppearance(SoMaterial* pcShapeMaterial, const App::Material& source);
    SoSwitch* getAppearance() const;
    SoGroup* getTextureGroup3D() const;

    /**
     * Select which appearance type is active
     *
     */
    /** Material only */
    void activateMaterial();
    /** 2D Texture */
    void activateTexture2D();
    /** 3D texture only */
    void activateTexture3D();
    /** Mix of material and 3D textures */
    void activateMixed3D();

private:
    SoSwitch* pcSwitchAppearance {nullptr};
    SoSwitch* pcSwitchTexture {nullptr};
    SoTexture2* pcShapeTexture2D {nullptr};
    SoGroup* pcTextureGroup3D {nullptr};
};

class GuiExport ViewProviderFaceTexture: public Gui::ViewProviderTextureExtension
{
    EXTENSION_PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderFaceTexture);

public:
    /// Constructor
    ViewProviderFaceTexture();
    ~ViewProviderFaceTexture() override;
    void setup(SoMaterial*);

private:
    // Used to support per face textures
    SoTexture3* pcShapeTexture3D;
    SoCoordinate3* pcShapeCoordinates;
    SoIndexedFaceSet* pcShapeFaceset;
};

}  // namespace Gui


#endif  // GUI_VIEWPROVIDERTEXTUREEXT_H
