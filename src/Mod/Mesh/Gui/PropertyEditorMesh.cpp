#include <Mod/Mesh/App/MeshFeature.h>

#include "PropertyEditorMesh.h"


using namespace MeshGui;
using MeshCore::MeshKernel;


PROPERTYITEM_SOURCE(MeshGui::PropertyMeshKernelItem)

PropertyMeshKernelItem::PropertyMeshKernelItem()
{
    // NOLINTBEGIN
    m_p = static_cast<Gui::PropertyEditor::PropertyIntegerItem*>(
        Gui::PropertyEditor::PropertyIntegerItem::create()
    );
    m_p->setParent(this);
    m_p->setPropertyName(QLatin1String("Points"));
    this->appendChild(m_p);
    m_e = static_cast<Gui::PropertyEditor::PropertyIntegerItem*>(
        Gui::PropertyEditor::PropertyIntegerItem::create()
    );
    m_e->setParent(this);
    m_e->setPropertyName(QLatin1String("Edges"));
    this->appendChild(m_e);
    m_f = static_cast<Gui::PropertyEditor::PropertyIntegerItem*>(
        Gui::PropertyEditor::PropertyIntegerItem::create()
    );
    m_f->setParent(this);
    m_f->setPropertyName(QLatin1String("Faces"));
    this->appendChild(m_f);
    // NOLINTEND
}

void PropertyMeshKernelItem::initialize()
{
    this->setReadOnly(true);
}

QVariant PropertyMeshKernelItem::value(const App::Property*) const
{
    int ctP = 0;
    int ctE = 0;
    int ctF = 0;

    const std::vector<App::Property*>& props = getPropertyData();
    for (auto prop : props) {
        Mesh::PropertyMeshKernel* pPropMesh = static_cast<Mesh::PropertyMeshKernel*>(prop);
        const MeshKernel& rMesh = pPropMesh->getValue().getKernel();
        ctP += (int)rMesh.CountPoints();
        ctE += (int)rMesh.CountEdges();
        ctF += (int)rMesh.CountFacets();
    }

    QString str = QObject::tr("[Points: %1, Edges: %2, Faces: %3]").arg(ctP).arg(ctE).arg(ctF);
    return {str};
}

QVariant PropertyMeshKernelItem::toolTip(const App::Property* prop) const
{
    return value(prop);
}

void PropertyMeshKernelItem::setValue(const QVariant& value)
{
    Q_UNUSED(value);
}

QWidget* PropertyMeshKernelItem::createEditor(
    QWidget* parent,
    const std::function<void()>& method,
    FrameOption frameOption
) const
{
    Q_UNUSED(parent);
    Q_UNUSED(method);
    Q_UNUSED(frameOption);
    return nullptr;
}

void PropertyMeshKernelItem::setEditorData(QWidget* editor, const QVariant& data) const
{
    Q_UNUSED(editor);
    Q_UNUSED(data);
}

QVariant PropertyMeshKernelItem::editorData(QWidget* editor) const
{
    Q_UNUSED(editor);
    return {};
}

int PropertyMeshKernelItem::countPoints() const
{
    int ctP = 0;
    const std::vector<App::Property*>& props = getPropertyData();
    for (auto prop : props) {
        Mesh::PropertyMeshKernel* pPropMesh = static_cast<Mesh::PropertyMeshKernel*>(prop);
        const MeshKernel& rMesh = pPropMesh->getValue().getKernel();
        ctP += (int)rMesh.CountPoints();
    }

    return ctP;
}

int PropertyMeshKernelItem::countEdges() const
{
    int ctE = 0;
    const std::vector<App::Property*>& props = getPropertyData();
    for (auto prop : props) {
        Mesh::PropertyMeshKernel* pPropMesh = static_cast<Mesh::PropertyMeshKernel*>(prop);
        const MeshKernel& rMesh = pPropMesh->getValue().getKernel();
        ctE += (int)rMesh.CountEdges();
    }

    return ctE;
}

int PropertyMeshKernelItem::countFaces() const
{
    int ctF = 0;
    const std::vector<App::Property*>& props = getPropertyData();
    for (auto prop : props) {
        Mesh::PropertyMeshKernel* pPropMesh = static_cast<Mesh::PropertyMeshKernel*>(prop);
        const MeshKernel& rMesh = pPropMesh->getValue().getKernel();
        ctF += (int)rMesh.CountFacets();
    }

    return ctF;
}

#include "moc_PropertyEditorMesh.cpp"
