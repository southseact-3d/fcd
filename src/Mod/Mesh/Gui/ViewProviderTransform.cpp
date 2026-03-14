#include <Inventor/manips/SoTransformerManip.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoNormalBinding.h>
#include <Inventor/nodes/SoSeparator.h>


#include <Gui/Selection/SoFCSelection.h>
#include <Mod/Mesh/App/MeshFeature.h>

#include "ViewProviderTransform.h"


using namespace MeshGui;
using Mesh::Feature;

PROPERTY_SOURCE(MeshGui::ViewProviderMeshTransform, MeshGui::ViewProviderMesh)

ViewProviderMeshTransform::ViewProviderMeshTransform()
{
    // NOLINTBEGIN
    pcTransformerDragger = new SoTransformerManip();
    pcTransformerDragger->ref();
    // NOLINTEND
}

ViewProviderMeshTransform::~ViewProviderMeshTransform()
{
    pcTransformerDragger->unref();
}

void ViewProviderMeshTransform::attach(App::DocumentObject* obj)
{
    // creates the standard viewing modes
    ViewProviderMesh::attach(obj);

    auto pcEditRoot = new SoSeparator();

    // flat shaded (Normal) ------------------------------------------
    auto pcFlatStyle = new SoDrawStyle();
    pcFlatStyle->style = SoDrawStyle::FILLED;
    auto pcBinding = new SoNormalBinding();
    pcBinding->value = SoNormalBinding::PER_FACE;

    pcEditRoot->addChild(pcTransformerDragger);
    pcEditRoot->addChild(pcFlatStyle);
    pcEditRoot->addChild(pcShapeMaterial);
    pcEditRoot->addChild(pcBinding);
    pcEditRoot->addChild(pcHighlight);

    // adding to the switch
    addDisplayMaskMode(pcEditRoot, "Edit");
}

void ViewProviderMeshTransform::updateData(const App::Property* prop)
{
    ViewProviderMesh::updateData(prop);
}

void ViewProviderMeshTransform::setDisplayMode(const char* ModeName)
{
    if (strcmp("Transform", ModeName) == 0) {
        setDisplayMaskMode("Edit");
    }
    ViewProviderMesh::setDisplayMode(ModeName);
}

const char* ViewProviderMeshTransform::getDefaultDisplayMode() const
{
    return "Transform";
}

std::vector<std::string> ViewProviderMeshTransform::getDisplayModes() const
{
    std::vector<std::string> StrList = ViewProviderMesh::getDisplayModes();
    StrList.emplace_back("Transform");
    return StrList;
}
