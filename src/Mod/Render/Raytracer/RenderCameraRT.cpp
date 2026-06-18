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

#include "RenderCameraRT.h"
#include <cmath>

namespace Render
{

static constexpr double PI = 3.14159265358979323846;

void RenderCameraRT::generateRay(double u, double v, double lensU, double lensV,
                                 CameraRay& ray) const
{
    // Compute camera basis vectors
    Base::Vector3d forward = (target - position);
    double focalLen = forward.Length();
    if (focalLen < 1e-10) {
        forward = Base::Vector3d(0.0, 1.0, 0.0);
    }
    else {
        forward = forward / focalLen;
    }

    Base::Vector3d right = forward.Cross(up);
    double rightLen = right.Length();
    if (rightLen < 1e-10) {
        right = Base::Vector3d(1.0, 0.0, 0.0);
    }
    else {
        right = right / rightLen;
    }

    Base::Vector3d cameraUp = right.Cross(forward);
    double cameraUpLen = cameraUp.Length();
    if (cameraUpLen > 1e-10) {
        cameraUp = cameraUp / cameraUpLen;
    }

    if (orthographic) {
        // Orthographic: parallel rays with position offset
        double halfHeight = focalDistance * std::tan(fov * 0.5 * PI / 180.0);
        double halfWidth = halfHeight * aspectRatio;

        Base::Vector3d offset = right * ((u - 0.5) * 2.0 * halfWidth)
                              + cameraUp * ((v - 0.5) * 2.0 * halfHeight);

        ray.origin = position + offset;
        ray.direction = forward;
        ray.weight = 1.0;
    }
    else {
        // Perspective projection
        double halfHeight = focalDistance * std::tan(fov * 0.5 * PI / 180.0);
        double halfWidth = halfHeight * aspectRatio;

        Base::Vector3d focalPoint = position + forward * focalDistance;
        Base::Vector3d pixelOffset = right * ((u - 0.5) * 2.0 * halfWidth)
                                  + cameraUp * ((v - 0.5) * 2.0 * halfHeight);
        Base::Vector3d focalHit = focalPoint + pixelOffset;

        // Depth of field
        if (aperture > 0.0) {
            // Sample point on lens disk
            double lensRadius = aperture * 0.5;
            double r1 = lensU * 2.0 - 1.0;
            double r2 = lensV * 2.0 - 1.0;

            // Concentric disk mapping
            double phi = std::atan2(r2, r1);
            double radius = std::sqrt(r1 * r1 + r2 * r2);
            if (radius > 1.0) {
                radius = 1.0;
            }
            double lensX = std::cos(phi) * radius * lensRadius;
            double lensY = std::sin(phi) * radius * lensRadius;

            Base::Vector3d lensPoint = position + right * lensX + cameraUp * lensY;
            ray.origin = lensPoint;
            ray.direction = (focalHit - lensPoint);
            double dirLen = ray.direction.Length();
            if (dirLen > 1e-10) {
                ray.direction = ray.direction / dirLen;
            }

            // Adjust focal distance for DOF sampling
            ray.weight = 1.0;
        }
        else {
            ray.origin = position;
            ray.direction = (focalHit - position);
            double dirLen = ray.direction.Length();
            if (dirLen > 1e-10) {
                ray.direction = ray.direction / dirLen;
            }
            ray.weight = 1.0;
        }
    }
}

}  // namespace Render
