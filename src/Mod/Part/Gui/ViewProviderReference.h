#ifndef PARTGUI_ViewProviderPartReference_H
#define PARTGUI_ViewProviderPartReference_H

#include <Standard_Boolean.hxx>
#include <TopoDS_Shape.hxx>
#include <Gui/ViewProviderGeometryObject.h>
#include <Gui/ViewProviderBuilder.h>
#include <Mod/Part/PartGlobal.h>
#include <map>

class TopoDS_Shape;
class TopoDS_Edge;
class TopoDS_Wire;
class TopoDS_Face;
class SoSeparator;
class SoGroup;
class SoSwitch;
class SoVertexShape;
class SoPickedPoint;
class SoShapeHints;
class SoEventCallback;
class SbVec3f;
class SoSphere;
class SoScale;

namespace PartGui
{


class PartGuiExport ViewProviderPartReference: public Gui::ViewProviderGeometryObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartGui::ViewProviderPartReference);

public:
    /// constructor
    ViewProviderPartReference();
    /// destructor
    ~ViewProviderPartReference() override;

    // Display properties
    // App::PropertyFloatConstraint LineWidth;
    // App::PropertyFloatConstraint PointSize;
    // App::PropertyColor LineColor;
    // App::PropertyColor PointColor;
    // App::PropertyMaterial LineMaterial;
    // App::PropertyMaterial PointMaterial;
    // App::PropertyBool ControlPoints;
    // App::PropertyEnumeration Lighting;


    void attach(App::DocumentObject*) override;
    void setDisplayMode(const char* ModeName) override;
    /// returns a list of all possible modes
    std::vector<std::string> getDisplayModes() const override;
    /// Update the view representation
    void reload();

    void updateData(const App::Property*) override;

protected:
    /// get called by the container whenever a property has been changed
    void onChanged(const App::Property* prop) override;
    // bool loadParameter();

    // nodes for the data representation
    SoGroup* EdgeRoot {nullptr};
    SoGroup* FaceRoot {nullptr};
    SoGroup* VertexRoot {nullptr};
    SoMaterial* pcLineMaterial {nullptr};
    SoMaterial* pcPointMaterial {nullptr};
    SoDrawStyle* pcLineStyle {nullptr};
    SoDrawStyle* pcPointStyle {nullptr};
    SoSwitch* pcControlPoints {nullptr};
    SoShapeHints* pShapeHints {nullptr};

private:
    // settings stuff
    float meshDeviation {0.01F};
    bool noPerVertexNormals {true};
    bool qualityNormals {false};
    static App::PropertyFloatConstraint::Constraints floatRange;
    static const char* LightingEnums[];
};


}  // namespace PartGui


#endif  // PARTGUI_ViewProviderPartReference_H
