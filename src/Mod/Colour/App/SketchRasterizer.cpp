// SPDX-License-Identifier: LGPL-2.1-or-later

#include "SketchRasterizer.h"

#include <algorithm>
#include <cmath>

namespace Colour
{

SketchRasterizer::SketchRasterizer(const RasterizationOptions& opts)
    : m_options(opts)
{}

void SketchRasterizer::setOptions(const RasterizationOptions& opts)
{
    m_options = opts;
}

const SketchRasterizer::RasterizationOptions& SketchRasterizer::getOptions() const
{
    return m_options;
}

ImageData SketchRasterizer::rasterize(
    const SketchData& sketch,
    const IslandRect& island,
    int atlasResolution,
    const RGBA& colour
) const
{
    ImageData img;
    img.width = island.width;
    img.height = island.height;
    img.rgba.assign(static_cast<std::size_t>(img.width) * static_cast<std::size_t>(img.height) * 4, 0);

    Vec3 normal = {0.0F, 0.0F, 1.0F};

    for (const auto& geom : sketch.geometries) {
        if (geom.isConstruction) {
            continue;
        }

        switch (geom.type) {
            case SketchElementType::Line: {
                if (geom.points.size() >= 2) {
                    auto uv0 = projectToUV(geom.points[0], normal, island, atlasResolution);
                    auto uv1 = projectToUV(geom.points[1], normal, island, atlasResolution);
                    int x0 = static_cast<int>(uv0.x);
                    int y0 = static_cast<int>(uv0.y);
                    int x1 = static_cast<int>(uv1.x);
                    int y1 = static_cast<int>(uv1.y);
                    if (m_options.antialiasing) {
                        drawLineAA(img, x0, y0, x1, y1, colour);
                    } else {
                        drawLine(img, x0, y0, x1, y1, colour);
                    }
                }
                break;
            }
            case SketchElementType::Circle: {
                if (geom.points.size() >= 2) {
                    auto center = projectToUV(geom.points[0], normal, island, atlasResolution);
                    auto edge = projectToUV(geom.points[1], normal, island, atlasResolution);
                    float dx = edge.x - center.x;
                    float dy = edge.y - center.y;
                    int radius = static_cast<int>(std::sqrt(dx * dx + dy * dy));
                    int cx = static_cast<int>(center.x);
                    int cy = static_cast<int>(center.y);
                    drawCircle(img, cx, cy, radius, colour);
                }
                break;
            }
            case SketchElementType::Arc: {
                if (geom.points.size() >= 3) {
                    auto center = projectToUV(geom.points[0], normal, island, atlasResolution);
                    auto start = projectToUV(geom.points[1], normal, island, atlasResolution);
                    auto end = projectToUV(geom.points[2], normal, island, atlasResolution);
                    float dx = start.x - center.x;
                    float dy = start.y - center.y;
                    int radius = static_cast<int>(std::sqrt(dx * dx + dy * dy));
                    float startAngle = std::atan2(dy, dx);
                    float endAngle = std::atan2(end.y - center.y, end.x - center.x);
                    int cx = static_cast<int>(center.x);
                    int cy = static_cast<int>(center.y);
                    drawArc(img, cx, cy, radius, startAngle, endAngle, colour);
                }
                break;
            }
            case SketchElementType::Ellipse:
            case SketchElementType::Bezier:
            case SketchElementType::Point:
            default:
                break;
        }
    }

    return img;
}

ImageData SketchRasterizer::rasterizeToAtlas(
    const SketchData& sketch,
    AtlasImage& atlas,
    const IslandRect& island,
    const RGBA& colour,
    BlendMode mode
) const
{
    ImageData sketchImg = rasterize(sketch, island, atlas.width, colour);

    for (int y = 0; y < island.height; ++y) {
        for (int x = 0; x < island.width; ++x) {
            int atlasX = island.x + x;
            int atlasY = island.y + y;

            if (atlasX < 0 || atlasY < 0 || atlasX >= atlas.width || atlasY >= atlas.height) {
                continue;
            }

            const std::size_t sketchIdx = static_cast<std::size_t>(y * sketchImg.width + x) * 4;
            if (sketchIdx + 3 >= sketchImg.rgba.size()) {
                continue;
            }

            RGBA sketchCol;
            sketchCol.r = sketchImg.rgba[sketchIdx + 0];
            sketchCol.g = sketchImg.rgba[sketchIdx + 1];
            sketchCol.b = sketchImg.rgba[sketchIdx + 2];
            sketchCol.a = sketchImg.rgba[sketchIdx + 3];

            if (sketchCol.a == 0) {
                continue;
            }

            const std::size_t atlasIdx = static_cast<std::size_t>(atlasY * atlas.width + atlasX) * 4;
            RGBA baseCol;
            baseCol.r = atlas.rgba[atlasIdx + 0];
            baseCol.g = atlas.rgba[atlasIdx + 1];
            baseCol.b = atlas.rgba[atlasIdx + 2];
            baseCol.a = atlas.rgba[atlasIdx + 3];

            float srcA = static_cast<float>(sketchCol.a) / 255.0F;
            float outA = srcA + ((1.0F - srcA) * (static_cast<float>(baseCol.a) / 255.0F));
            if (outA > 0.0F) {
                atlas.rgba[atlasIdx + 0] = static_cast<std::uint8_t>(
                    (static_cast<float>(sketchCol.r) * srcA + static_cast<float>(baseCol.r) * (1.0F - srcA)) / outA * 255.0F
                );
                atlas.rgba[atlasIdx + 1] = static_cast<std::uint8_t>(
                    (static_cast<float>(sketchCol.g) * srcA + static_cast<float>(baseCol.g) * (1.0F - srcA)) / outA * 255.0F
                );
                atlas.rgba[atlasIdx + 2] = static_cast<std::uint8_t>(
                    (static_cast<float>(sketchCol.b) * srcA + static_cast<float>(baseCol.b) * (1.0F - srcA)) / outA * 255.0F
                );
                atlas.rgba[atlasIdx + 3] = static_cast<std::uint8_t>(outA * 255.0F);
            }
        }
    }

    return sketchImg;
}

void SketchRasterizer::drawLine(ImageData& img, int x0, int y0, int x1, int y1, const RGBA& colour) const
{
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        for (int w = -m_options.strokeWidth / 2; w <= m_options.strokeWidth / 2; ++w) {
            for (int h = -m_options.strokeWidth / 2; h <= m_options.strokeWidth / 2; ++h) {
                setPixel(img, x0 + w, y0 + h, colour);
            }
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void SketchRasterizer::drawLineAA(ImageData& img, int x0, int y0, int x1, int y1, const RGBA& colour) const
{
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    int stroke = m_options.strokeWidth;
    float intensity = 1.0F / static_cast<float>(stroke);

    while (true) {
        for (int w = -stroke / 2; w <= stroke / 2; ++w) {
            for (int h = -stroke / 2; h <= stroke / 2; ++h) {
                float dist = std::sqrt(static_cast<float>(w * w + h * h));
                if (dist <= static_cast<float>(stroke / 2)) {
                    RGBA adjusted = colour;
                    adjusted.a = static_cast<std::uint8_t>(static_cast<float>(colour.a) * intensity * (1.0F - dist / (stroke / 2.0F)));
                    setPixel(img, x0 + w, y0 + h, adjusted);
                }
            }
        }

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void SketchRasterizer::drawCircle(ImageData& img, int cx, int cy, int radius, const RGBA& colour) const
{
    int stroke = m_options.strokeWidth;

    for (int angle = 0; angle < 360; angle += 1) {
        float rad = static_cast<float>(angle) * 3.14159265F / 180.0F;
        for (int w = -stroke / 2; w <= stroke / 2; ++w) {
            int x = cx + static_cast<int>(static_cast<float>(radius) * std::cos(rad)) + w;
            int y = cy + static_cast<int>(static_cast<float>(radius) * std::sin(rad)) + w;
            setPixel(img, x, y, colour);
        }
    }
}

void SketchRasterizer::drawArc(
    ImageData& img,
    int cx,
    int cy,
    int radius,
    float startAngle,
    float endAngle,
    const RGBA& colour
) const
{
    int stroke = m_options.strokeWidth;

    if (endAngle < startAngle) {
        endAngle += 2.0F * 3.14159265F;
    }

    for (float angle = startAngle; angle <= endAngle; angle += 0.01F) {
        for (int w = -stroke / 2; w <= stroke / 2; ++w) {
            int x = cx + static_cast<int>(static_cast<float>(radius) * std::cos(angle)) + w;
            int y = cy + static_cast<int>(static_cast<float>(radius) * std::sin(angle)) + w;
            setPixel(img, x, y, colour);
        }
    }
}

SketchPoint SketchRasterizer::projectToUV(
    const SketchPoint& point3d,
    const Vec3& /* normal */,
    const IslandRect& island,
    int resolution
) const
{
    SketchPoint uv;
    uv.x = island.x + (point3d.x * static_cast<float>(island.width));
    uv.y = island.y + (point3d.y * static_cast<float>(island.height));

    uv.x = std::clamp(uv.x, 0.0F, static_cast<float>(resolution - 1));
    uv.y = std::clamp(uv.y, 0.0F, static_cast<float>(resolution - 1));

    return uv;
}

void SketchRasterizer::setPixel(ImageData& img, int x, int y, const RGBA& colour) const
{
    if (x < 0 || y < 0 || x >= img.width || y >= img.height) {
        return;
    }

    const std::size_t i = static_cast<std::size_t>(y * img.width + x) * 4;

    float srcA = static_cast<float>(colour.a) / 255.0F;
    float dstA = static_cast<float>(img.rgba[i + 3]) / 255.0F;
    float outA = srcA + dstA * (1.0F - srcA);

    if (outA > 0.0F) {
        img.rgba[i + 0] = static_cast<std::uint8_t>(
            (static_cast<float>(colour.r) * srcA + static_cast<float>(img.rgba[i + 0]) * dstA * (1.0F - srcA)) / outA * 255.0F
        );
        img.rgba[i + 1] = static_cast<std::uint8_t>(
            (static_cast<float>(colour.g) * srcA + static_cast<float>(img.rgba[i + 1]) * dstA * (1.0F - srcA)) / outA * 255.0F
        );
        img.rgba[i + 2] = static_cast<std::uint8_t>(
            (static_cast<float>(colour.b) * srcA + static_cast<float>(img.rgba[i + 2]) * dstA * (1.0F - srcA)) / outA * 255.0F
        );
        img.rgba[i + 3] = static_cast<std::uint8_t>(outA * 255.0F);
    }
}

RGBA SketchRasterizer::getPixel(const ImageData& img, int x, int y) const
{
    RGBA colour;
    if (x < 0 || y < 0 || x >= img.width || y >= img.height) {
        return colour;
    }

    const std::size_t i = static_cast<std::size_t>(y * img.width + x) * 4;
    colour.r = img.rgba[i + 0];
    colour.g = img.rgba[i + 1];
    colour.b = img.rgba[i + 2];
    colour.a = img.rgba[i + 3];
    return colour;
}

}  // namespace Colour
