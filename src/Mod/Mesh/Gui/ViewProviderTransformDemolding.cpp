#include <Inventor/draggers/SoTrackballDragger.h>
#include <Inventor/manips/SoTransformerManip.h>
#include <Inventor/nodes/SoAntiSquish.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSurroundScale.h>


#include <Base/Console.h>
#include <Gui/Selection/SoFCSelection.h>

#include <Mod/Mesh/App/Core/Iterator.h>
#include <Mod/Mesh/App/MeshFeature.h>

#include "ViewProviderTransformDemolding.h"


using Mesh::Feature;
using MeshCore::MeshFacetIterator;
using MeshCore::MeshGeomFacet;
using MeshCore::MeshKernel;
using namespace MeshGui;


PROPERTY_SOURCE(MeshGui::ViewProviderMeshTransformDemolding, MeshGui::ViewProviderMesh)


ViewProviderMeshTransformDemolding::ViewProviderMeshTransformDemolding()
{
    // NOLINTBEGIN
    pcTrackballDragger = new SoTrackballDragger;
    pcTrackballDragger->ref();
    pcTransformDrag = nullptr;
    pcColorMat = nullptr;
    // NOLINTEND
}

ViewProviderMeshTransformDemolding::~ViewProviderMeshTransformDemolding()
{
    pcTrackballDragger->unref();
}

void ViewProviderMeshTransformDemolding::attach(App::DocumentObject* obj)
{
    // creates the standard viewing modes
    ViewProviderMesh::attach(obj);

    auto pcDemoldRoot = new SoGroup();

    auto pcFlatStyle = new SoDrawStyle();
    pcFlatStyle->style = SoDrawStyle::FILLED;
    pcDemoldRoot->addChild(pcFlatStyle);

    // dragger
    auto surroundsep = new SoSeparator;

    auto ss = new SoSurroundScale;
    ss->numNodesUpToReset = 1;
    ss->numNodesUpToContainer = 2;
    surroundsep->addChild(ss);

    auto antisquish = new SoAntiSquish;
    antisquish->sizing = SoAntiSquish::AVERAGE_DIMENSION;
    surroundsep->addChild(antisquish);

    pcTrackballDragger->addValueChangedCallback(sValueChangedCallback, this);
    pcTrackballDragger->addFinishCallback(sDragEndCallback, this);
    surroundsep->addChild(pcTrackballDragger);

    pcTransformDrag = new SoTransform();


    auto pcMatBinding = new SoMaterialBinding;

    pcMatBinding->value = SoMaterialBinding::PER_FACE_INDEXED;
    pcColorMat = new SoMaterial;
    pcColorMat->diffuseColor.set1Value(0, 1, 1, 0);
    pcColorMat->diffuseColor.set1Value(1, 1, 0, 0);
    pcColorMat->diffuseColor.set1Value(2, 0, 1, 0);

    pcDemoldRoot->addChild(surroundsep);
    pcDemoldRoot->addChild(pcTransformDrag);
    pcDemoldRoot->addChild(pcColorMat);
    pcDemoldRoot->addChild(pcMatBinding);
    pcDemoldRoot->addChild(pcHighlight);

    // adding to the switch
    addDisplayMaskMode(pcDemoldRoot, "Demold");

    calcNormalVector();
    calcMaterialIndex(SbRotation());
    setCenterPoint();
}

void ViewProviderMeshTransformDemolding::setCenterPoint()
{
    // getting center point
    const Mesh::MeshObject& mesh = getMeshObject();
    const MeshCore::MeshKernel& kernel = mesh.getKernel();
    Base::BoundBox3f bbox = kernel.GetBoundBox();
    center = bbox.GetCenter();
}

void ViewProviderMeshTransformDemolding::calcNormalVector()
{
    const Mesh::MeshObject& mesh = getMeshObject();
    const MeshCore::MeshKernel& kernel = mesh.getKernel();

    MeshFacetIterator cFIt(kernel);
    for (cFIt.Init(); cFIt.More(); cFIt.Next()) {
        const MeshGeomFacet& rFace = *cFIt;

        Base::Vector3f norm(rFace.GetNormal());
        normalVector.emplace_back(norm.x, norm.y, norm.z);
    }
}

void ViewProviderMeshTransformDemolding::calcMaterialIndex(const SbRotation& rot)
{
    SbVec3f Up(0, 0, 1);
    SbVec3f result;

    int i = 0;
    for (auto it = normalVector.begin(); it != normalVector.end(); ++it, i++) {
        rot.multVec(*it, result);
    }
}

void ViewProviderMeshTransformDemolding::sValueChangedCallback(
    void* This,
    [[maybe_unused]] SoDragger* dragger
)
{
    static_cast<ViewProviderMeshTransformDemolding*>(This)->valueChangedCallback();
}

void ViewProviderMeshTransformDemolding::sDragEndCallback(void* This, [[maybe_unused]] SoDragger* dragger)
{
    static_cast<ViewProviderMeshTransformDemolding*>(This)->DragEndCallback();
}

void ViewProviderMeshTransformDemolding::DragEndCallback()
{
    SbRotation rot = pcTrackballDragger->rotation.getValue();
    calcMaterialIndex(rot);

    Base::Console().log("View: Finish dragging\n");
}

void ViewProviderMeshTransformDemolding::valueChangedCallback()
{
    SbMatrix temp;
    SbRotation rot = pcTrackballDragger->rotation.getValue();

    temp.setTransform(
        SbVec3f(0, 0, 0),  // no transformation
        rot,               // rotation from the dragger
        SbVec3f(1, 1, 1),  // no scaling
        SbRotation(),      // no scaling orientation
        SbVec3f(center.x, center.y, center.z)
    );  // center of rotation
    pcTransformDrag->setMatrix(temp);
}

void ViewProviderMeshTransformDemolding::setDisplayMode(const char* ModeName)
{
    if (strcmp("Demold", ModeName) == 0) {
        setDisplayMaskMode("Demold");
    }
    ViewProviderMesh::setDisplayMode(ModeName);
}

const char* ViewProviderMeshTransformDemolding::getDefaultDisplayMode() const
{
    return "Demold";
}

std::vector<std::string> ViewProviderMeshTransformDemolding::getDisplayModes() const
{
    std::vector<std::string> StrList = ViewProviderMesh::getDisplayModes();
    StrList.emplace_back("Demold");
    return StrList;
}
