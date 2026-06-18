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

#include "BVH.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <limits>
#include <cassert>

namespace Render
{

// ========== AABB ==========

AABB::AABB()
{
    double maxVal = std::numeric_limits<double>::max();
    min = Base::Vector3d(maxVal, maxVal, maxVal);
    max = Base::Vector3d(-maxVal, -maxVal, -maxVal);
}

AABB::AABB(const Base::Vector3d& p) : min(p), max(p) {}

void AABB::expand(const AABB& other)
{
    if (other.min.x < min.x) min.x = other.min.x;
    if (other.min.y < min.y) min.y = other.min.y;
    if (other.min.z < min.z) min.z = other.min.z;
    if (other.max.x > max.x) max.x = other.max.x;
    if (other.max.y > max.y) max.y = other.max.y;
    if (other.max.z > max.z) max.z = other.max.z;
}

void AABB::expand(const Base::Vector3d& point)
{
    if (point.x < min.x) min.x = point.x;
    if (point.y < min.y) min.y = point.y;
    if (point.z < min.z) min.z = point.z;
    if (point.x > max.x) max.x = point.x;
    if (point.y > max.y) max.y = point.y;
    if (point.z > max.z) max.z = point.z;
}

bool AABB::intersect(const Ray& ray, double& tmin, double& tmax) const
{
    for (int i = 0; i < 3; ++i) {
        double invD = 1.0 / ray.direction[i];
        double t0 = (min[i] - ray.origin[i]) * invD;
        double t1 = (max[i] - ray.origin[i]) * invD;
        if (invD < 0.0) {
            std::swap(t0, t1);
        }
        tmin = (t0 > tmin) ? t0 : tmin;
        tmax = (t1 < tmax) ? t1 : tmax;
        if (tmax <= tmin) {
            return false;
        }
    }
    return true;
}

double AABB::surfaceArea() const
{
    double dx = max.x - min.x;
    double dy = max.y - min.y;
    double dz = max.z - min.z;
    return 2.0 * (dx * dy + dy * dz + dz * dx);
}

Base::Vector3d AABB::centroid() const
{
    return (min + max) * 0.5;
}

int AABB::maxExtent() const
{
    double dx = max.x - min.x;
    double dy = max.y - min.y;
    double dz = max.z - min.z;
    if (dx > dy && dx > dz) return 0;
    if (dy > dz) return 1;
    return 2;
}

// ========== BVH ==========

int32_t BVH::buildRecursive(std::vector<Triangle>& tris, int start, int end)
{
    int32_t nodeIdx = static_cast<int32_t>(nodes.size());
    nodes.emplace_back();

    // Compute bounds of all triangles in this range
    AABB bounds;
    bounds.expand(tris[start].v0);
    bounds.expand(tris[start].v1);
    bounds.expand(tris[start].v2);
    for (int i = start + 1; i < end; ++i) {
        bounds.expand(tris[i].v0);
        bounds.expand(tris[i].v1);
        bounds.expand(tris[i].v2);
    }

    int count = end - start;

    if (count <= MAX_LEAF_SIZE) {
        nodes[nodeIdx].bounds = bounds;
        nodes[nodeIdx].triangleOffset = start;
        nodes[nodeIdx].triangleCount = count;
        nodes[nodeIdx].left = -1;
        nodes[nodeIdx].right = -1;
        return nodeIdx;
    }

    // SAH: try each axis
    int axis = bounds.maxExtent();
    AABB centroidBounds;
    for (int i = start; i < end; ++i) {
        centroidBounds.expand(tris[i].v0);
        centroidBounds.expand(tris[i].v1);
        centroidBounds.expand(tris[i].v2);
        Base::Vector3d c = (tris[i].v0 + tris[i].v1 + tris[i].v2) * (1.0 / 3.0);
        centroidBounds.expand(c);
    }

    // Try SAH with bins
    constexpr int NUM_BINS = 12;
    struct Bin {
        AABB bounds;
        int count = 0;
    };
    std::vector<Bin> bins(NUM_BINS);

    double axisMin = centroidBounds.min[axis];
    double axisMax = centroidBounds.max[axis];
    double axisRange = axisMax - axisMin;

    if (axisRange < 1e-12) {
        // All centroids same position - make leaf
        nodes[nodeIdx].bounds = bounds;
        nodes[nodeIdx].triangleOffset = start;
        nodes[nodeIdx].triangleCount = count;
        nodes[nodeIdx].left = -1;
        nodes[nodeIdx].right = -1;
        return nodeIdx;
    }

    double binSize = axisRange / NUM_BINS;

    for (int i = start; i < end; ++i) {
        Base::Vector3d c = (tris[i].v0 + tris[i].v1 + tris[i].v2) * (1.0 / 3.0);
        int binIdx = static_cast<int>((c[axis] - axisMin) / binSize);
        if (binIdx >= NUM_BINS) binIdx = NUM_BINS - 1;
        bins[binIdx].bounds.expand(tris[i].v0);
        bins[binIdx].bounds.expand(tris[i].v1);
        bins[binIdx].bounds.expand(tris[i].v2);
        bins[binIdx].count++;
    }

    // Evaluate SAH cost for each split
    double bestCost = std::numeric_limits<double>::max();
    int bestSplit = -1;

    std::vector<AABB> leftBounds(NUM_BINS);
    std::vector<AABB> rightBounds(NUM_BINS);
    std::vector<int> leftCount(NUM_BINS, 0);
    std::vector<int> rightCount(NUM_BINS, 0);

    // Prefix scan from left
    leftBounds[0] = bins[0].bounds;
    leftCount[0] = bins[0].count;
    for (int i = 1; i < NUM_BINS; ++i) {
        leftBounds[i] = leftBounds[i - 1];
        leftBounds[i].expand(bins[i].bounds);
        leftCount[i] = leftCount[i - 1] + bins[i].count;
    }

    // Suffix scan from right
    rightBounds[NUM_BINS - 1] = bins[NUM_BINS - 1].bounds;
    rightCount[NUM_BINS - 1] = bins[NUM_BINS - 1].count;
    for (int i = NUM_BINS - 2; i >= 0; --i) {
        rightBounds[i] = rightBounds[i + 1];
        rightBounds[i].expand(bins[i].bounds);
        rightCount[i] = rightCount[i + 1] + bins[i].count;
    }

    double parentArea = bounds.surfaceArea();
    for (int i = 0; i < NUM_BINS - 1; ++i) {
        if (leftCount[i] == 0 || rightCount[i + 1] == 0) continue;

        double leftArea = leftBounds[i].surfaceArea();
        double rightArea = rightBounds[i + 1].surfaceArea();
        double cost = C_TRAVERSAL + C_INTERSECTION *
            (leftArea / parentArea * leftCount[i] +
             rightArea / parentArea * rightCount[i + 1]);

        if (cost < bestCost) {
            bestCost = cost;
            bestSplit = i;
        }
    }

    // Leaf cost: C_INTERSECTION * count
    double leafCost = C_INTERSECTION * count;

    if (bestSplit < 0 || bestCost > leafCost) {
        // Make leaf
        nodes[nodeIdx].bounds = bounds;
        nodes[nodeIdx].triangleOffset = start;
        nodes[nodeIdx].triangleCount = count;
        nodes[nodeIdx].left = -1;
        nodes[nodeIdx].right = -1;
        return nodeIdx;
    }

    // Partition triangles around split
    double splitBound = axisMin + (bestSplit + 1) * binSize;
    auto midIt = std::partition(
        tris.begin() + start, tris.begin() + end,
        [&](const Triangle& tri) {
            Base::Vector3d c = (tri.v0 + tri.v1 + tri.v2) * (1.0 / 3.0);
            return c[axis] < splitBound;
        });

    // Fallback: if partition produced empty side, use median
    int mid = static_cast<int>(midIt - tris.begin());
    if (mid == start || mid == end) {
        mid = start + count / 2;
    }

    nodes[nodeIdx].bounds = bounds;
    nodes[nodeIdx].left = buildRecursive(tris, start, mid);
    nodes[nodeIdx].right = buildRecursive(tris, mid, end);
    nodes[nodeIdx].triangleCount = 0;

    return nodeIdx;
}

void BVH::flattenRecursive(int32_t nodeIdx, int32_t& slot)
{
    BVHNode& node = nodes[nodeIdx];
    int32_t currentSlot = slot++;
    flatNodes[currentSlot] = node;

    if (node.isLeaf()) {
        return;
    }

    if (node.left >= 0) {
        flattenRecursive(node.left, slot);
    }
    if (node.right >= 0) {
        flattenRecursive(node.right, slot);
    }

    flatNodes[currentSlot].left = slot;
    if (node.right >= 0) {
        // right comes after left subtree
    }
}

void BVH::build(std::vector<Triangle>& triangles)
{
    if (triangles.empty()) {
        nodes.clear();
        flatNodes.clear();
        orderedTriangles.clear();
        return;
    }

    nodes.clear();
    orderedTriangles = triangles;

    buildRecursive(orderedTriangles, 0, static_cast<int>(orderedTriangles.size()));

    // Flatten tree
    flatNodes.resize(nodes.size());
    int32_t slot = 0;
    flattenRecursive(0, slot);
}

bool BVH::intersect(const Ray& ray, HitInfo& hit) const
{
    if (flatNodes.empty()) return false;

    bool found = false;
    double closest = hit.t;

    int32_t stack[64];
    int32_t stackPtr = 0;
    stack[stackPtr++] = 0;

    while (stackPtr > 0) {
        int32_t nodeIdx = stack[--stackPtr];
        const BVHNode& node = flatNodes[nodeIdx];

        double tmin = ray.tmin;
        double tmax = closest;
        if (!node.bounds.intersect(ray, tmin, tmax)) {
            continue;
        }

        if (node.isLeaf()) {
            for (int i = 0; i < node.triangleCount; ++i) {
                const Triangle& tri = orderedTriangles[node.triangleOffset + i];

                Base::Vector3d edge1 = tri.v1 - tri.v0;
                Base::Vector3d edge2 = tri.v2 - tri.v0;
                Base::Vector3d h = ray.direction.Cross(edge2);
                double a = edge1.Dot(h);

                if (a > -1e-12 && a < 1e-12) continue;

                double f = 1.0 / a;
                Base::Vector3d s = ray.origin - tri.v0;
                double u = f * s.Dot(h);
                if (u < 0.0 || u > 1.0) continue;

                Base::Vector3d q = s.Cross(edge1);
                double v = f * ray.direction.Dot(q);
                if (v < 0.0 || u + v > 1.0) continue;

                double t = f * edge2.Dot(q);
                if (t > ray.tmin && t < closest) {
                    closest = t;
                    found = true;
                    hit.t = t;
                    hit.point = ray.origin + ray.direction * t;
                    hit.normal = tri.normal;
                    hit.materialIndex = tri.materialIndex;
                    hit.triangleIndex = static_cast<uint32_t>(node.triangleOffset + i);
                }
            }
        }
        else {
            if (node.right >= 0) stack[stackPtr++] = node.right;
            if (node.left >= 0) stack[stackPtr++] = node.left;
        }
    }

    if (found) {
        hit.hit = true;
        hit.t = closest;
    }
    return found;
}

bool BVH::intersectAny(const Ray& ray) const
{
    if (flatNodes.empty()) return false;

    int32_t stack[64];
    int32_t stackPtr = 0;
    stack[stackPtr++] = 0;

    while (stackPtr > 0) {
        int32_t nodeIdx = stack[--stackPtr];
        const BVHNode& node = flatNodes[nodeIdx];

        double tmin = ray.tmin;
        double tmax = ray.tmax;
        if (!node.bounds.intersect(ray, tmin, tmax)) {
            continue;
        }

        if (node.isLeaf()) {
            for (int i = 0; i < node.triangleCount; ++i) {
                const Triangle& tri = orderedTriangles[node.triangleOffset + i];

                Base::Vector3d edge1 = tri.v1 - tri.v0;
                Base::Vector3d edge2 = tri.v2 - tri.v0;
                Base::Vector3d h = ray.direction.Cross(edge2);
                double a = edge1.Dot(h);

                if (a > -1e-12 && a < 1e-12) continue;

                double f = 1.0 / a;
                Base::Vector3d s = ray.origin - tri.v0;
                double u = f * s.Dot(h);
                if (u < 0.0 || u > 1.0) continue;

                Base::Vector3d q = s.Cross(edge1);
                double v = f * ray.direction.Dot(q);
                if (v < 0.0 || u + v > 1.0) continue;

                double t = f * edge2.Dot(q);
                if (t > ray.tmin && t < ray.tmax) {
                    return true;
                }
            }
        }
        else {
            if (node.right >= 0) stack[stackPtr++] = node.right;
            if (node.left >= 0) stack[stackPtr++] = node.left;
        }
    }

    return false;
}

}  // namespace Render
