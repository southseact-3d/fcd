// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ViewProviderColourMap.h"

#include <App/DocumentObject.h>
#include <App/PropertyLinks.h>
#include <Base/Tools.h>
#include <Gui/View3DInventorViewer.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoTextureCoordinateBinding.h>
#include <Inventor/nodes/SoNormal.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>

#include <App/PropertyTopoShape.h>

#include <Mod/Colour/App/ColourMapObject.h>
#include <Mod/Colour/App/TextureCompositor.h>
#include <Mod/Colour/App/UVAtlas.h>
#include <Mod/Colour/App/BRepTessellator.h>
#include <Mod/Part/App/PartFeature.h>

using namespace ColourGui;

namespace {

bool extractMeshFromObject(App::DocumentObject* obj, std::vector<Colour::Vertex>& vertices, std::vector<Colour::Triangle>& triangles)
{
    if (!obj) {
        return false;
    }

    const TopoDS_Shape* shapePtr = nullptr;
    
    if (obj->getTypeId().isDerivedFrom(Part::Feature::getClassTypeId())) {
        const Part::Feature* partFeature = static_cast<const Part::Feature*>(obj);
        shapePtr = &partFeature->Shape.getValue();
    }
    else {
        App::Property* shapeProp = obj->getPropertyByName("Shape");
        if (!shapeProp) {
            return false;
        }
        if (shapeProp->getTypeId() == App::PropertyTopoShape::getClassTypeId()) {
            const App::PropertyTopoShape& propShape = static_cast<const App::PropertyTopoShape&>(*shapeProp);
            shapePtr = &propShape.getValue();
        }
    }

    if (!shapePtr || shapePtr->IsNull()) {
        return false;
    }

    const TopoDS_Shape& shape = *shapePtr;

    Colour::BRepTessellator tessellator;
    tessellator.setQuality(0.5);

    BRepTessellator::TessellationOptions opts;
    opts.linearDeflection = 0.01;
    opts.angularDeflection = 0.5;
    opts.relativeDeflection = true;

    return tessellator.tessellate(shape, vertices, triangles, opts);
}

} // anonymous namespace

PROPERTY_SOURCE(ColourGui::ViewProviderColourMap, Gui::ViewProviderDocumentObject)

ViewProviderColourMap::ViewProviderColourMap()
{
    pcRoot = new SoSeparator;
    pcRoot->ref();
}

ViewProviderColourMap::~ViewProviderColourMap()
{
    pcRoot->unref();
}

void ViewProviderColourMap::attach(App::DocumentObject* obj)
{
    Gui::ViewProviderDocumentObject::attach(obj);

    pcRoot->removeAllChildren();

    pcTexture = new SoTexture2;
    pcTexture->ref();
    pcRoot->addChild(pcTexture);

    SoTextureCoordinateBinding* texBind = new SoTextureCoordinateBinding;
    texBind->value = SoTextureCoordinateBinding::PER_VERTEX_INDEXED;
    pcRoot->addChild(texBind);

    pcMaterial = new SoMaterial;
    pcMaterial->ref();
    pcMaterial->diffuseColor.setValue(1.0f, 1.0f, 1.0f);
    pcMaterial->ambientColor.setValue(0.2f, 0.2f, 0.2f);
    pcRoot->addChild(pcMaterial);

    SoShapeHints* shapeHints = new SoShapeHints;
    shapeHints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
    shapeHints->shapeType = SoShapeHints::SOLID;
    pcRoot->addChild(shapeHints);

    pcCoords = new SoCoordinate3;
    pcCoords->ref();
    pcRoot->addChild(pcCoords);

    pcNormals = new SoNormal;
    pcNormals->ref();
    pcRoot->addChild(pcNormals);

    pcUvs = new SoTextureCoordinate2;
    pcUvs->ref();
    pcRoot->addChild(pcUvs);

    pcFaces = new SoIndexedFaceSet;
    pcFaces->ref();
    pcRoot->addChild(pcFaces);

    addDisplayMaskMode(pcRoot, "Texture");

    updateData(&static_cast<Colour::ColourMapObject*>(obj)->ColourDataXml);
}

