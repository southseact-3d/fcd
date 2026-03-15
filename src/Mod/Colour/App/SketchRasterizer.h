// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_APP_SKETCHRASTERIZER_H
#define COLOUR_APP_SKETCHRASTERIZER_H

#include <cstdint>
#include <vector>

#include "ColourMap.h"
#include "TextureCompositor.h"
#include "UVAtlas.h"

namespace Colour
{

class SketchRasterizer
{
public:
    struct RasterizationOptions
    {
        int strokeWidth {2};
        bool antialiasing {true};
    };

    SketchRasterizer() = default;
    explicit SketchRasterizer(const RasterizationOptions& opts);

    void setOptions(const RasterizationOptions& opts);
    const RasterizationOptions& getOptions() const;

    ImageData rasterize(
        const SketchData& sketch,
        const IslandRect& island,
        int atlasResolution,
        const RGBA& colour
    ) const;

    ImageData rasterizeToAtlas(
        const SketchData& sketch,
        AtlasImage& atlas,
        const IslandRect& island,
        const RGBA& colour,
        BlendMode mode
    ) const;

private:
    RasterizationOptions m_options;

    void drawLine(ImageData& img, int x0, int y0, int x1, int y1, const RGBA& colour) const;
    void drawLineAA(ImageData& img, int x0, int y0, int x1, int y1, const RGBA& colour) const;
    void drawCircle(ImageData& img, int cx, int cy, int radius, const RGBA& colour) const;
    void drawArc(ImageData& img, int cx, int cy, int radius, float startAngle, float endAngle, const RGBA& colour) const;

    SketchPoint projectToUV(
        const SketchPoint& point3d,
        const Vec3& normal,
        const IslandRect& island,
        int resolution
    ) const;

    void setPixel(ImageData& img, int x, int y, const RGBA& colour) const;
    RGBA getPixel(const ImageData& img, int x, int y) const;
};

}  // namespace Colour

#endif  // COLOUR_APP_SKETCHRASTERIZER_H
