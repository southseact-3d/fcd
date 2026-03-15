// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_APP_UVATLAS_H
#define COLOUR_APP_UVATLAS_H

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace Colour
{

struct Vec3
{
    float x {0.0F};
    float y {0.0F};
    float z {0.0F};
};

struct UV
{
    float u {0.0F};
    float v {0.0F};
};

struct Vertex
{
    Vec3 position;
    Vec3 normal;
};

struct Triangle
{
    std::uint32_t indices[3] {0, 0, 0};
    int faceIndex {0};
};

struct IslandRect
{
    int x {0};
    int y {0};
    int width {1};
    int height {1};
};

struct TriangleUv
{
    UV uv[3];
};

class UVAtlas
{
public:
    struct GenerateOptions
    {
        int resolution {2048};
        int paddingPixels {2};
        float islandScale {1.0F};
    };

    bool generateFacePerIsland(
        const std::vector<Vertex>& vertices,
        const std::vector<Triangle>& triangles,
        int resolution,
        int paddingPixels
    );

    bool generateFacePerIsland(
        const std::vector<Vertex>& vertices,
        const std::vector<Triangle>& triangles,
        const GenerateOptions& options
    );

    const std::vector<TriangleUv>& triangleUvs() const;
    const std::unordered_map<int, IslandRect>& islandsByFace() const;
    int resolution() const;

private:
    std::vector<TriangleUv> m_triangleUvs;
    std::unordered_map<int, IslandRect> m_islandsByFace;
    int m_resolution {2048};
};

}  // namespace Colour

#endif  // COLOUR_APP_UVATLAS_H
