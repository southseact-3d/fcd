// SPDX-License-Identifier: LGPL-2.1-or-later

#include "XAtlasWrapper.h"
#include "UVAtlas.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>

// Include the actual xatlas library
#define XATLAS_ASSERT assert
#define XATLAS_MALLOC(sz) std::malloc(sz)
#define XATLAS_FREE(p) std::free(p)
#include "xatlas/xatlas.h"

namespace Colour
{
    
class XAtlasWrapper::Impl
{
public:
    bool generateAtlas(
        const std::vector<Vertex>& vertices,
        const std::vector<Triangle>& triangles,
        UVAtlas::GenerateOptions options,
        std::vector<TriangleUv>& outTriangleUvs,
        std::unordered_map<int, IslandRect>& outIslandsByFace,
        int& outResolution
    )
    {
        if (vertices.empty() || triangles.empty() || options.resolution <= 0) {
            return false;
        }

        // Create xatlas mesh
        xatlas::MeshDecl meshDecl;
        meshDecl.vertexCount = vertices.size();
        meshDecl.vertexPositionData = vertices.data();
        meshDecl.vertexPositionStride = sizeof(Vertex);
        meshDecl.vertexNormalData = vertices.data();
        meshDecl.vertexNormalStride = sizeof(Vertex);
        meshDecl.vertexUvData = nullptr;
        meshDecl.vertexUvStride = 0;
        
        // xatlas expects indices as uint32_t array
        std::vector<uint32_t> indices;
        indices.reserve(triangles.size() * 3);
        for (const auto& tri : triangles) {
            indices.push_back(tri.indices[0]);
            indices.push_back(tri.indices[1]);
            indices.push_back(tri.indices[2]);
        }
        
        meshDecl.indexCount = indices.size();
        meshDecl.indexData = indices.data();
        meshDecl.indexFormat = xatlas::IndexFormat::UInt32;

        // Create atlas
        xatlas::Atlas* atlas = xatlas::Create();
        
        // Add mesh to atlas
        xatlas::AddMeshError err = xatlas::AddMesh(atlas, meshDecl);
        if (err != xatlas::AddMeshError::Success) {
            xatlas::Destroy(atlas);
            return false;
        }
        
        // Configure chart options
        xatlas::ChartOptions chartOptions;
        chartOptions.maxChartSize = 1.0f; // No limit on chart size
        chartOptions.maxBoundaryLength = std::numeric_limits<float>::max();
        chartOptions.normalDeviationWeight = 0.25f;
        chartOptions.roundnessWeight = 0.0f;
        chartOptions.straightnessWeight = 6.0f;
        chartOptions.normalSeamWeight = 4.0f;
        chartOptions.textureSeamWeight = 1.0f;
        chartOptions.seamWeightMultiplier = 1.0f;
        
        // Configure pack options
        xatlas::PackOptions packOptions;
        packOptions.resolution = options.resolution;
        packOptions.padding = options.paddingPixels;
        packOptions.maxChartAtlasRatio = 1.0f; // No limit on atlas usage
        packOptions.blockAlign = true;
        packOptions.blockOffset = true;
        packOptions.bruteForce = false;
        packOptions.createImages = false;
        
        // Generate the atlas
        xatlas::Generate(atlas, chartOptions, packOptions);
        
        // Get the results
        const xatlas::Mesh* mesh = xatlas::GetMesh(atlas, 0);
        outResolution = atlas->width;
        outTriangleUvs.clear();
        outIslandsByFace.clear();
        outTriangleUvs.resize(triangles.size());
        
        // Map xatlas results to our format per triangle
        for (size_t triIdx = 0; triIdx < triangles.size(); ++triIdx) {
            const Triangle& tri = triangles[triIdx];
            TriangleUv triUv;
            
            // Get UVs for each vertex of the triangle
            for (int k = 0; k < 3; ++k) {
                uint32_t vertexIndex = tri.indices[k];
                
                // Find the vertex in the xatlas mesh
                bool found = false;
                for (size_t vtxIdx = 0; vtxIdx < mesh->vertexCount; ++vtxIdx) {
                    const xatlas::Vertex& vertex = mesh->vertexArray[vtxIdx];
                    if (vertex.index == vertexIndex) {
                        const xatlas::VertexAttributeRef<xatlas::UV>& uvRef = 
                            mesh->vertexAttributeArray[vtxIdx];
                        triUv.uv[k].u = uvRef[0].x;
                        triUv.uv[k].v = uvRef[0].y;
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    // Fallback: compute planar projection
                    const Vertex& vertex = vertices[vertexIndex];
                    const float ax = std::abs(vertex.normal.x);
                    const float ay = std::abs(vertex.normal.y);
                    const float az = std::abs(vertex.normal.z);
                    
                    if (ax >= ay && ax >= az) {
                        triUv.uv[k] = {vertex.position.y, vertex.position.z};
                    } else if (ay >= ax && ay >= az) {
                        triUv.uv[k] = {vertex.position.x, vertex.position.z};
                    } else {
                        triUv.uv[k] = {vertex.position.x, vertex.position.y};
                    }
                }
            }
            
            outTriangleUvs[triIdx] = triUv;
        }
        
        // Calculate island bounds (face-based) - group triangles by face index
        std::map<int, std::vector<size_t>> trianglesByFace;
        for (size_t i = 0; i < triangles.size(); ++i) {
            trianglesByFace[triangles[i].faceIndex].push_back(i);
        }
        
        int faceOrdinal = 0;
        const int faceCount = static_cast<int>(trianglesByFace.size());
        const int grid = std::max(1, static_cast<int>(std::ceil(std::sqrt(static_cast<float>(faceCount)))));
        const int tile = std::max(1, options.resolution / grid);
        const int padding = std::max(0, std::min(options.paddingPixels, options.resolution / 8));
        
        for (const auto& [faceIndex, triIds] : trianglesByFace) {
            const int gx = faceOrdinal % grid;
            const int gy = faceOrdinal / grid;
            const int tileX = gx * tile + padding;
            const int tileY = gy * tile + padding;
            const int tileW = std::max(1, tile - (2 * padding));
            const int tileH = std::max(1, tile - (2 * padding));
            
            outIslandsByFace[faceIndex] = IslandRect {tileX, tileY, tileW, tileH};
            ++faceOrdinal;
        }
        
        // Clean up
        xatlas::Destroy(atlas);
        return true;
    }
    
    bool isAvailable() const {
        // With actual xatlas integration, this should return true
        return true;
    }
    
};

XAtlasWrapper::XAtlasWrapper()
    : m_impl(std::make_unique<Impl>())
{
}

XAtlasWrapper::~XAtlasWrapper() = default;

bool XAtlasWrapper::generateAtlas(
    const std::vector<Vertex>& vertices,
    const std::vector<Triangle>& triangles,
    UVAtlas::GenerateOptions options,
    std::vector<TriangleUv>& outTriangleUvs,
    std::unordered_map<int, IslandRect>& outIslandsByFace,
    int& outResolution
)
{
    return m_impl->generateAtlas(vertices, triangles, options, outTriangleUvs, outIslandsByFace, outResolution);
}

bool XAtlasWrapper::isAvailable() const {
    return m_impl->isAvailable();
}

} // namespace Colour