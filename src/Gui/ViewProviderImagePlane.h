#ifndef GUI_ViewProviderImagePlane_H
#define GUI_ViewProviderImagePlane_H

#include <Gui/ViewProviderGeometryObject.h>


class SoCoordinate3;
class SoDrawStyle;
class SoShapeHints;
class SoTexture2;
class QImage;

namespace Gui
{

class GuiExport ViewProviderImagePlane: public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Gui::ViewProviderImagePlane);

public:
    ViewProviderImagePlane();
    ~ViewProviderImagePlane() override;

    App::PropertyEnumeration Lighting;

    void attach(App::DocumentObject* pcObject) override;
    void setDisplayMode(const char* ModeName) override;
    std::vector<std::string> getDisplayModes() const override;
    void updateData(const App::Property*) override;
    void setupContextMenu(QMenu*, QObject*, const char*) override;
    bool doubleClicked() override;
    void onChanged(const App::Property* prop) override;

private:
    void resizePlane(float xsize, float ysize);
    void loadImage();
    void setPlaneSize(const QSizeF& size, const QImage& img);
    void reloadIfSvg();
    bool isSvgFile(const char*) const;
    QSizeF getSizeInMM(const QImage&) const;
    QSizeF defaultSizeOfSvg(const char*) const;
    QSizeF pixelSize(const char*, const QSizeF&) const;
    QImage loadSvg(const char*) const;
    QImage loadSvgOfSize(const char*, const QSizeF&) const;
    QImage loadRaster(const char*) const;
    void convertToSFImage(const QImage& img);
    void manipulateImage();

private:
    SoCoordinate3* pcCoords;
    SoTexture2* texture;
    SoShapeHints* shapeHints;
    static const char* LightingEnums[];
};

}  // namespace Gui


#endif  // GUI_ViewProviderImagePlane_H
