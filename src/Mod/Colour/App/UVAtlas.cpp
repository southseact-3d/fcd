// SPDX-License-Identifier: LGPL-2.1-or-later

#include "UVAtlas.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_map>

namespace Colour
{

namespace
{
struct Bounds2D
{
    float minU {0.0F};
    float minV {0.0F};
    float maxU {1.0F};
    float maxV {1.0F};
};

UV projectVertex(const Vertex& vertex)
{
    const float ax = std::abs(vertex.normal.x);
    const float ay = std::abs(vertex.normal.y);
    const float az = std::abs(vertex.normal.z);

    if (ax >= ay && ax >= az) {
        return {vertex.position.y, vertex.position.z};
    }
    if (ay >= ax && ay >= az) {
        return {vertex.position.x, vertex.position.z};
    }
    return {vertex.position.x, vertex.position.y};
}

}  // namespace

bool UVAtlas::generateFacePerIsland(
    const std::vector<Vertex>& vertices,
    const std::vector<Triangle>& triangles,
    int resolution,
    int paddingPixels
)
{
    if (vertices.empty() || triangles.empty() || resolution <= 0 || paddingPixels < 0) {
        return false;
    }

    paddingPixels = std::max(0, std::min(paddingPixels, resolution / 8));

    m_resolution = resolution;
    m_triangleUvs.clear();
    m_islandsByFace.clear();
    m_triangleUvs.resize(triangles.size());

    std::map<int, std::vector<std::size_t>> faces;
    for (std::size_t i = 0; i < triangles.size(); ++i) {
        faces[triangles[i].faceIndex].push_back(i);
    }

    const int faceCount = static_cast<int>(faces.size());
    const int grid = std::max(1, static_cast<int>(std::ceil(std::sqrt(static_cast<float>(faceCount)))));
    const int tile = std::max(1, resolution / grid);

    int faceOrdinal = 0;
    for (const auto& [faceIndex, triIds] : faces) {
        const int gx = faceOrdinal % grid;
        const int gy = faceOrdinal / grid;
        const int tileX = gx * tile + paddingPixels;
        const int tileY = gy * tile + paddingPixels;
        const int tileW = std::max(1, tile - (2 * paddingPixels));
        const int tileH = std::max(1, tile - (2 * paddingPixels));

        m_islandsByFace[faceIndex] = IslandRect {tileX, tileY, tileW, tileH};

        Bounds2D b;
        bool first = true;
        for (std::size_t triId : triIds) {
            const Triangle& tri = triangles[triId];
            for (std::uint32_t idx : tri.indices) {
                if (idx >= vertices.size()) {
                    return false;
                }
                UV uv = projectVertex(vertices[idx]);
                if (first) {
                    b.minU = b.maxU = uv.u;
                    b.minV = b.maxV = uv.v;
                    first = false;
                }
                else {
                    b.minU = std::min(b.minU, uv.u);
                    b.maxU = std::max(b.maxU, uv.u);
                    b.minV = std::min(b.minV, uv.v);
                    b.maxV = std::max(b.maxV, uv.v);
                }
            }
        }

        const float du = std::max(1.0e-7F, b.maxU - b.minU);
        const float dv = std::max(1.0e-7F, b.maxV - b.minV);

        for (std::size_t triId : triIds) {
            const Triangle& tri = triangles[triId];
            TriangleUv triUv;
            for (int k = 0; k < 3; ++k) {
                UV local = projectVertex(vertices[tri.indices[k]]);
                const float uNorm = (local.u - b.minU) / du;
                const float vNorm = (local.v - b.minV) / dv;

                const float px = (static_cast<float>(tileX) + (uNorm * static_cast<float>(tileW)))
                    / static_cast<float>(resolution);
                const float py = (static_cast<float>(tileY) + (vNorm * static_cast<float>(tileH)))
                    / static_cast<float>(resolution);
                triUv.uv[k] = UV {px, py};
            }
            m_triangleUvs[triId] = triUv;
        }

        ++faceOrdinal;
    }

    return true;
}

bool UVAtlas::generateFacePerIsland(
    const std::vector<Vertex>& vertices,
    const std::vector<Triangle>& triangles,
    const GenerateOptions& options
)
{
    const int scaledResolution = std::max(
        1,
        static_cast<int>(static_cast<float>(options.resolution) * std::max(0.1F, options.islandScale))
    );
    return generateFacePerIsland(vertices, triangles, scaledResolution, options.paddingPixels);
}

const std::vector<TriangleUv>& UVAtlas::triangleUvs() const
{
    return m_triangleUvs;
}

const std::unordered_map<int, IslandRect>& UVAtlas::islandsByFace() const
{
    return m_islandsByFace;
}

int UVAtlas::resolution() const
{
    return m_resolution;
}

}  // namespace Colour
