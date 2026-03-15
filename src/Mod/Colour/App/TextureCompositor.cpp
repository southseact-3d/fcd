// SPDX-License-Identifier: LGPL-2.1-or-later

#include "TextureCompositor.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <sstream>

namespace Colour
{

std::string PerformanceStats::toString() const
{
    std::ostringstream oss;
    oss << "Performance Statistics:\n";
    oss << "  Face count: " << faceCount << "\n";
    oss << "  Layer count: " << layerCount << "\n";
    oss << "  Atlas resolution: " << atlasResolution << "x" << atlasResolution << "\n";
    oss << "  Total compositing time: " << totalCompositingTimeMs << " ms\n";
    oss << "  Image compositing time: " << imageCompositingTimeMs << " ms\n";
    oss << "  Sketch rasterization time: " << sketchRasterizationTimeMs << " ms\n";
    return oss.str();
}

void TextureCompositor::compositeSketchLayer(
    AtlasImage& image,
    const IslandRect& island,
    const SketchData& sketch,
    const RGBA& layerColour,
    BlendMode mode,
    int atlasResolution
) const
{
    if (sketch.geometries.empty()) {
        return;
    }

    SketchRasterizer rasterizer;
    SketchRasterizer::RasterizationOptions opts;
    opts.strokeWidth = 2;
    opts.antialiasing = true;
    rasterizer.setOptions(opts);

    rasterizer.rasterizeToAtlas(sketch, image, island, layerColour, mode);
}

void TextureCompositor::compositeImageLayer(
    AtlasImage& image,
    const IslandRect& island,
    const ImageData& img,
    const RGBA& layerColour,
    BlendMode mode,
    int atlasResolution
) const
{
    (void)atlasResolution;
    if (!img.valid() || img.width == 0 || img.height == 0) {
        return;
    }

    for (int y = 0; y < island.height; ++y) {
        for (int x = 0; x < island.width; ++x) {
            const int atlasX = island.x + x;
            const int atlasY = island.y + y;

            if (atlasX < 0 || atlasY < 0 || atlasX >= image.width || atlasY >= image.height) {
                continue;
            }

            const float uNorm = static_cast<float>(x) / static_cast<float>(island.width);
            const float vNorm = static_cast<float>(y) / static_cast<float>(island.height);

            const int imgX = static_cast<int>(uNorm * static_cast<float>(img.width - 1));
            const int imgY = static_cast<int>(vNorm * static_cast<float>(img.height - 1));

            const std::size_t imgIdx = static_cast<std::size_t>(imgY * img.width + imgX) * 4;
            if (imgIdx + 3 >= img.rgba.size()) {
                continue;
            }

            RGBA texColour;
            texColour.r = static_cast<std::uint8_t>(
                (static_cast<int>(img.rgba[imgIdx + 0]) * static_cast<int>(layerColour.r)) / 255
            );
            texColour.g = static_cast<std::uint8_t>(
                (static_cast<int>(img.rgba[imgIdx + 1]) * static_cast<int>(layerColour.g)) / 255
            );
            texColour.b = static_cast<std::uint8_t>(
                (static_cast<int>(img.rgba[imgIdx + 2]) * static_cast<int>(layerColour.b)) / 255
            );
            texColour.a = static_cast<std::uint8_t>(
                (static_cast<int>(img.rgba[imgIdx + 3]) * static_cast<int>(layerColour.a)) / 255
            );

            blendPixel(image, atlasX, atlasY, texColour, mode);
        }
    }
}

AtlasImage TextureCompositor::compose(const ColourMap& colourMap, const UVAtlas& atlas)
{
    auto startTime = std::chrono::high_resolution_clock::now();

    AtlasImage image;
    const int resolution = atlas.resolution();
    image.width = std::max(1, resolution);
    image.height = std::max(1, resolution);
    image.rgba.assign(static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height) * 4,
                      255);

