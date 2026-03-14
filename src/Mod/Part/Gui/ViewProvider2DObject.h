#ifndef PARTGUI_VIEWPROVIDER2DOBJECT_H
#define PARTGUI_VIEWPROVIDER2DOBJECT_H

#include "ViewProvider.h"
#include <App/PropertyUnits.h>
#include <Gui/ViewProviderFeaturePython.h>

#include <Mod/Part/PartGlobal.h>

class TopoDS_Shape;
class TopoDS_Face;
class SoSeparator;
class SbVec3f;
class SoTransform;

namespace PartGui
{


class PartGuiExport ViewProvider2DObject: public PartGui::ViewProviderPart
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProvider2DObject);

    static constexpr float horizontalPlanePadding = 8;
    static constexpr float verticalPlanePadding = 6;

public:
    /// constructor
    ViewProvider2DObject();
    /// destructor
    ~ViewProvider2DObject() override;

    App::PropertyBool ShowPlane;

    void attach(App::DocumentObject*) override;
    void updateData(const App::Property*) override;
    void onChanged(const App::Property*) override;

    std::vector<std::string> getDisplayModes() const override;
    const char* getDefaultDisplayMode() const override;

protected:
    void updatePlane();

    Gui::CoinPtr<SoSwitch> plane;
};

class PartGuiExport ViewProvider2DObjectGrid: public ViewProvider2DObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProvider2DObjectGrid);

public:
    /// constructor
    ViewProvider2DObjectGrid();
    /// destructor
    ~ViewProvider2DObjectGrid() override;

    /// Property to switch the grid on and off
    App::PropertyBool ShowGrid;
    App::PropertyBool ShowOnlyInEditMode;
    App::PropertyLength GridSize;
    App::PropertyEnumeration GridStyle;
    App::PropertyBool TightGrid;
    App::PropertyBool GridSnap;
    App::PropertyBool GridAutoSize;
    App::PropertyInteger maxNumberOfLines;

    void attach(App::DocumentObject*) override;
    void updateData(const App::Property*) override;

    /// creates the grid
    SoSeparator* createGrid();

protected:
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;
    /// get called by the container whenever a property has been changed
    void onChanged(const App::Property* prop) override;
    void Restore(Base::XMLReader& reader) override;
    void handleChangedPropertyType(
        Base::XMLReader& reader,
        const char* TypeName,
        App::Property* prop
    ) override;

    SoSeparator* GridRoot;

    void updateGridExtent(float minx, float maxx, float miny, float maxy);

    static const char* GridStyleEnums[];
    static App::PropertyQuantityConstraint::Constraints GridSizeRange;

private:
    float MinX;
    float MaxX;
    float MinY;
    float MaxY;
};

using ViewProvider2DObjectPython = Gui::ViewProviderFeaturePythonT<ViewProvider2DObject>;

}  // namespace PartGui


#endif  // PARTGUI_VIEWPROVIDER2DOBJECT_H
