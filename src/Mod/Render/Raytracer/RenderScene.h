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

#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include <vector>
#include "BVH.h"
#include "ShadingSystem.h"
#include "RenderCameraRT.h"
#include "Light.h"
#include "TextureRT.h"
#include "RenderRaytracerGlobal.h"

namespace Render
{

class RenderRaytracerExport RenderScene
{
public:
    void extractFromDocument();

    const std::vector<Triangle>& triangles() const { return m_triangles; }
    const std::vector<ShadingParams>& materials() const { return m_materials; }
    const std::vector<Light>& lights() const { return m_lights; }
    const RenderCameraRT& camera() const { return m_camera; }
    const TextureRT& environmentMap() const { return m_environment; }
    const BVH& bvh() const { return m_bvh; }

private:
    std::vector<Triangle> m_triangles;
    std::vector<ShadingParams> m_materials;
    std::vector<Light> m_lights;
    RenderCameraRT m_camera;
    TextureRT m_environment;
    BVH m_bvh;
};

}  // namespace Render

#endif  // RENDER_SCENE_H