    int totalLayers = 0;
    auto imageStart = std::chrono::high_resolution_clock::now();
    for (const auto& [faceIndex, rect] : atlas.islandsByFace()) {
        RGBA base = colourMap.getFaceColour(faceIndex);
        const auto& layers = colourMap.getLayers(faceIndex);
        totalLayers += static_cast<int>(layers.size());

        for (int y = rect.y; y < (rect.y + rect.height); ++y) {
            for (int x = rect.x; x < (rect.x + rect.width); ++x) {
                blendPixel(image, x, y, base, BlendMode::Normal);

                for (const Layer& layer : layers) {
                    if (!layer.visible) {
                        continue;
                    }

                    if (layer.isImageLayer && !layer.imageRef.empty()) {
                        const ImageData* imgData = colourMap.getImage(layer.imageRef);
                        if (imgData != nullptr && imgData->valid()) {
                            compositeImageLayer(image, rect, *imgData, layer.colour, layer.blendMode, resolution);
                        }
                    }
                    else if (layer.isSketchLayer && !layer.sketchRef.empty()) {
                        const SketchData* sketchData = colourMap.getSketch(layer.sketchRef);
                        if (sketchData != nullptr && !sketchData->geometries.empty()) {
                            compositeSketchLayer(image, rect, *sketchData, layer.colour, layer.blendMode, resolution);
                        }
                    }
                    else {
                        blendPixel(image, x, y, layer.colour, layer.blendMode);
                    }
                }
            }
        }
    }
    auto imageEnd = std::chrono::high_resolution_clock::now();

    auto endTime = std::chrono::high_resolution_clock::now();
    double totalMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    double imageMs = std::chrono::duration<double, std::milli>(imageEnd - imageStart).count();

    if (m_enableStats) {
        m_stats.faceCount = static_cast<int>(atlas.islandsByFace().size());
        m_stats.layerCount = totalLayers;
        m_stats.atlasResolution = resolution;
        m_stats.imageCompositingTimeMs = imageMs;
        m_stats.totalCompositingTimeMs = totalMs;
    }

    return image;
}

AtlasImage TextureCompositor::composeMetallicRoughness(
    const ColourMap& colourMap,
    const UVAtlas& atlas
)
{
    AtlasImage image;
    image.width = std::max(1, atlas.resolution());
    image.height = std::max(1, atlas.resolution());
    image.rgba.assign(static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height) * 4,
                      0);

    for (const auto& [faceIndex, rect] : atlas.islandsByFace()) {
        const PBR facePbr = colourMap.getFacePbr(faceIndex);
        const auto& layers = colourMap.getLayers(faceIndex);

        float metallic = facePbr.metallic;
        float roughness = facePbr.roughness;

        for (const Layer& layer : layers) {
            if (!layer.visible) {
                continue;
            }

            metallic = layer.pbr.metallic;
            roughness = layer.pbr.roughness;
        }

        const std::uint8_t m = static_cast<std::uint8_t>(
            std::clamp(metallic, 0.0F, 1.0F) * 255.0F
        );
        const std::uint8_t r = static_cast<std::uint8_t>(
            std::clamp(roughness, 0.0F, 1.0F) * 255.0F
        );

        for (int y = rect.y; y < (rect.y + rect.height); ++y) {
            for (int x = rect.x; x < (rect.x + rect.width); ++x) {
                if (x < 0 || y < 0 || x >= image.width || y >= image.height) {
                    continue;
                }
                const std::size_t i = pixelIndex(image, x, y);
                image.rgba[i + 0] = m;
                image.rgba[i + 1] = r;
                image.rgba[i + 2] = 0;
                image.rgba[i + 3] = 255;
            }
        }
    }

    return image;
}

bool TextureCompositor::writePpm(const AtlasImage& image, const std::string& outputPath) const
{
    if (image.width <= 0 || image.height <= 0 || image.rgba.empty()) {
        return false;
    }

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }

    out << "P6\n" << image.width << " " << image.height << "\n255\n";
    for (int y = 0; y < image.height; ++y) {
        for (int x = 0; x < image.width; ++x) {
            const std::size_t i = pixelIndex(image, x, y);
            out.put(static_cast<char>(image.rgba[i + 0]));
            out.put(static_cast<char>(image.rgba[i + 1]));
            out.put(static_cast<char>(image.rgba[i + 2]));
        }
    }

    return out.good();
}

