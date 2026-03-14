#ifndef MESHGUI_PROPERTYEDITOR_MESH_H
#define MESHGUI_PROPERTYEDITOR_MESH_H

#include <Gui/propertyeditor/PropertyItem.h>
#ifndef MESH_GLOBAL_H
# include <Mod/Mesh/MeshGlobal.h>
#endif


namespace MeshGui
{

using FrameOption = Gui::PropertyEditor::FrameOption;

/**
 * Display data of a mesh kernel.
 * \author Werner Mayer
 */
class MeshGuiExport PropertyMeshKernelItem: public Gui::PropertyEditor::PropertyItem
{
    Q_OBJECT
    Q_PROPERTY(int Points READ countPoints CONSTANT)
    Q_PROPERTY(int Edges READ countEdges CONSTANT)
    Q_PROPERTY(int Faces READ countFaces CONSTANT)
    PROPERTYITEM_HEADER

    // clang-format off
    QWidget* createEditor(QWidget* parent,
                          const std::function<void()>& method,
                          FrameOption frameOption) const override;
    // clang-format on
    void setEditorData(QWidget* editor, const QVariant& data) const override;
    QVariant editorData(QWidget* editor) const override;

    int countPoints() const;
    int countEdges() const;
    int countFaces() const;

protected:
    QVariant toolTip(const App::Property*) const override;
    QVariant value(const App::Property*) const override;
    void setValue(const QVariant&) override;

protected:
    PropertyMeshKernelItem();
    void initialize() override;

private:
    Gui::PropertyEditor::PropertyIntegerItem* m_p;
    Gui::PropertyEditor::PropertyIntegerItem* m_e;
    Gui::PropertyEditor::PropertyIntegerItem* m_f;
};

}  // namespace MeshGui


#endif  // MESHGUI_PROPERTYEDITOR_MESH_H
