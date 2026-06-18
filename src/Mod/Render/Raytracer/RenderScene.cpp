// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten CAD Contributors                          *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 *                                                                         *
 **************************************************************************/

#include "RenderScene.h"
#include <App/Application.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/ViewProvider.h>
#include <Gui/ViewProviderGeometryObject.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Mesh/App/MeshFeature.h>
#include <Mod/Mesh/App/Mesh.h>

#include <vector>
#include <cmath>

namespace Render
{

void RenderScene::extractFromDocument()
{
    m_triangles.clear();
    m_materials.clear();
    m_lights.clear();

    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) return;

    Gui::Document* guiDoc = Gui::Application::Instance->activeDocument();
    if (!guiDoc) return;

    uint32_t materialIndex = 0;

    // Extract Part::Feature objects
    std::vector<App::DocumentObject*> partObjects = doc->getObjectsOfType<Part::Feature>();
    for (App::DocumentObject* obj : partObjects) {
        Gui::ViewProvider* vp = guiDoc->getViewProvider(obj);
        if (!vp || !vp->isShow()) continue;

        Part::Feature* feature = static_cast<Part::Feature*>(obj);
        Part::TopoShape topoShape = Part::Feature::getTopoShape(
            obj,
            Part::ShapeOption::ResolveLink | Part::ShapeOption::Transform
        );

        if (topoShape.isNull()) continue;

        // Get material from ViewProvider
        ShadingParams shadingParams;
        Gui::ViewProviderGeometryObject* vpg =
            dynamic_cast<Gui::ViewProviderGeometryObject*>(vp);
        if (vpg) {
            App::Material mat;
            if (!vpg->ShapeAppearance.getValues().empty()) {
                mat = vpg->ShapeAppearance.getValues()[0];
            }
            shadingParams = ShadingSystem::fromAppMaterial(mat);
        }

        // Find or add material
        uint32_t matIdx = materialIndex;
        bool found = false;
        for (uint32_t i = 0; i < static_cast<uint32_t>(m_materials.size()); ++i) {
            if (m_materials[i].baseColor == shadingParams.baseColor &&
                m_materials[i].metallic == shadingParams.metallic &&
                m_materials[i].roughness == shadingParams.roughness) {
                matIdx = i;
                found = true;
                break;
            }
        }
        if (!found) {
            matIdx = static_cast<uint32_t>(m_materials.size());
            m_materials.push_back(shadingParams);
        }

        // Tessellate shape
        std::vector<Base::Vector3d> points;
        std::vector<Data::ComplexGeoData::Facet> faces;
        topoShape.getFaces(points, faces, topoShape.getAccuracy());

        // Convert faces to triangles
        for (const auto& face : faces) {
            if (face.I1 >= points.size() || face.I2 >= points.size() ||
                face.I3 >= points.size()) {
                continue;
            }

            Triangle tri;
            tri.v0 = points[face.I1];
            tri.v1 = points[face.I2];
            tri.v2 = points[face.I3];
            tri.materialIndex = matIdx;

            // Compute face normal
            Base::Vector3d edge1 = tri.v1 - tri.v0;
            Base::Vector3d edge2 = tri.v2 - tri.v0;
            tri.normal = edge1.Cross(edge2);
            double len = tri.normal.Length();
            if (len > 1e-10) {
                tri.normal = tri.normal / len;
            }
            else {
                tri.normal = Base::Vector3d(0.0, 0.0, 1.0);
            }

            m_triangles.push_back(tri);
        }
    }

    // Extract Mesh::Feature objects
    std::vector<App::DocumentObject*> meshObjects = doc->getObjectsOfType<Mesh::Feature>();
    for (App::DocumentObject* obj : meshObjects) {
        Gui::ViewProvider* vp = guiDoc->getViewProvider(obj);
        if (!vp || !vp->isShow()) continue;

        Mesh::Feature* meshFeature = static_cast<Mesh::Feature*>(obj);
        const Mesh::MeshObject& meshObj = meshFeature->Mesh.getValue();

        // Get material
        ShadingParams shadingParams;
        Gui::ViewProviderGeometryObject* vpg =
            dynamic_cast<Gui::ViewProviderGeometryObject*>(vp);
        if (vpg) {
            App::Material mat;
            if (!vpg->ShapeAppearance.getValues().empty()) {
                mat = vpg->ShapeAppearance.getValues()[0];
            }
            shadingParams = ShadingSystem::fromAppMaterial(mat);
        }

        uint32_t matIdx = static_cast<uint32_t>(m_materials.size());
        m_materials.push_back(shadingParams);

        // Use the ComplexGeoData interface to get faces
        std::vector<Base::Vector3d> pts;
        std::vector<Data::ComplexGeoData::Facet> faces;
        meshObj.getFaces(pts, faces, meshObj.getAccuracy());

        for (const auto& face : faces) {
            if (face.I1 >= pts.size() || face.I2 >= pts.size() ||
                face.I3 >= pts.size()) {
                continue;
            }

            Triangle tri;
            tri.v0 = pts[face.I1];
            tri.v1 = pts[face.I2];
            tri.v2 = pts[face.I3];
            tri.materialIndex = matIdx;

            Base::Vector3d edge1 = tri.v1 - tri.v0;
            Base::Vector3d edge2 = tri.v2 - tri.v0;
            tri.normal = edge1.Cross(edge2);
            double len = tri.normal.Length();
            if (len > 1e-10) {
                tri.normal = tri.normal / len;
            }
            else {
                tri.normal = Base::Vector3d(0.0, 0.0, 1.0);
            }

            m_triangles.push_back(tri);
        }
    }

    // Build BVH
    m_bvh.build(m_triangles);
}

}  // namespace Render