bool TextureCompositor::writePng(const AtlasImage& image, const std::string& outputPath) const
{
    std::vector<std::uint8_t> pngBytes;
    if (!encodePng(image, pngBytes)) {
        return false;
    }

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }

    out.write(reinterpret_cast<const char*>(pngBytes.data()), static_cast<std::streamsize>(pngBytes.size()));
    return out.good();
}

bool TextureCompositor::encodePng(const AtlasImage& image, std::vector<std::uint8_t>& pngBytes) const
{
    if (image.width <= 0 || image.height <= 0) {
        return false;
    }

    const std::size_t expectedSize = static_cast<std::size_t>(image.width)
        * static_cast<std::size_t>(image.height) * 4U;
    if (image.rgba.size() != expectedSize) {
        return false;
    }

    pngBytes.clear();
    const std::uint8_t signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
    pngBytes.insert(pngBytes.end(), std::begin(signature), std::end(signature));

    std::vector<std::uint8_t> ihdr;
    ihdr.reserve(13);
    appendBigEndianU32(ihdr, static_cast<std::uint32_t>(image.width));
    appendBigEndianU32(ihdr, static_cast<std::uint32_t>(image.height));
    ihdr.push_back(8);   // bit depth
    ihdr.push_back(6);   // RGBA
    ihdr.push_back(0);   // compression
    ihdr.push_back(0);   // filter
    ihdr.push_back(0);   // interlace
    writeChunk(pngBytes, "IHDR", ihdr);

    std::vector<std::uint8_t> scanlines;
    scanlines.reserve(static_cast<std::size_t>(image.height)
                      * (static_cast<std::size_t>(image.width) * 4U + 1U));
    for (int y = 0; y < image.height; ++y) {
        scanlines.push_back(0);  // filter type 0
        const std::size_t rowStart = static_cast<std::size_t>(y) * static_cast<std::size_t>(image.width) * 4U;
        scanlines.insert(
            scanlines.end(),
            image.rgba.begin() + static_cast<std::vector<std::uint8_t>::difference_type>(rowStart),
            image.rgba.begin()
                + static_cast<std::vector<std::uint8_t>::difference_type>(rowStart
                                                                           + static_cast<std::size_t>(image.width)
                                                                               * 4U)
        );
    }

    // zlib stream using uncompressed deflate blocks.
    std::vector<std::uint8_t> zlibData;
    zlibData.reserve(scanlines.size() + (scanlines.size() / 65535U + 1U) * 5U + 8U);
    zlibData.push_back(0x78);
    zlibData.push_back(0x01);

    std::size_t offset = 0;
    while (offset < scanlines.size()) {
        const std::size_t remaining = scanlines.size() - offset;
        const std::size_t blockSize = std::min<std::size_t>(65535U, remaining);
        const bool isFinal = (offset + blockSize) == scanlines.size();

        zlibData.push_back(isFinal ? 1U : 0U);
        const std::uint16_t len = static_cast<std::uint16_t>(blockSize);
        const std::uint16_t nlen = static_cast<std::uint16_t>(~len);
        zlibData.push_back(static_cast<std::uint8_t>(len & 0xFFU));
        zlibData.push_back(static_cast<std::uint8_t>((len >> 8U) & 0xFFU));
        zlibData.push_back(static_cast<std::uint8_t>(nlen & 0xFFU));
        zlibData.push_back(static_cast<std::uint8_t>((nlen >> 8U) & 0xFFU));

        zlibData.insert(
            zlibData.end(),
            scanlines.begin() + static_cast<std::vector<std::uint8_t>::difference_type>(offset),
            scanlines.begin()
                + static_cast<std::vector<std::uint8_t>::difference_type>(offset + blockSize)
        );

        offset += blockSize;
    }

    appendBigEndianU32(zlibData, adler32(scanlines.data(), scanlines.size()));
    writeChunk(pngBytes, "IDAT", zlibData);

    std::vector<std::uint8_t> iend;
    writeChunk(pngBytes, "IEND", iend);
    return true;
}

