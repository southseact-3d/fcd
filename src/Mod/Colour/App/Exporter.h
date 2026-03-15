// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_APP_EXPORTER_H
#define COLOUR_APP_EXPORTER_H

#include <string>
#include <vector>
#include <unordered_map>

#include "TextureCompositor.h"
#include "UVAtlas.h"

namespace Colour
{

class Exporter
{
public:
    struct ExportOptions
    {
        bool forceOpaqueAlpha {false};
        bool srgbTextures {true};
        bool jlcWjpPreset {false};
        bool validateWallThickness {true};
    };

    struct ValidationResult
    {
        bool valid {true};
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
        double minWallThickness {0.0};
    };

    ValidationResult validateForJlc3dp(
        const std::vector<Vertex>& vertices,
        const std::vector<Triangle>& triangles,
        const UVAtlas& atlas
    ) const;

    bool exportObjMtl(
        const std::string& outputStem,
        const std::vector<Vertex>& vertices,
        const std::vector<Triangle>& triangles,
        const UVAtlas& atlas,
        const AtlasImage& atlasImage
    ) const;

    bool exportGlb(
        const std::string& outputPath,
        const std::vector<Vertex>& vertices,
        const std::vector<Triangle>& triangles,
        const UVAtlas& atlas,
        const AtlasImage& baseColourAtlas,
        const AtlasImage& metallicRoughnessAtlas,
        const ExportOptions& options = {}
    ) const;

    bool export3mf(
        const std::string& outputPath,
        const std::vector<Vertex>& vertices,
        const std::vector<Triangle>& triangles,
        const UVAtlas& atlas,
        const AtlasImage& baseColourAtlas,
        const ExportOptions& options = {}
    ) const;
};

}  // namespace Colour

#endif  // COLOUR_APP_EXPORTER_H
