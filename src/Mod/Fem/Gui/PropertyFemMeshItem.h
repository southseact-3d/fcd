#ifndef FEMGUI_PROPERTY_FEMMESH_ITEM_H
#define FEMGUI_PROPERTY_FEMMESH_ITEM_H

#include <Gui/propertyeditor/PropertyItem.h>

namespace FemGui
{

using FrameOption = Gui::PropertyEditor::FrameOption;

/**
 * Display data of an FEM mesh.
 * \author Werner Mayer
 */
class PropertyFemMeshItem: public Gui::PropertyEditor::PropertyItem
{
    Q_OBJECT
    Q_PROPERTY(int Nodes READ countNodes CONSTANT)
    Q_PROPERTY(int Edges READ countEdges CONSTANT)
    Q_PROPERTY(int Faces READ countFaces CONSTANT)
    Q_PROPERTY(int Polygons READ countPolygons CONSTANT)
    Q_PROPERTY(int Volumes READ countVolumes CONSTANT)
    Q_PROPERTY(int Polyhedrons READ countPolyhedrons CONSTANT)
    Q_PROPERTY(int Groups READ countGroups CONSTANT)
    PROPERTYITEM_HEADER

    QWidget* createEditor(
        QWidget* parent,
        const std::function<void()>& method,
        FrameOption frameOption
    ) const override;
    void setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;

    int countNodes() const;
    int countEdges() const;
    int countFaces() const;
    int countPolygons() const;
    int countVolumes() const;
    int countPolyhedrons() const;
    int countGroups() const;

protected:
    QVariant toolTip(const App::Property*) const override;
    QVariant value(const App::Property*) const override;
    void setValue(const QVariant&) override;

protected:
    PropertyFemMeshItem();
    void initialize() override;

private:
    Gui::PropertyEditor::PropertyIntegerItem* m_n;
    Gui::PropertyEditor::PropertyIntegerItem* m_e;
    Gui::PropertyEditor::PropertyIntegerItem* m_f;
    Gui::PropertyEditor::PropertyIntegerItem* m_p;
    Gui::PropertyEditor::PropertyIntegerItem* m_v;
    Gui::PropertyEditor::PropertyIntegerItem* m_h;
    Gui::PropertyEditor::PropertyIntegerItem* m_g;
};

}  // namespace FemGui


#endif  // FEMGUI_PROPERTY_FEMMESH_ITEM_H