std::size_t TextureCompositor::pixelIndex(const AtlasImage& image, int x, int y)
{
    return (static_cast<std::size_t>(y) * static_cast<std::size_t>(image.width)
            + static_cast<std::size_t>(x))
        * 4U;
}

void TextureCompositor::blendPixel(
    AtlasImage& image,
    int x,
    int y,
    const RGBA& colour,
    BlendMode mode
)
{
    if (x < 0 || y < 0 || x >= image.width || y >= image.height) {
        return;
    }

    const std::size_t i = pixelIndex(image, x, y);
    auto ch = [&](int off) -> std::uint8_t& { return image.rgba[i + static_cast<std::size_t>(off)]; };

    const float srcA = static_cast<float>(colour.a) / 255.0F;
    const auto blendChan = [&](std::uint8_t dst, std::uint8_t src) -> std::uint8_t {
        float d = static_cast<float>(dst) / 255.0F;
        float s = static_cast<float>(src) / 255.0F;
        float blended = s;
        switch (mode) {
            case BlendMode::Multiply:
                blended = d * s;
                break;
            case BlendMode::Screen:
                blended = 1.0F - ((1.0F - d) * (1.0F - s));
                break;
            case BlendMode::Normal:
            default:
                blended = s;
                break;
        }
        float out = (srcA * blended) + ((1.0F - srcA) * d);
        return static_cast<std::uint8_t>(std::clamp(out, 0.0F, 1.0F) * 255.0F);
    };

    ch(0) = blendChan(ch(0), colour.r);
    ch(1) = blendChan(ch(1), colour.g);
    ch(2) = blendChan(ch(2), colour.b);
    ch(3) = 255;
}

std::uint32_t TextureCompositor::crc32(const std::uint8_t* data, std::size_t size)
{
    std::uint32_t crc = 0xFFFFFFFFU;
    for (std::size_t i = 0; i < size; ++i) {
        crc ^= static_cast<std::uint32_t>(data[i]);
        for (int bit = 0; bit < 8; ++bit) {
            const std::uint32_t mask = static_cast<std::uint32_t>(-(static_cast<int>(crc & 1U)));
            crc = (crc >> 1U) ^ (0xEDB88320U & mask);
        }
    }
    return ~crc;
}

std::uint32_t TextureCompositor::adler32(const std::uint8_t* data, std::size_t size)
{
    constexpr std::uint32_t mod = 65521U;
    std::uint32_t s1 = 1U;
    std::uint32_t s2 = 0U;

    for (std::size_t i = 0; i < size; ++i) {
        s1 = (s1 + data[i]) % mod;
        s2 = (s2 + s1) % mod;
    }

    return (s2 << 16U) | s1;
}

void TextureCompositor::writeChunk(
    std::vector<std::uint8_t>& out,
    const char type[4],
    const std::vector<std::uint8_t>& payload
)
{
    appendBigEndianU32(out, static_cast<std::uint32_t>(payload.size()));

    const std::size_t typeOffset = out.size();
    out.push_back(static_cast<std::uint8_t>(type[0]));
    out.push_back(static_cast<std::uint8_t>(type[1]));
    out.push_back(static_cast<std::uint8_t>(type[2]));
    out.push_back(static_cast<std::uint8_t>(type[3]));
    out.insert(out.end(), payload.begin(), payload.end());

    const std::uint32_t chunkCrc = crc32(
        out.data() + static_cast<std::ptrdiff_t>(typeOffset),
        4U + payload.size()
    );
    appendBigEndianU32(out, chunkCrc);
}

void TextureCompositor::appendBigEndianU32(std::vector<std::uint8_t>& out, std::uint32_t value)
{
    out.push_back(static_cast<std::uint8_t>((value >> 24U) & 0xFFU));
    out.push_back(static_cast<std::uint8_t>((value >> 16U) & 0xFFU));
    out.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xFFU));
    out.push_back(static_cast<std::uint8_t>(value & 0xFFU));
}

}  // namespace Colour