// SPDX-License-Identifier: LGPL-2.1-or-later

#include "ColourMap.h"

#include <algorithm>
#include <map>
#include <sstream>
#include <stdexcept>

namespace Colour
{

void ColourMap::setFaceColour(int faceIndex, const RGBA& colour)
{
    m_faceColours[faceIndex] = colour;
}

RGBA ColourMap::getFaceColour(int faceIndex) const
{
    auto it = m_faceColours.find(faceIndex);
    if (it != m_faceColours.end()) {
        return it->second;
    }
    return {};
}

void ColourMap::setFacePbr(int faceIndex, const PBR& pbr)
{
    m_facePbr[faceIndex] = pbr;
}

PBR ColourMap::getFacePbr(int faceIndex) const
{
    auto it = m_facePbr.find(faceIndex);
    if (it != m_facePbr.end()) {
        return it->second;
    }
    return {};
}

std::size_t ColourMap::addLayer(int faceIndex, const Layer& layer)
{
    auto& layers = m_faceLayers[faceIndex];
    layers.push_back(layer);
    std::stable_sort(layers.begin(), layers.end(), [](const Layer& a, const Layer& b) {
        return a.zOrder < b.zOrder;
    });
    return layers.size() - 1;
}

bool ColourMap::moveLayer(int faceIndex, std::size_t fromIndex, std::size_t toIndex)
{
    auto it = m_faceLayers.find(faceIndex);
    if (it == m_faceLayers.end() || fromIndex >= it->second.size() || toIndex >= it->second.size()) {
        return false;
    }

    auto layer = it->second[fromIndex];
    it->second.erase(it->second.begin() + static_cast<long long>(fromIndex));
    it->second.insert(it->second.begin() + static_cast<long long>(toIndex), layer);

    for (std::size_t i = 0; i < it->second.size(); ++i) {
        it->second[i].zOrder = static_cast<int>(i);
    }

    return true;
}

bool ColourMap::removeLayer(int faceIndex, std::size_t index)
{
    auto it = m_faceLayers.find(faceIndex);
    if (it == m_faceLayers.end() || index >= it->second.size()) {
        return false;
    }

    it->second.erase(it->second.begin() + static_cast<long long>(index));

    for (std::size_t i = 0; i < it->second.size(); ++i) {
        it->second[i].zOrder = static_cast<int>(i);
    }

    return true;
}

void ColourMap::remapFaceIndices(const std::unordered_map<int, int>& oldToNewFaceIndex)
{
    std::unordered_map<int, RGBA> remappedColours;
    std::unordered_map<int, PBR> remappedPbr;
    std::unordered_map<int, std::vector<Layer>> remappedLayers;

    for (const auto& [oldIndex, colour] : m_faceColours) {
        auto it = oldToNewFaceIndex.find(oldIndex);
        if (it != oldToNewFaceIndex.end()) {
            remappedColours[it->second] = colour;
        }
    }

    for (const auto& [oldIndex, pbr] : m_facePbr) {
        auto it = oldToNewFaceIndex.find(oldIndex);
        if (it != oldToNewFaceIndex.end()) {
            remappedPbr[it->second] = pbr;
        }
    }

    for (const auto& [oldIndex, layers] : m_faceLayers) {
        auto it = oldToNewFaceIndex.find(oldIndex);
        if (it != oldToNewFaceIndex.end()) {
            remappedLayers[it->second] = layers;
        }
    }

    m_faceColours = std::move(remappedColours);
    m_facePbr = std::move(remappedPbr);
    m_faceLayers = std::move(remappedLayers);
}

void ColourMap::registerImage(const std::string& imageId, const ImageData& imageData)
{
    if (imageId.empty()) {
        return;
    }
    m_imageData[imageId] = imageData;
}

void ColourMap::registerImagePath(const std::string& imageId, const std::string& storagePath)
{
    if (imageId.empty()) {
        return;
    }
    m_imagePaths[imageId] = storagePath;
}

const ImageData* ColourMap::getImage(const std::string& imageId) const
{
    auto it = m_imageData.find(imageId);
    if (it != m_imageData.end()) {
        return &it->second;
    }
    return nullptr;
}

std::string ColourMap::getImageStoragePath(const std::string& imageId) const
{
    auto it = m_imagePaths.find(imageId);
    if (it != m_imagePaths.end()) {
        return it->second;
    }
    auto dataIt = m_imageData.find(imageId);
    if (dataIt != m_imageData.end() && dataIt->second.valid()) {
        return {};
    }
    return {};
}

const std::unordered_map<std::string, std::string>& ColourMap::imageRegistry() const
{
    return m_imagePaths;
}

void ColourMap::registerSketch(const std::string& sketchId, const SketchData& sketchData)
{
    if (sketchId.empty()) {
        return;
    }
    m_sketches[sketchId] = sketchData;
}

const SketchData* ColourMap::getSketch(const std::string& sketchId) const
{
    auto it = m_sketches.find(sketchId);
    if (it != m_sketches.end()) {
        return &it->second;
    }
    return nullptr;
}

void ColourMap::removeSketch(const std::string& sketchId)
{
    m_sketches.erase(sketchId);
}
 
const std::vector<Layer>& ColourMap::getLayers(int faceIndex) const
{
    static const std::vector<Layer> kEmpty;
    auto it = m_faceLayers.find(faceIndex);
    return it != m_faceLayers.end() ? it->second : kEmpty;
}

void ColourMap::setLayers(int faceIndex, const std::vector<Layer>& layers)
{
    m_faceLayers[faceIndex] = layers;
}

const std::unordered_map<int, RGBA>& ColourMap::faceColours() const
{
    return m_faceColours;
}

std::string ColourMap::serializeXml() const
{
    std::ostringstream out;
    out << "<ColourMap>\n";

    std::map<int, RGBA> sortedColours(m_faceColours.begin(), m_faceColours.end());
    for (const auto& [faceIndex, colour] : sortedColours) {
        out << "  <Face index=\"" << faceIndex << "\" r=\"" << static_cast<int>(colour.r)
            << "\" g=\"" << static_cast<int>(colour.g) << "\" b=\""
            << static_cast<int>(colour.b) << "\" a=\"" << static_cast<int>(colour.a)
            << "\"/>\n";
    }

    std::map<int, PBR> sortedPbr(m_facePbr.begin(), m_facePbr.end());
    for (const auto& [faceIndex, pbr] : sortedPbr) {
        out << "  <Pbr index=\"" << faceIndex << "\" metallic=\"" << pbr.metallic
            << "\" roughness=\"" << pbr.roughness << "\" alpha=\"" << pbr.alpha
            << "\"/>\n";
    }

    std::map<int, std::vector<Layer>> sortedLayers(m_faceLayers.begin(), m_faceLayers.end());
    for (const auto& [faceIndex, layers] : sortedLayers) {
        for (const auto& layer : layers) {
            out << "  <Layer index=\"" << faceIndex << "\" name=\"" << escapeXml(layer.name)
                << "\" z=\"" << layer.zOrder << "\" mode=\""
                << blendModeToString(layer.blendMode) << "\" visible=\""
                << (layer.visible ? 1 : 0) << "\" locked=\"" << (layer.locked ? 1 : 0)
                << "\" image=\"" << escapeXml(layer.imageRef) << "\" sketch=\""
                << escapeXml(layer.sketchRef) << "\" isImage=\""
                << (layer.isImageLayer ? 1 : 0) << "\" isSketch=\""
                << (layer.isSketchLayer ? 1 : 0) << "\" r=\"" << static_cast<int>(layer.colour.r)
                << "\" g=\"" << static_cast<int>(layer.colour.g) << "\" b=\""
                << static_cast<int>(layer.colour.b) << "\" a=\""
                << static_cast<int>(layer.colour.a) << "\"/>\n";
        }
    }

    std::map<std::string, std::string> sortedImages(m_imagePaths.begin(), m_imagePaths.end());
    for (const auto& [imageId, storagePath] : sortedImages) {
        out << "  <Image id=\"" << escapeXml(imageId) << "\" path=\"" << escapeXml(storagePath)
            << "\"/>\n";
    }

    for (const auto& [imageId, imgData] : m_imageData) {
        if (imgData.valid()) {
            out << "  <ImageData id=\"" << escapeXml(imageId) << "\" width=\""
                << imgData.width << "\" height=\"" << imgData.height << "\">";
            for (std::size_t i = 0; i < imgData.rgba.size(); i += 4) {
                out << static_cast<int>(imgData.rgba[i]) << ",";
                out << static_cast<int>(imgData.rgba[i + 1]) << ",";
                out << static_cast<int>(imgData.rgba[i + 2]) << ",";
                out << static_cast<int>(imgData.rgba[i + 3]) << ";";
            }
            out << "</ImageData>\n";
        }
    }

    for (const auto& [sketchId, sketchData] : m_sketches) {
        out << "  <Sketch id=\"" << escapeXml(sketchId) << "\" obj=\""
            << escapeXml(sketchData.referencedObject) << "\" face=\""
            << sketchData.faceIndex << "\">\n";
        for (const auto& geom : sketchData.geometries) {
            out << "    <Geom type=\"";
            switch (geom.type) {
                case SketchElementType::Line: out << "line"; break;
                case SketchElementType::Arc: out << "arc"; break;
                case SketchElementType::Circle: out << "circle"; break;
                case SketchElementType::Ellipse: out << "ellipse"; break;
                case SketchElementType::Bezier: out << "bezier"; break;
                case SketchElementType::Point: out << "point"; break;
            }
            out << "\" const=\"" << (geom.isConstruction ? 1 : 0) << "\">";
            for (const auto& pt : geom.points) {
                out << pt.x << "," << pt.y << ";";
            }
            out << "</Geom>\n";
        }
        out << "  </Sketch>\n";
    }
 
    out << "</ColourMap>\n";
    return out.str();
}

static bool parseIntAttr(const std::string& line, const std::string& key, int& out)
{
    const std::string token = key + "=\"";
    auto pos = line.find(token);
    if (pos == std::string::npos) {
        return false;
    }
    pos += token.size();
    auto end = line.find('"', pos);
    if (end == std::string::npos) {
        return false;
    }
    try {
        out = std::stoi(line.substr(pos, end - pos));
    }
    catch (const std::exception&) {
        return false;
    }
    return true;
}

static bool parseFloatAttr(const std::string& line, const std::string& key, float& out)
{
    const std::string token = key + "=\"";
    auto pos = line.find(token);
    if (pos == std::string::npos) {
        return false;
    }
    pos += token.size();
    auto end = line.find('"', pos);
    if (end == std::string::npos) {
        return false;
    }
    try {
        out = std::stof(line.substr(pos, end - pos));
    }
    catch (const std::exception&) {
        return false;
    }
    return true;
}

static bool parseStringAttr(const std::string& line, const std::string& key, std::string& out)
{
    const std::string token = key + "=\"";
    auto pos = line.find(token);
    if (pos == std::string::npos) {
        return false;
    }
    pos += token.size();
    auto end = line.find('"', pos);
    if (end == std::string::npos) {
        return false;
    }
    out = line.substr(pos, end - pos);
    return true;
}

bool ColourMap::deserializeXml(const std::string& xmlText)
{
    m_faceColours.clear();
    m_facePbr.clear();
    m_faceLayers.clear();
    m_imagePaths.clear();
    m_imageData.clear();
    m_sketches.clear();
 
    std::istringstream in(xmlText);
    std::string line;
    bool sawRoot = false;
    std::string currentSketchId;
    SketchData* currentSketch = nullptr;
    
    while (std::getline(in, line)) {
        if (line.find("<ColourMap") != std::string::npos) {
            sawRoot = true;
        }

        if (line.find("<Sketch ") != std::string::npos) {
            std::string sketchId, obj;
            int faceIdx = 0;
            if (parseStringAttr(line, "id", sketchId) && parseStringAttr(line, "obj", obj)) {
                parseIntAttr(line, "face", faceIdx);
                currentSketchId = unescapeXml(sketchId);
                m_sketches[currentSketchId] = SketchData();
                currentSketch = &m_sketches[currentSketchId];
                currentSketch->sketchId = currentSketchId;
                currentSketch->referencedObject = unescapeXml(obj);
                currentSketch->faceIndex = faceIdx;
            }
        }
        else if (line.find("</Sketch>") != std::string::npos) {
            currentSketch = nullptr;
            currentSketchId.clear();
        }
        else if (line.find("<Geom ") != std::string::npos && currentSketch != nullptr) {
            std::string type;
            int isConst = 0;
            if (parseStringAttr(line, "type", type) && parseIntAttr(line, "const", isConst)) {
                SketchGeometry geom;
                geom.isConstruction = (isConst != 0);
                
                if (type == "line") geom.type = SketchElementType::Line;
                else if (type == "arc") geom.type = SketchElementType::Arc;
                else if (type == "circle") geom.type = SketchElementType::Circle;
                else if (type == "ellipse") geom.type = SketchElementType::Ellipse;
                else if (type == "bezier") geom.type = SketchElementType::Bezier;
                else geom.type = SketchElementType::Point;
                
                auto start = line.find('>');
                auto end = line.find("</Geom>");
                if (start != std::string::npos && end != std::string::npos) {
                    std::string dataStr = line.substr(start + 1, end - start - 1);
                    std::string coord;
                    std::istringstream coordStream(dataStr);
                    while (std::getline(coordStream, coord, ';')) {
                        if (coord.empty()) continue;
                        std::string::size_type comma = coord.find(',');
                        if (comma != std::string::npos) {
                            try {
                                SketchPoint pt;
                                pt.x = std::stof(coord.substr(0, comma));
                                pt.y = std::stof(coord.substr(comma + 1));
                                geom.points.push_back(pt);
                            } catch (...) {}
                        }
                    }
                }
                currentSketch->geometries.push_back(geom);
            }
        }

        if (line.find("<Face ") != std::string::npos) {
            int index = 0;
            int r = 255;
            int g = 255;
            int b = 255;
            int a = 255;
            if (parseIntAttr(line, "index", index) && parseIntAttr(line, "r", r)
                && parseIntAttr(line, "g", g) && parseIntAttr(line, "b", b)
                && parseIntAttr(line, "a", a)) {
                m_faceColours[index] = RGBA {static_cast<std::uint8_t>(r),
                                             static_cast<std::uint8_t>(g),
                                             static_cast<std::uint8_t>(b),
                                             static_cast<std::uint8_t>(a)};
            }
        }
        else if (line.find("<Pbr ") != std::string::npos) {
            int index = 0;
            float metallic = 0.0F;
            float roughness = 1.0F;
            float alpha = 1.0F;
            if (parseIntAttr(line, "index", index) && parseFloatAttr(line, "metallic", metallic)
                && parseFloatAttr(line, "roughness", roughness)
                && parseFloatAttr(line, "alpha", alpha)) {
                m_facePbr[index] = PBR {metallic, roughness, alpha};
            }
        }
        else if (line.find("<Layer ") != std::string::npos) {
            int index = 0;
            int z = 0;
            int visible = 1;
            int locked = 0;
            int isImage = 0;
            int isSketch = 0;
            int r = 255;
            int g = 255;
            int b = 255;
            int a = 255;
            std::string mode;
            std::string name;
            std::string image;
            std::string sketch;
            if (parseIntAttr(line, "index", index) && parseIntAttr(line, "z", z)
                && parseIntAttr(line, "visible", visible) && parseIntAttr(line, "locked", locked)
                && parseIntAttr(line, "isImage", isImage) && parseIntAttr(line, "isSketch", isSketch)
                && parseIntAttr(line, "r", r)
                && parseIntAttr(line, "g", g) && parseIntAttr(line, "b", b)
                && parseIntAttr(line, "a", a) && parseStringAttr(line, "mode", mode)
                && parseStringAttr(line, "name", name) && parseStringAttr(line, "image", image)
                && parseStringAttr(line, "sketch", sketch)) {
                Layer layer;
                layer.name = unescapeXml(name);
                layer.zOrder = z;
                layer.visible = (visible != 0);
                layer.locked = (locked != 0);
                layer.isImageLayer = (isImage != 0);
                layer.isSketchLayer = (isSketch != 0);
                layer.imageRef = unescapeXml(image);
                layer.sketchRef = unescapeXml(sketch);
                layer.blendMode = blendModeFromString(mode);
                layer.colour = RGBA {static_cast<std::uint8_t>(r),
                                     static_cast<std::uint8_t>(g),
                                     static_cast<std::uint8_t>(b),
                                     static_cast<std::uint8_t>(a)};
                m_faceLayers[index].push_back(layer);
            }
        }
        else if (line.find("<Image ") != std::string::npos) {
            std::string imageId;
            std::string storagePath;
            if (parseStringAttr(line, "id", imageId) && parseStringAttr(line, "path", storagePath)) {
                m_imagePaths[unescapeXml(imageId)] = unescapeXml(storagePath);
            }
        }
        else if (line.find("<ImageData") != std::string::npos) {
            std::string imageId;
            int width = 0;
            int height = 0;
            if (parseStringAttr(line, "id", imageId) && parseIntAttr(line, "width", width)
                && parseIntAttr(line, "height", height)) {
                auto start = line.find('>');
                auto end = line.find("</ImageData>");
                if (start != std::string::npos && end != std::string::npos && width > 0 && height > 0) {
                    std::string dataStr = line.substr(start + 1, end - start - 1);
                    ImageData imgData;
                    imgData.width = width;
                    imgData.height = height;
                    imgData.rgba.reserve(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4);
                    
                    std::string pixel;
                    std::istringstream pixelStream(dataStr);
                    while (std::getline(pixelStream, pixel, ';')) {
                        if (pixel.empty()) continue;
                        std::string channel;
                        std::istringstream channelStream(pixel);
                        int idx = 0;
                        std::uint8_t rgba[4] = {0, 0, 0, 255};
                        while (std::getline(channelStream, channel, ',') && idx < 4) {
                            try {
                                rgba[idx] = static_cast<std::uint8_t>(std::stoi(channel));
                            } catch (...) {}
                            idx++;
                        }
                        imgData.rgba.push_back(rgba[0]);
                        imgData.rgba.push_back(rgba[1]);
                        imgData.rgba.push_back(rgba[2]);
                        imgData.rgba.push_back(rgba[3]);
                    }
                    
                    if (imgData.valid()) {
                        m_imageData[unescapeXml(imageId)] = imgData;
                    }
                }
            }
        }
    }

    for (auto& [faceIndex, layers] : m_faceLayers) {
        (void)faceIndex;
        std::stable_sort(layers.begin(), layers.end(), [](const Layer& a, const Layer& b) {
            return a.zOrder < b.zOrder;
        });
    }

    return sawRoot;
}

std::string ColourMap::blendModeToString(BlendMode mode)
{
    switch (mode) {
        case BlendMode::Multiply:
            return "multiply";
        case BlendMode::Screen:
            return "screen";
        case BlendMode::Normal:
        default:
            return "normal";
    }
}

BlendMode ColourMap::blendModeFromString(const std::string& text)
{
    if (text == "multiply") {
        return BlendMode::Multiply;
    }
    if (text == "screen") {
        return BlendMode::Screen;
    }
    return BlendMode::Normal;
}

std::string ColourMap::escapeXml(const std::string& text)
{
    std::string escaped;
    escaped.reserve(text.size());
    for (char ch : text) {
        switch (ch) {
            case '&':
                escaped += "&amp;";
                break;
            case '<':
                escaped += "&lt;";
                break;
            case '>':
                escaped += "&gt;";
                break;
            case '\"':
                escaped += "&quot;";
                break;
            case '\'':
                escaped += "&apos;";
                break;
            default:
                escaped.push_back(ch);
                break;
        }
    }
    return escaped;
}

std::string ColourMap::unescapeXml(const std::string& text)
{
    std::string value = text;
    const std::pair<const char*, const char*> replacements[] = {
        {"&lt;", "<"},
        {"&gt;", ">"},
        {"&quot;", "\""},
        {"&apos;", "'"},
        {"&amp;", "&"}
    };

    for (const auto& [from, to] : replacements) {
        std::size_t pos = 0;
        while ((pos = value.find(from, pos)) != std::string::npos) {
            value.replace(pos, std::char_traits<char>::length(from), to);
            pos += std::char_traits<char>::length(to);
        }
    }

    return value;
}

}  // namespace Colour
