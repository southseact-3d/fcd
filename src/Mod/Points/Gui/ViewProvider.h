#ifndef POINTSGUI_VIEWPROVIDERPOINTS_H
#define POINTSGUI_VIEWPROVIDERPOINTS_H

#include <Inventor/SbVec2f.h>

#include <Gui/ViewProviderBuilder.h>
#include <Gui/ViewProviderGeometryObject.h>
#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Points/PointsGlobal.h>


class SoSwitch;
class SoPointSet;
class SoIndexedPointSet;
class SoLocateHighlight;
class SoCoordinate3;
class SoNormal;
class SoEventCallback;

namespace App
{
class PropertyColorList;
}

namespace Gui
{
class SoFCSelection;
}

namespace Points
{
class PropertyGreyValueList;
class PropertyNormalList;
class PointKernel;
class Feature;
}  // namespace Points

namespace PointsGui
{

class ViewProviderPointsBuilder: public Gui::ViewProviderBuilder
{
public:
    ViewProviderPointsBuilder() = default;
    ~ViewProviderPointsBuilder() override = default;
    void buildNodes(const App::Property*, std::vector<SoNode*>&) const override;
    void createPoints(const App::Property*, SoCoordinate3*, SoPointSet*) const;
    void createPoints(const App::Property*, SoCoordinate3*, SoIndexedPointSet*) const;
};

/**
 * The ViewProviderPoints class creates
 * a node representing the point data structure.
 * @author Werner Mayer
 */
class PointsGuiExport ViewProviderPoints: public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(PointsGui::ViewProviderPoints);

public:
    ViewProviderPoints();
    ~ViewProviderPoints() override;

    App::PropertyFloatConstraint PointSize;

    /// set the viewing mode
    void setDisplayMode(const char* ModeName) override;
    /// returns a list of all possible modes
    std::vector<std::string> getDisplayModes() const override;
    QIcon getIcon() const override;

    /// Sets the edit mnode
    bool setEdit(int ModNum) override;
    /// Unsets the edit mode
    void unsetEdit(int ModNum) override;

public:
    static void clipPointsCallback(void* ud, SoEventCallback* n);

protected:
    void onChanged(const App::Property* prop) override;
    void setVertexColorMode(App::PropertyColorList*);
    void setVertexGreyvalueMode(Points::PropertyGreyValueList*);
    void setVertexNormalMode(Points::PropertyNormalList*);
    virtual void cut(const std::vector<SbVec2f>& picked, Gui::View3DInventorViewer& Viewer) = 0;

protected:
    Gui::SoFCSelection* pcHighlight;
    SoCoordinate3* pcPointsCoord;
    SoMaterial* pcColorMat;
    SoNormal* pcPointsNormal;
    SoDrawStyle* pcPointStyle;

private:
    static App::PropertyFloatConstraint::Constraints floatRange;
};

/**
 * The ViewProviderScattered class creates
 * a node representing the scattered point cloud.
 * @author Werner Mayer
 */
class PointsGuiExport ViewProviderScattered: public ViewProviderPoints
{
    PROPERTY_HEADER_WITH_OVERRIDE(PointsGui::ViewProviderScattered);

public:
    ViewProviderScattered();
    ~ViewProviderScattered() override;

    /**
     * Extracts the point data from the feature \a pcFeature and creates
     * an Inventor node \a SoNode with these data.
     */
    void attach(App::DocumentObject*) override;
    /// Update the point representation
    void updateData(const App::Property*) override;

protected:
    void cut(const std::vector<SbVec2f>& picked, Gui::View3DInventorViewer& Viewer) override;

protected:
    SoPointSet* pcPoints;
};

/**
 * The ViewProviderStructured class creates
 * a node representing the structured points.
 * @author Werner Mayer
 */
class PointsGuiExport ViewProviderStructured: public ViewProviderPoints
{
    PROPERTY_HEADER_WITH_OVERRIDE(PointsGui::ViewProviderStructured);

public:
    ViewProviderStructured();
    ~ViewProviderStructured() override;

    /**
     * Extracts the point data from the feature \a pcFeature and creates
     * an Inventor node \a SoNode with these data.
     */
    void attach(App::DocumentObject*) override;
    /// Update the point representation
    void updateData(const App::Property*) override;

protected:
    void cut(const std::vector<SbVec2f>& picked, Gui::View3DInventorViewer& Viewer) override;

protected:
    SoIndexedPointSet* pcPoints;
};

using ViewProviderPython = Gui::ViewProviderFeaturePythonT<ViewProviderScattered>;

}  // namespace PointsGui


#endif  // POINTSGUI_VIEWPROVIDERPOINTS_H
