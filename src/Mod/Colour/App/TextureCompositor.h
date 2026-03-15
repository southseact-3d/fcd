// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_APP_TEXTURECOMPOSITOR_H
#define COLOUR_APP_TEXTURECOMPOSITOR_H

#include <cstdint>
#include <string>
#include <vector>
#include <chrono>
#include <unordered_map>

#include "ColourMap.h"
#include "SketchRasterizer.h"
#include "UVAtlas.h"

namespace Colour
{

struct AtlasImage
{
    int width {2048};
    int height {2048};
    std::vector<std::uint8_t> rgba;
};

struct PerformanceStats
{
    double tessellationTimeMs {0.0};
    double uvGenerationTimeMs {0.0};
    double imageCompositingTimeMs {0.0};
    double sketchRasterizationTimeMs {0.0};
    double totalCompositingTimeMs {0.0};
    int faceCount {0};
    int layerCount {0};
    int atlasResolution {0};

    std::string toString() const;
};

class TextureCompositor
{
public:
    TextureCompositor() = default;

    AtlasImage compose(const ColourMap& colourMap, const UVAtlas& atlas);
    AtlasImage composeMetallicRoughness(const ColourMap& colourMap, const UVAtlas& atlas);
    bool writePpm(const AtlasImage& image, const std::string& outputPath) const;
    bool writePng(const AtlasImage& image, const std::string& outputPath) const;
    bool encodePng(const AtlasImage& image, std::vector<std::uint8_t>& pngBytes) const;

    void setPerformanceStatsEnabled(bool enabled) { m_enableStats = enabled; }
    const PerformanceStats& getPerformanceStats() const { return m_stats; }
    void resetPerformanceStats() { m_stats = PerformanceStats(); }

private:
    bool m_enableStats = false;
    mutable PerformanceStats m_stats;
    static std::size_t pixelIndex(const AtlasImage& image, int x, int y);
    static void blendPixel(AtlasImage& image, int x, int y, const RGBA& colour, BlendMode mode);
    static std::uint32_t crc32(const std::uint8_t* data, std::size_t size);
    static std::uint32_t adler32(const std::uint8_t* data, std::size_t size);
    static void writeChunk(
        std::vector<std::uint8_t>& out,
        const char type[4],
        const std::vector<std::uint8_t>& payload
    );
    static void appendBigEndianU32(std::vector<std::uint8_t>& out, std::uint32_t value);

    void compositeImageLayer(
        AtlasImage& image,
        const IslandRect& island,
        const ImageData& img,
        const RGBA& layerColour,
        BlendMode mode,
        int atlasResolution
    ) const;

    void compositeSketchLayer(
        AtlasImage& image,
        const IslandRect& island,
        const SketchData& sketch,
        const RGBA& layerColour,
        BlendMode mode,
        int atlasResolution
    ) const;
};

}  // namespace Colour

#endif  // COLOUR_APP_TEXTURECOMPOSITOR_H
