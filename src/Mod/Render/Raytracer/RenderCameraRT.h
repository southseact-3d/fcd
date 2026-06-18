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

#ifndef RENDER_CAMERART_H
#define RENDER_CAMERART_H

#include <Base/Vector3D.h>
#include "RenderRaytracerGlobal.h"

namespace Render
{

struct CameraRay
{
    Base::Vector3d origin;
    Base::Vector3d direction;
    double weight = 1.0;
};

class RenderRaytracerExport RenderCameraRT
{
public:
    RenderCameraRT() = default;

    void generateRay(double u, double v, double lensU, double lensV, CameraRay& ray) const;

    Base::Vector3d position{0.0, -10.0, 0.0};
    Base::Vector3d target{0.0, 0.0, 0.0};
    Base::Vector3d up{0.0, 0.0, 1.0};
    double fov = 45.0;
    double aspectRatio = 16.0 / 9.0;
    bool orthographic = false;
    double aperture = 0.0;
    double focalDistance = 10.0;
    double exposure = 0.0;
    int width = 1920;
    int height = 1080;
};

}  // namespace Render

#endif  // RENDER_CAMERART_H
