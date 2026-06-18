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

#ifndef RENDER_LIGHT_H
#define RENDER_LIGHT_H

#include <Base/Vector3D.h>
#include "RenderRaytracerGlobal.h"

namespace Render
{

enum class LightType
{
    Point,
    Directional,
    Area,
    Environment
};

struct Light
{
    LightType type = LightType::Point;
    Base::Vector3d position{0.0, 0.0, 5.0};
    Base::Vector3d direction{0.0, 0.0, -1.0};
    Base::Vector3d color{1.0, 1.0, 1.0};
    double intensity = 1.0;
    double size = 1.0;
};

struct LightSample
{
    Base::Vector3d point{0.0, 0.0, 0.0};
    Base::Vector3d normal{0.0, 0.0, 1.0};
    Base::Vector3d radiance{0.0, 0.0, 0.0};
    double pdf = 0.0;
};

RenderRaytracerExport LightSample sampleLight(const Light& light,
                                              const Base::Vector3d& surfacePoint,
                                              double u, double v);

RenderRaytracerExport double lightPDF(const Light& light,
                                      const Base::Vector3d& surfacePoint,
                                      const LightSample& sample);

}  // namespace Render

#endif  // RENDER_LIGHT_H
