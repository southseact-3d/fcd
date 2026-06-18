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

#ifndef RENDER_ENGINE_H
#define RENDER_ENGINE_H

#include <atomic>
#include <functional>
#include <QImage>
#include "RenderRaytracerGlobal.h"

namespace Render
{

class RenderScene;
class RenderSettings;

class RenderRaytracerExport RenderEngine
{
public:
    virtual ~RenderEngine() = default;

    virtual void render(RenderScene& scene, QImage& output,
                        const RenderSettings& settings,
                        std::function<void(float)> progressCallback) = 0;

    virtual void cancel() { cancelled = true; }

protected:
    std::atomic<bool> cancelled{false};
};

}  // namespace Render

#endif  // RENDER_ENGINE_H
