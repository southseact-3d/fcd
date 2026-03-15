// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOURGUI_VIEWPROVIDERCOLOURMAP_H
#define COLOURGUI_VIEWPROVIDERCOLOURMAP_H

#include <Gui/ViewProviderDocumentObject.h>
#include <Mod/Colour/ColourGlobal.h>

class SoSeparator;
class SoTexture2;
class SoMaterial;
class SoCoordinate3;
class SoIndexedFaceSet;
class SoNormal;
class SoTextureCoordinate2;

namespace ColourGui
{

class ColourGuiExport ViewProviderColourMap: public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(ColourGui::ViewProviderColourMap);

public:
    ViewProviderColourMap();
    ~ViewProviderColourMap() override;

    void attach(App::DocumentObject* obj) override;
    void updateData(const App::Property* prop) override;

    QIcon getIcon() const override;

    std::vector<std::string> getDisplayModes() const override;
    const char* getDefaultDisplayMode() const override;

protected:
    void onChanged(const App::Property* prop) override;

private:
    void rebuildTextureAndGeometry();

    SoSeparator* pcRoot {nullptr};
    SoTexture2* pcTexture {nullptr};
    SoMaterial* pcMaterial {nullptr};
    SoCoordinate3* pcCoords {nullptr};
    SoNormal* pcNormals {nullptr};
    SoTextureCoordinate2* pcUvs {nullptr};
    SoIndexedFaceSet* pcFaces {nullptr};
};

} // namespace ColourGui

#endif // COLOURGUI_VIEWPROVIDERCOLOURMAP_H
