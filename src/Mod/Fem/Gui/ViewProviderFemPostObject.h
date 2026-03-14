#ifndef FEM_VIEWPROVIDERFEMPOSTOBJECT_H
#define FEM_VIEWPROVIDERFEMPOSTOBJECT_H

#include <Base/Observer.h>
#include <Gui/ViewProviderGeometryObject.h>
#include <Mod/Fem/FemGlobal.h>

#include <vtkAppendPolyData.h>
#include <vtkDataObject.h>
#include <vtkExtractEdges.h>
#include <vtkGeometryFilter.h>
#include <vtkOutlineCornerFilter.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>

class SoIndexedPointSet;
class vtkUnsignedCharArray;
class vtkDataArray;
class vtkPoints;
class SoSeparator;
class SoNormal;
class SoNormalBinding;
class SoMaterial;
class SoShapeHints;
class SoMaterialBinding;
class SoIndexedFaceSet;
class SoIndexedLineSet;
class SoIndexedMarkerSet;
class SoCoordinate3;
class SoDrawStyle;
class SoIndexedFaceSet;
class SoIndexedLineSet;
class SoIndexedTriangleStripSet;
class SoTransparencyType;
class SoDepthBuffer;
class SoSwitch;

namespace Gui
{
class SelectionChanges;
class SoFCColorBar;
}  // namespace Gui

namespace FemGui
{

class TaskDlgPost;

class FemGuiExport ViewProviderFemPostObject: public Gui::ViewProviderDocumentObject,
                                              public Base::Observer<int>
{
    PROPERTY_HEADER_WITH_OVERRIDE(FemGui::ViewProviderFemPostObject);

public:
    /// constructor.
    ViewProviderFemPostObject();

    /// destructor.
    ~ViewProviderFemPostObject() override;

    App::PropertyEnumeration Field;
    App::PropertyEnumeration Component;
    App::PropertyPercent Transparency;
    App::PropertyBool PlainColorEdgeOnSurface;
    App::PropertyColor EdgeColor;
    App::PropertyFloatConstraint LineWidth;
    App::PropertyFloatConstraint PointSize;

    void attach(App::DocumentObject* pcObject) override;
    void setDisplayMode(const char* ModeName) override;
    std::vector<std::string> getDisplayModes() const override;
    void updateData(const App::Property*) override;
    void onChanged(const App::Property* prop) override;

    // edit handling
    bool doubleClicked() override;
    bool setEdit(int ModNum) override;
    void unsetEdit(int ModNum) override;

    void hide() override;
    void show() override;

    SoSeparator* getFrontRoot() const override;

    // observer for the color bar
    void OnChange(Base::Subject<int>& rCaller, int rcReason) override;
    // update color bar
    void updateMaterial();

    // handling when object is deleted
    bool onDelete(const std::vector<std::string>&) override;
    bool canDelete(App::DocumentObject* obj) const override;
    virtual void onSelectionChanged(const Gui::SelectionChanges& sel);

    // setting up task dialogs
    virtual void setupTaskDialog(TaskDlgPost* dlg);

protected:
    void handleChangedPropertyName(
        Base::XMLReader& reader,
        const char* typeName,
        const char* propName
    ) override;

    bool setupPipeline();
    void updateVtk();
    void setRangeOfColorBar(float min, float max);

    SoCoordinate3* m_coordinates;
    SoIndexedPointSet* m_markers;
    SoIndexedLineSet* m_lines;
    SoIndexedFaceSet* m_faces;
    SoIndexedTriangleStripSet* m_triangleStrips;
    SoSwitch* m_switchMatEdges;
    SoMaterial* m_material;
    SoMaterial* m_matPlainEdges;
    SoMaterialBinding* m_materialBinding;
    SoShapeHints* m_shapeHints;
    SoNormalBinding* m_normalBinding;
    SoNormal* m_normals;
    SoDrawStyle* m_drawStyle;
    SoSeparator* m_separator;
    Gui::SoFCColorBar* m_colorBar;
    SoSeparator* m_colorRoot;
    SoDrawStyle* m_colorStyle;
    SoTransparencyType* m_transpType;
    SoSeparator* m_sepMarkerLine;
    SoDepthBuffer* m_depthBuffer;

    vtkSmartPointer<vtkPolyDataAlgorithm> m_currentAlgorithm;
    vtkSmartPointer<vtkGeometryFilter> m_surface;
    vtkSmartPointer<vtkAppendPolyData> m_surfaceEdges;
    vtkSmartPointer<vtkOutlineCornerFilter> m_outline;
    vtkSmartPointer<vtkExtractEdges> m_wireframe, m_wireframeSurface;
    vtkSmartPointer<vtkVertexGlyphFilter> m_points, m_pointsSurface;

private:
    void updateProperties();
    void update3D();
    void WritePointData(vtkPoints* points, vtkDataArray* normals, vtkDataArray* tcoords);
    void WriteColorData(bool ResetColorBarRange);
    void WriteTransparency();
    void deleteColorBar();

    App::Enumeration m_coloringEnum, m_vectorEnum;
    bool m_blockPropertyChanges {false};

    static App::PropertyFloatConstraint::Constraints sizeRange;
};

}  // namespace FemGui


#endif  // FEM_VIEWPROVIDERFEMPOSTOBJECT_H
