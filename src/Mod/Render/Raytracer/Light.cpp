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

#include "Light.h"
#include <cmath>
#include <algorithm>

namespace Render
{

static constexpr double PI = 3.14159265358979323846;

LightSample sampleLight(const Light& light,
                        const Base::Vector3d& surfacePoint,
                        double u, double v)
{
    LightSample sample;

    switch (light.type) {
        case LightType::Point: {
            sample.point = light.position;
            Base::Vector3d toLight = light.position - surfacePoint;
            double dist = toLight.Length();
            if (dist < 1e-10) {
                sample.pdf = 0.0;
                return sample;
            }
            sample.normal = toLight / dist;
            // Inverse square falloff
            sample.radiance = light.color * (light.intensity / (dist * dist));
            // Solid angle PDF: 1 / (dist^2 * area of sphere = 4*pi*dist^2) * dist^2
            // For point light: probability of hitting direction = 1/(4*pi)
            sample.pdf = 1.0 / (4.0 * PI);
            break;
        }
        case LightType::Directional: {
            // Directional light is at infinity
            sample.point = surfacePoint + light.direction * 1e6;
            sample.normal = -light.direction;
            sample.radiance = light.color * light.intensity;
            // Delta light: PDF is 1 (all energy in one direction)
            sample.pdf = 1.0;
            break;
        }
        case LightType::Area: {
            // Sample a point on the area light disk
            // Create coordinate system from direction
            Base::Vector3d tangent, bitangent;
            if (std::abs(light.direction.x) > std::abs(light.direction.z)) {
                double invLen = 1.0 / std::sqrt(light.direction.x * light.direction.x +
                                                light.direction.y * light.direction.y);
                tangent = Base::Vector3d(-light.direction.y * invLen,
                                          light.direction.x * invLen, 0.0);
            }
            else {
                double invLen = 1.0 / std::sqrt(light.direction.y * light.direction.y +
                                                light.direction.z * light.direction.z);
                tangent = Base::Vector3d(0.0, -light.direction.z * invLen,
                                          light.direction.y * invLen);
            }
            bitangent = light.direction.Cross(tangent);

            // Concentric disk mapping
            double r1 = u * 2.0 - 1.0;
            double r2 = v * 2.0 - 1.0;
            double phi = std::atan2(r2, r1);
            double radius = std::min(std::sqrt(r1 * r1 + r2 * r2), 1.0);
            double diskX = std::cos(phi) * radius * light.size;
            double diskY = std::sin(phi) * radius * light.size;

            sample.point = light.position + tangent * diskX + bitangent * diskY;
            sample.normal = light.direction;

            Base::Vector3d toLight = sample.point - surfacePoint;
            double dist = toLight.Length();
            if (dist < 1e-10) {
                sample.pdf = 0.0;
                return sample;
            }

            double cosAtSurface = std::max(0.0, sample.normal.Dot(-toLight / dist));
            double lightArea = PI * light.size * light.size;

            sample.radiance = light.color * (light.intensity / (dist * dist));

            // PDF: 1 / (lightArea * cosTheta_light / dist^2)
            // This is area measure to solid angle measure conversion
            double denom = lightArea * cosAtSurface;
            if (denom < 1e-10) {
                sample.pdf = 0.0;
            }
            else {
                sample.pdf = dist * dist / denom;
            }
            break;
        }
        case LightType::Environment: {
            // Environment map sampling: direction from u,v
            double theta = std::acos(std::max(-1.0, std::min(1.0, 2.0 * v - 1.0)));
            double phi = 2.0 * PI * u;
            sample.point = surfacePoint + Base::Vector3d(
                std::sin(theta) * std::cos(phi),
                std::sin(theta) * std::sin(phi),
                std::cos(theta)
            ) * 1e6;
            sample.normal = Base::Vector3d(
                std::sin(theta) * std::cos(phi),
                std::sin(theta) * std::sin(phi),
                std::cos(theta)
            );
            sample.radiance = light.color * light.intensity;
            sample.pdf = 1.0 / (4.0 * PI);
            break;
        }
    }

    return sample;
}

double lightPDF(const Light& light,
                const Base::Vector3d& surfacePoint,
                const LightSample& sample)
{
    switch (light.type) {
        case LightType::Point:
            return 1.0 / (4.0 * PI);
        case LightType::Directional:
            return 1.0;
        case LightType::Area: {
            Base::Vector3d toLight = sample.point - surfacePoint;
            double dist = toLight.Length();
            if (dist < 1e-10) return 0.0;
            double cosAtSurface = std::max(0.0, sample.normal.Dot(-toLight / dist));
            double lightArea = PI * light.size * light.size;
            double denom = lightArea * cosAtSurface;
            if (denom < 1e-10) return 0.0;
            return dist * dist / denom;
        }
        case LightType::Environment:
            return 1.0 / (4.0 * PI);
    }
    return 0.0;
}

}  // namespace Render
