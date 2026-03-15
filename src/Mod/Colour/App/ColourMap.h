// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef COLOUR_APP_COLOURMAP_H
#define COLOUR_APP_COLOURMAP_H

#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace Colour
{

struct RGBA
{
    std::uint8_t r {255};
    std::uint8_t g {255};
    std::uint8_t b {255};
    std::uint8_t a {255};
};

struct PBR
{
    float metallic {0.0F};
    float roughness {1.0F};
    float alpha {1.0F};
};

enum class BlendMode
{
    Normal,
    Multiply,
    Screen
};

struct ImageData
{
    int width {0};
    int height {0};
    std::vector<std::uint8_t> rgba;
    bool valid() const { return width > 0 && height > 0 && !rgba.empty(); }
};

enum class SketchElementType
{
    Line,
    Arc,
    Circle,
    Ellipse,
    Bezier,
    Point
};

struct SketchPoint
{
    float x {0.0F};
    float y {0.0F};
};

struct SketchGeometry
{
    SketchElementType type;
    std::vector<SketchPoint> points;
    bool isConstruction {false};
};

struct SketchData
{
    std::string sketchId;
    std::vector<SketchGeometry> geometries;
    std::string referencedObject;
    int faceIndex {0};
};

struct Layer
{
    std::string name;
    int zOrder {0};
    BlendMode blendMode {BlendMode::Normal};
    bool visible {true};
    bool locked {false};
    bool isImageLayer {false};
    bool isSketchLayer {false};
    std::string imageRef;
    std::string sketchRef;
    RGBA colour;
    PBR pbr;
};

class ColourMap
{
public:
    void setFaceColour(int faceIndex, const RGBA& colour);
    RGBA getFaceColour(int faceIndex) const;

    void setFacePbr(int faceIndex, const PBR& pbr);
    PBR getFacePbr(int faceIndex) const;

    std::size_t addLayer(int faceIndex, const Layer& layer);
    bool moveLayer(int faceIndex, std::size_t fromIndex, std::size_t toIndex);
    bool removeLayer(int faceIndex, std::size_t index);
    void remapFaceIndices(const std::unordered_map<int, int>& oldToNewFaceIndex);

    void registerImage(const std::string& imageId, const ImageData& imageData);
    void registerImagePath(const std::string& imageId, const std::string& storagePath);
    const ImageData* getImage(const std::string& imageId) const;
    std::string getImageStoragePath(const std::string& imageId) const;
    const std::unordered_map<std::string, std::string>& imageRegistry() const;

    void registerSketch(const std::string& sketchId, const SketchData& sketchData);
    const SketchData* getSketch(const std::string& sketchId) const;
    void removeSketch(const std::string& sketchId);

    const std::vector<Layer>& getLayers(int faceIndex) const;
    void setLayers(int faceIndex, const std::vector<Layer>& layers);
    const std::unordered_map<int, RGBA>& faceColours() const;

    std::string serializeXml() const;
    bool deserializeXml(const std::string& xmlText);

private:
    std::unordered_map<int, RGBA> m_faceColours;
    std::unordered_map<int, PBR> m_facePbr;
    std::unordered_map<int, std::vector<Layer>> m_faceLayers;
    std::unordered_map<std::string, ImageData> m_imageData;
    std::unordered_map<std::string, std::string> m_imagePaths;
    std::unordered_map<std::string, SketchData> m_sketches;

    static std::string blendModeToString(BlendMode mode);
    static BlendMode blendModeFromString(const std::string& text);
    static std::string escapeXml(const std::string& text);
    static std::string unescapeXml(const std::string& text);
};

}  // namespace Colour

#endif  // COLOUR_APP_COLOURMAP_H
