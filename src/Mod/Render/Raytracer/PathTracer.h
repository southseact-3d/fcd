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

#ifndef RENDER_PATHTRACER_H
#define RENDER_PATHTRACER_H

#include <vector>
#include <mutex>
#include <atomic>
#include "RenderEngine.h"
#include "BVH.h"
#include "ShadingSystem.h"
#include "RenderCameraRT.h"
#include "Light.h"
#include "TextureRT.h"
#include "RenderRaytracerGlobal.h"

namespace Render
{

class RenderScene;
class RenderSettings;

class RenderRaytracerExport PathTracer : public RenderEngine
{
public:
    void render(RenderScene& scene, QImage& output,
                const RenderSettings& settings,
                std::function<void(float)> progressCallback) override;

private:
    struct Tile
    {
        int x, y, w, h;
    };

    Base::Vector3d traceRay(const Ray& ray, const RenderScene& scene, int maxBounces,
                            int depth, Base::Vector3d& normalOut);

    Base::Vector3d estimateDirectLight(const HitInfo& hit, const ShadingParams& params,
                                       const Base::Vector3d& wo, const RenderScene& scene);

    Base::Vector3d sampleEnvironment(const Base::Vector3d& dir, const RenderScene& scene);

    bool isOccluded(const Base::Vector3d& origin, const Base::Vector3d& target,
                    const RenderScene& scene);
};

}  // namespace Render

#endif  // RENDER_PATHTRACER_H
