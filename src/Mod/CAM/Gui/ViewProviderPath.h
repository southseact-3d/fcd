#ifndef PATH_ViewProviderPath_H
#define PATH_ViewProviderPath_H

#include <App/PropertyGeo.h>
#include <Gui/Selection/Selection.h>
#include <Gui/ViewProviderGeometryObject.h>
#include <Gui/ViewProviderFeaturePython.h>
#include <Mod/Part/Gui/SoBrepEdgeSet.h>
#include <Mod/CAM/PathGlobal.h>


class SoCoordinate3;
class SoDrawStyle;
class SoMaterial;
class SoBaseColor;
class SoMaterialBinding;
class SoTransform;
class SoSwitch;

namespace Path
{
class Toolpath;
}

namespace PathGui
{

class PathSelectionObserver;

class PathGuiExport ViewProviderPath: public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(PathGui::ViewProviderPath);
    using inherited = ViewProviderGeometryObject;

public:
    /// constructor.
    ViewProviderPath();

    /// destructor.
    ~ViewProviderPath() override;

    // Display properties
    App::PropertyInteger LineWidth;
    App::PropertyColor NormalColor;
    App::PropertyColor MarkerColor;
    App::PropertyBool ShowNodes;
    App::PropertyVector StartPosition;

    App::PropertyIntegerConstraint StartIndex;
    App::PropertyIntegerConstraint::Constraints StartIndexConstraints;
    App::PropertyIntegerConstraint ShowCount;
    App::PropertyIntegerConstraint::Constraints ShowCountConstraints;

    void attach(App::DocumentObject* pcObject) override;
    void setDisplayMode(const char* ModeName) override;
    std::vector<std::string> getDisplayModes() const override;
    void updateData(const App::Property*) override;
    void recomputeBoundingBox();
    QIcon getIcon() const override;

    bool useNewSelectionModel() const override;
    std::string getElement(const SoDetail*) const override;
    SoDetail* getDetail(const char* subelement) const override;

    void updateShowConstraints();
    void updateVisual(bool rebuild = false);
    void hideSelection();

    void showBoundingBox(bool show) override;

    friend class PathSelectionObserver;

private:
    /// Find the index of the first non-rapid move command
    long findFirstFeedMoveIndex(const Path::Toolpath& path) const;

protected:
    void onChanged(const App::Property* prop) override;
    unsigned long getBoundColor() const override;

    SoCoordinate3* pcLineCoords;
    SoCoordinate3* pcMarkerCoords;
    SoDrawStyle* pcDrawStyle;
    SoDrawStyle* pcMarkerStyle;
    PartGui::SoBrepEdgeSet* pcLines;
    SoMaterial* pcLineColor;
    SoBaseColor* pcMarkerColor;
    SoMaterialBinding* pcMatBind;
    std::vector<int> colorindex;
    SoSwitch* pcMarkerSwitch;
    SoSwitch* pcArrowSwitch;
    SoTransform* pcArrowTransform;

    std::vector<int> command2Edge;
    std::deque<int> edge2Command;
    std::deque<int> edgeIndices;

    mutable int pt0Index;
    bool blockPropertyChange;
    int edgeStart;
    int coordStart;
    int coordEnd;
};

using ViewProviderPathPython = Gui::ViewProviderFeaturePythonT<ViewProviderPath>;

}  // namespace PathGui


#endif  // PATH_VIEWPROVIDERPATH_H