void ViewProviderColourMap::updateData(const App::Property* prop)
{
    if (prop->getTypeId().isDerivedFrom(App::PropertyString::getClassTypeId())) {
        rebuildTextureAndGeometry();
    }

    Gui::ViewProviderDocumentObject::updateData(prop);
}

void ViewProviderColourMap::rebuildTextureAndGeometry()
{
    auto* colourMapObj = static_cast<Colour::ColourMapObject*>(pcObject);
    if (!colourMapObj) {
        return;
    }

    App::DocumentObject* source = colourMapObj->Source.getValue();
    if (!source) {
        return;
    }

    std::vector<Colour::Vertex> vertices;
    std::vector<Colour::Triangle> triangles;

    if (!extractMeshFromObject(source, vertices, triangles)) {
        return;
    }

    if (vertices.empty() || triangles.empty()) {
        return;
    }

    Colour::UVAtlas atlas;
    UVAtlas::GenerateOptions options;
    options.resolution = 2048;
    options.paddingPixels = 2;

    if (!atlas.generateFacePerIsland(vertices, triangles, options)) {
        return;
    }

    const Colour::ColourMap& colourMap = colourMapObj->getColourMap();
    Colour::TextureCompositor compositor;
    Colour::AtlasImage atlasImage = compositor.compose(colourMap, atlas);

    std::vector<std::uint8_t> pngData;
    if (!compositor.encodePng(atlasImage, pngData)) {
        return;
    }

    pcTexture->image.setValue(
        SbVec2s(atlasImage.width, atlasImage.height),
        4,
        pngData.data(),
        SoTexture2::RGBA
    );

    const std::vector<Colour::TriangleUv>& triUvs = atlas.triangleUvs();

    pcCoords->point.setNum(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
        pcCoords->point.set1Value(i, vertices[i].position.x, vertices[i].position.y, vertices[i].position.z);
    }

    pcNormals->vector.setNum(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
        pcNormals->vector.set1Value(i, vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z);
    }

    int uvCoordCount = triangles.size() * 3;
    pcUvs->point.setNum(uvCoordCount);

    int uvIdx = 0;
    for (size_t triIdx = 0; triIdx < triangles.size(); ++triIdx) {
        const Colour::TriangleUv& triUv = triUvs[triIdx];
        for (int k = 0; k < 3; ++k) {
            pcUvs->point.set1Value(uvIdx, triUv.uv[k].u, triUv.uv[k].v);
            ++uvIdx;
        }
    }

    pcFaces->coordIndex.setNum(triangles.size() * 4);
    pcFaces->textureCoordIndex.setNum(triangles.size() * 4);

    int faceSetIdx = 0;
    int uvSetIdx = 0;
    for (size_t i = 0; i < triangles.size(); ++i) {
        pcFaces->coordIndex.set1Value(faceSetIdx++, triangles[i].indices[0]);
        pcFaces->coordIndex.set1Value(faceSetIdx++, triangles[i].indices[1]);
        pcFaces->coordIndex.set1Value(faceSetIdx++, triangles[i].indices[2]);
        pcFaces->coordIndex.set1Value(faceSetIdx++, -1);

        pcFaces->textureCoordIndex.set1Value(uvSetIdx++, i * 3);
        pcFaces->textureCoordIndex.set1Value(uvSetIdx++, i * 3 + 1);
        pcFaces->textureCoordIndex.set1Value(uvSetIdx++, i * 3 + 2);
        pcFaces->textureCoordIndex.set1Value(uvSetIdx++, -1);
    }
}

QIcon ViewProviderColourMap::getIcon() const
{
    return QIcon(QString::fromLatin1(":/icons/ColourWorkbench.svg"));
}

std::vector<std::string> ViewProviderColourMap::getDisplayModes() const
{
    return {"Texture"};
}

const char* ViewProviderColourMap::getDefaultDisplayMode() const
{
    return "Texture";
}

void ViewProviderColourMap::onChanged(const App::Property* prop)
{
    if (prop == &pcObject) {
        if (pcObject) {
            rebuildTextureAndGeometry();
        }
    }

    Gui::ViewProviderDocumentObject::onChanged(prop);
}
