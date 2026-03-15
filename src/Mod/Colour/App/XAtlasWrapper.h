// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_APP_XATLASWRAPPER_H
#define COLOUR_APP_XATLASWRAPPER_H

#include "UVAtlas.h"

namespace Colour
{

class XAtlasWrapper
{
public:
    XAtlasWrapper();
    ~XAtlasWrapper();

    bool generateAtlas(
        const std::vector<Vertex>& vertices,
        const std::vector<Triangle>& triangles,
        UVAtlas::GenerateOptions options,
        std::vector<TriangleUv>& outTriangleUvs,
        std::unordered_map<int, IslandRect>& outIslandsByFace,
        int& outResolution
    );

    bool isAvailable() const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace Colour

#endif // COLOUR_APP_XATLASWRAPPER_H
