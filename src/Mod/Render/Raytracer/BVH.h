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

#ifndef RENDER_BVH_H
#define RENDER_BVH_H

#include <vector>
#include <cstdint>
#include <Base/Vector3D.h>
#include "RenderRaytracerGlobal.h"

namespace Render
{

struct Ray
{
    Base::Vector3d origin;
    Base::Vector3d direction;
    double tmin = 1e-6;
    double tmax = 1e30;
};

struct HitInfo
{
    double t = 1e30;
    Base::Vector3d point;
    Base::Vector3d normal;
    uint32_t materialIndex = 0;
    uint32_t triangleIndex = 0;
    bool hit = false;
};

struct Triangle
{
    Base::Vector3d v0, v1, v2;
    Base::Vector3d normal;
    uint32_t materialIndex = 0;
};

struct AABB
{
    Base::Vector3d min;
    Base::Vector3d max;

    AABB();
    AABB(const Base::Vector3d& p);
    void expand(const AABB& other);
    void expand(const Base::Vector3d& point);
    bool intersect(const Ray& ray, double& tmin, double& tmax) const;
    double surfaceArea() const;
    Base::Vector3d centroid() const;
    int maxExtent() const;
};

struct BVHNode
{
    AABB bounds;
    int32_t left = -1;
    int32_t right = -1;
    int32_t triangleOffset = 0;
    int32_t triangleCount = 0;

    bool isLeaf() const { return triangleCount > 0; }
};

class RenderRaytracerExport BVH
{
public:
    BVH() = default;
    void build(std::vector<Triangle>& triangles);
    bool intersect(const Ray& ray, HitInfo& hit) const;
    bool intersectAny(const Ray& ray) const;

    const std::vector<Triangle>& getOrderedTriangles() const { return orderedTriangles; }

private:
    int32_t buildRecursive(std::vector<Triangle>& triangles, int start, int end);
    void flattenRecursive(int32_t nodeIdx, int32_t& slot);

    static constexpr int MAX_LEAF_SIZE = 4;
    static constexpr double C_TRAVERSAL = 1.0;
    static constexpr double C_INTERSECTION = 1.0;

    std::vector<BVHNode> nodes;
    std::vector<BVHNode> flatNodes;
    std::vector<Triangle> orderedTriangles;
};

}  // namespace Render

#endif  // RENDER_BVH_H
