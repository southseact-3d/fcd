// SPDX-License-Identifier: LGPL-2.1-or-later

#include "Exporter.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <string_view>
#include <vector>

#include <Base/Tools.h>
#include <zipios++/zipoutputstream.h>

#include "TextureCompositor.h"

namespace Colour
{

namespace
{

void appendU32Le(std::vector<std::uint8_t>& out, std::uint32_t value)
{
    out.push_back(static_cast<std::uint8_t>(value & 0xFFU));
    out.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xFFU));
    out.push_back(static_cast<std::uint8_t>((value >> 16U) & 0xFFU));
    out.push_back(static_cast<std::uint8_t>((value >> 24U) & 0xFFU));
}

void appendU16Le(std::vector<std::uint8_t>& out, std::uint16_t value)
{
    out.push_back(static_cast<std::uint8_t>(value & 0xFFU));
    out.push_back(static_cast<std::uint8_t>((value >> 8U) & 0xFFU));
}

void appendF32Le(std::vector<std::uint8_t>& out, float value)
{
    static_assert(sizeof(float) == sizeof(std::uint32_t), "float size is not 32-bit");
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(float));
    appendU32Le(out, bits);
}

void padTo4(std::vector<std::uint8_t>& out, std::uint8_t fill = 0)
{
    while ((out.size() % 4U) != 0U) {
        out.push_back(fill);
    }
}

void forceOpaque(AtlasImage& image)
{
    for (std::size_t i = 3; i < image.rgba.size(); i += 4U) {
        image.rgba[i] = 255;
    }
}

std::string stemName(const std::string& outputStem)
{
    const std::size_t pos = outputStem.find_last_of("/\\");
    if (pos == std::string::npos) {
        return outputStem;
    }
    return outputStem.substr(pos + 1);
}

}  // namespace

Exporter::ValidationResult Exporter::validateForJlc3dp(
    const std::vector<Vertex>& vertices,
    const std::vector<Triangle>& triangles,
    const UVAtlas& atlas
) const
{
    ValidationResult result;
    result.valid = true;

    if (vertices.empty() || triangles.empty()) {
        result.valid = false;
        result.errors.push_back("Mesh is empty - no vertices or triangles");
        return result;
    }

    constexpr double jlcMinWallThickness = 0.8;
    double minEdgeLength = std::numeric_limits<double>::max();

    for (const Triangle& tri : triangles) {
        if (tri.indices[0] >= vertices.size() || 
            tri.indices[1] >= vertices.size() || 
            tri.indices[2] >= vertices.size()) {
            result.errors.push_back("Invalid triangle indices found");
            result.valid = false;
            continue;
        }

        const Vec3& v0 = vertices[tri.indices[0]].position;
        const Vec3& v1 = vertices[tri.indices[1]].position;
        const Vec3& v2 = vertices[tri.indices[2]].position;

        auto edgeLen = [](const Vec3& a, const Vec3& b) -> double {
            double dx = static_cast<double>(b.x - a.x);
            double dy = static_cast<double>(b.y - a.y);
            double dz = static_cast<double>(b.z - a.z);
            return std::sqrt(dx*dx + dy*dy + dz*dz);
        };

        double e1 = edgeLen(v0, v1);
        double e2 = edgeLen(v1, v2);
        double e3 = edgeLen(v2, v0);

        minEdgeLength = std::min(minEdgeLength, e1);
        minEdgeLength = std::min(minEdgeLength, e2);
        minEdgeLength = std::min(minEdgeLength, e3);
    }

    result.minWallThickness = minEdgeLength;

    if (minEdgeLength < jlcMinWallThickness) {
        result.warnings.push_back(
            "Wall thickness may be below JLC3DP minimum (0.8mm). " +
            std::string("Minimum edge length: ") + std::to_string(minEdgeLength) + "mm"
        );
    }

    int atlasRes = atlas.resolution();
    if (atlasRes < 1024) {
        result.warnings.push_back(
            "Atlas resolution (" + std::to_string(atlasRes) + 
            ") is below recommended 2048 for JLC3DP"
        );
    }

    return result;
}

bool Exporter::exportObjMtl(
    const std::string& outputStem,
    const std::vector<Vertex>& vertices,
    const std::vector<Triangle>& triangles,
    const UVAtlas& atlas,
    const AtlasImage& atlasImage
) const
{
    if (vertices.empty() || triangles.empty()) {
        return false;
    }
    if (atlas.triangleUvs().size() != triangles.size()) {
        return false;
    }

    const std::string objPath = outputStem + ".obj";
    const std::string mtlPath = outputStem + ".mtl";
    const std::string texPath = outputStem + "_atlas.png";

    TextureCompositor compositor;
    if (!compositor.writePng(atlasImage, texPath)) {
        return false;
    }

    std::ofstream mtl(mtlPath);
    if (!mtl.is_open()) {
        return false;
    }
    mtl << "newmtl ColourPaint\n";
    mtl << "Ka 1.000 1.000 1.000\n";
    mtl << "Kd 1.000 1.000 1.000\n";
    mtl << "Ks 0.000 0.000 0.000\n";
    mtl << "d 1.0\n";
    mtl << "illum 2\n";
    mtl << "map_Kd " << stemName(outputStem) << "_atlas.png\n";

    std::ofstream obj(objPath);
    if (!obj.is_open()) {
        return false;
    }

    obj << "mtllib " << stemName(outputStem) << ".mtl\n";
    obj << "usemtl ColourPaint\n";

    for (const Vertex& v : vertices) {
        obj << "v " << v.position.x << " " << v.position.y << " " << v.position.z << "\n";
    }

    for (const Vertex& v : vertices) {
        obj << "vn " << v.normal.x << " " << v.normal.y << " " << v.normal.z << "\n";
    }

    for (const TriangleUv& t : atlas.triangleUvs()) {
        for (int k = 0; k < 3; ++k) {
            obj << "vt " << t.uv[k].u << " " << (1.0F - t.uv[k].v) << "\n";
        }
    }

    std::size_t vtBase = 1;
    for (const Triangle& t : triangles) {
        obj << "f " << (t.indices[0] + 1) << "/" << vtBase << "/" << (t.indices[0] + 1) << " "
            << (t.indices[1] + 1) << "/" << (vtBase + 1) << "/" << (t.indices[1] + 1) << " "
            << (t.indices[2] + 1) << "/" << (vtBase + 2) << "/" << (t.indices[2] + 1) << "\n";
        vtBase += 3;
    }

    return obj.good() && mtl.good();
}

bool Exporter::exportGlb(
    const std::string& outputPath,
    const std::vector<Vertex>& vertices,
    const std::vector<Triangle>& triangles,
    const UVAtlas& atlas,
    const AtlasImage& baseColourAtlas,
    const AtlasImage& metallicRoughnessAtlas,
    const ExportOptions& options
) const
{
    if (vertices.empty() || triangles.empty()) {
        return false;
    }
    if (atlas.triangleUvs().size() != triangles.size()) {
        return false;
    }

    AtlasImage base = baseColourAtlas;
    if (options.forceOpaqueAlpha || options.jlcWjpPreset) {
        forceOpaque(base);
    }

    TextureCompositor compositor;
    std::vector<std::uint8_t> basePng;
    std::vector<std::uint8_t> mrPng;
    if (!compositor.encodePng(base, basePng)) {
        return false;
    }
    if (!compositor.encodePng(metallicRoughnessAtlas, mrPng)) {
        return false;
    }

    std::vector<float> positions;
    std::vector<float> uvs;
    std::vector<std::uint32_t> indices;
    positions.reserve(triangles.size() * 9U);
    uvs.reserve(triangles.size() * 6U);
    indices.reserve(triangles.size() * 3U);

    float minPos[3] = {
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max(),
        std::numeric_limits<float>::max()
    };
    float maxPos[3] = {
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::lowest()
    };

    for (std::size_t triId = 0; triId < triangles.size(); ++triId) {
        const Triangle& tri = triangles[triId];
        const TriangleUv& triUv = atlas.triangleUvs()[triId];
        for (int k = 0; k < 3; ++k) {
            const std::uint32_t idx = tri.indices[k];
            if (idx >= vertices.size()) {
                return false;
            }

            const Vec3& p = vertices[idx].position;
            positions.push_back(p.x);
            positions.push_back(p.y);
            positions.push_back(p.z);

            minPos[0] = std::min(minPos[0], p.x);
            minPos[1] = std::min(minPos[1], p.y);
            minPos[2] = std::min(minPos[2], p.z);
            maxPos[0] = std::max(maxPos[0], p.x);
            maxPos[1] = std::max(maxPos[1], p.y);
            maxPos[2] = std::max(maxPos[2], p.z);

            uvs.push_back(triUv.uv[k].u);
            uvs.push_back(1.0F - triUv.uv[k].v);

            indices.push_back(static_cast<std::uint32_t>(indices.size()));
        }
    }

    std::vector<std::uint8_t> bin;
    const std::uint32_t posOffset = static_cast<std::uint32_t>(bin.size());
    for (float v : positions) {
        appendF32Le(bin, v);
    }
    padTo4(bin);

    const std::uint32_t uvOffset = static_cast<std::uint32_t>(bin.size());
    for (float v : uvs) {
        appendF32Le(bin, v);
    }
    padTo4(bin);

    const std::uint32_t indexOffset = static_cast<std::uint32_t>(bin.size());
    for (std::uint32_t i : indices) {
        appendU32Le(bin, i);
    }
    padTo4(bin);

    const std::uint32_t baseTexOffset = static_cast<std::uint32_t>(bin.size());
    bin.insert(bin.end(), basePng.begin(), basePng.end());
    padTo4(bin);

    const std::uint32_t mrTexOffset = static_cast<std::uint32_t>(bin.size());
    bin.insert(bin.end(), mrPng.begin(), mrPng.end());
    padTo4(bin);

    std::ostringstream json;
    json << std::fixed << std::setprecision(7);
    json << "{";
    json << "\"asset\":{\"version\":\"2.0\",\"generator\":\"FreeCAD Colour Workbench\"},";
    json << "\"scene\":0,";
    json << "\"scenes\":[{\"nodes\":[0]}],";
    json << "\"nodes\":[{\"mesh\":0}],";
    json << "\"meshes\":[{\"primitives\":[{";
    json << "\"attributes\":{\"POSITION\":0,\"TEXCOORD_0\":1},";
    json << "\"indices\":2,";
    json << "\"material\":0";
    json << "}]}],";
    json << "\"materials\":[{";
    json << "\"pbrMetallicRoughness\":{";
    json << "\"baseColorTexture\":{\"index\":0},";
    json << "\"metallicRoughnessTexture\":{\"index\":1},";
    json << "\"metallicFactor\":1.0,\"roughnessFactor\":1.0";
    json << "}}],";
    json << "\"textures\":[{\"source\":0},{\"source\":1}],";
    json << "\"images\":[";
    json << "{\"bufferView\":3,\"mimeType\":\"image/png\"},";
    json << "{\"bufferView\":4,\"mimeType\":\"image/png\"}";
    json << "],";
    json << "\"buffers\":[{\"byteLength\":" << bin.size() << "}],";
    json << "\"bufferViews\":[";
    json << "{\"buffer\":0,\"byteOffset\":" << posOffset << ",\"byteLength\":"
         << (positions.size() * sizeof(float)) << ",\"target\":34962},";
    json << "{\"buffer\":0,\"byteOffset\":" << uvOffset << ",\"byteLength\":"
         << (uvs.size() * sizeof(float)) << ",\"target\":34962},";
    json << "{\"buffer\":0,\"byteOffset\":" << indexOffset << ",\"byteLength\":"
         << (indices.size() * sizeof(std::uint32_t)) << ",\"target\":34963},";
    json << "{\"buffer\":0,\"byteOffset\":" << baseTexOffset << ",\"byteLength\":"
         << basePng.size() << "},";
    json << "{\"buffer\":0,\"byteOffset\":" << mrTexOffset << ",\"byteLength\":"
         << mrPng.size() << "}";
    json << "],";
    json << "\"accessors\":[";
    json << "{\"bufferView\":0,\"componentType\":5126,\"count\":"
         << (positions.size() / 3U)
         << ",\"type\":\"VEC3\",\"min\":[" << minPos[0] << "," << minPos[1] << ","
         << minPos[2] << "],\"max\":[" << maxPos[0] << "," << maxPos[1] << "," << maxPos[2]
         << "]},";
    json << "{\"bufferView\":1,\"componentType\":5126,\"count\":" << (uvs.size() / 2U)
         << ",\"type\":\"VEC2\"},";
    json << "{\"bufferView\":2,\"componentType\":5125,\"count\":" << indices.size()
         << ",\"type\":\"SCALAR\"}";
    json << "]";
    json << "}";

    std::string jsonString = json.str();
    std::vector<std::uint8_t> jsonChunk(jsonString.begin(), jsonString.end());
    while ((jsonChunk.size() % 4U) != 0U) {
        jsonChunk.push_back(static_cast<std::uint8_t>(' '));
    }

    const std::uint32_t totalLength = static_cast<std::uint32_t>(12U + 8U + jsonChunk.size() + 8U + bin.size());

    std::ofstream glb(outputPath, std::ios::binary);
    if (!glb.is_open()) {
        return false;
    }

    std::vector<std::uint8_t> header;
    header.reserve(static_cast<std::size_t>(totalLength));
    appendU32Le(header, 0x46546C67U);  // glTF
    appendU32Le(header, 2U);
    appendU32Le(header, totalLength);

    appendU32Le(header, static_cast<std::uint32_t>(jsonChunk.size()));
    appendU32Le(header, 0x4E4F534AU);  // JSON
    header.insert(header.end(), jsonChunk.begin(), jsonChunk.end());

    appendU32Le(header, static_cast<std::uint32_t>(bin.size()));
    appendU32Le(header, 0x004E4942U);  // BIN
    header.insert(header.end(), bin.begin(), bin.end());

    glb.write(reinterpret_cast<const char*>(header.data()), static_cast<std::streamsize>(header.size()));
    return glb.good();
}

bool Exporter::export3mf(
    const std::string& outputPath,
    const std::vector<Vertex>& vertices,
    const std::vector<Triangle>& triangles,
    const UVAtlas& atlas,
    const AtlasImage& baseColourAtlas,
    const ExportOptions& options
) const
{
    if (vertices.empty() || triangles.empty()) {
        return false;
    }
    if (atlas.triangleUvs().size() != triangles.size()) {
        return false;
    }

    AtlasImage base = baseColourAtlas;
    if (options.forceOpaqueAlpha || options.jlcWjpPreset) {
        forceOpaque(base);
    }

    TextureCompositor compositor;
    std::vector<std::uint8_t> basePng;
    if (!compositor.encodePng(base, basePng)) {
        return false;
    }

    std::ostringstream model;
    model << std::fixed << std::setprecision(7);
    model << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    model << "<model unit=\"millimeter\" xml:lang=\"en-US\" "
          << "xmlns=\"http://schemas.microsoft.com/3dmanufacturing/core/2015/02\" "
          << "xmlns:m=\"http://schemas.microsoft.com/3dmanufacturing/material/2015/02\">\n";
    model << " <metadata name=\"Application\">FreeCAD Colour Workbench</metadata>\n";
    model << " <resources>\n";
    model << "  <m:texture2d id=\"2\" path=\"/3D/Textures/basecolor.png\" contenttype=\"image/png\" "
          << "tilestyleu=\"wrap\" tilestylev=\"wrap\"/>\n";
    model << "  <m:texture2dgroup id=\"3\" texid=\"2\">\n";

    for (const TriangleUv& triUv : atlas.triangleUvs()) {
        for (int k = 0; k < 3; ++k) {
            model << "   <m:tex2coord u=\"" << triUv.uv[k].u << "\" v=\"" << (1.0F - triUv.uv[k].v)
                  << "\"/>\n";
        }
    }

    model << "  </m:texture2dgroup>\n";
    model << "  <object id=\"1\" type=\"model\">\n";
    model << "   <mesh>\n";
    model << "    <vertices>\n";
    for (const Vertex& v : vertices) {
        model << "     <vertex x=\"" << v.position.x << "\" y=\"" << v.position.y << "\" z=\""
              << v.position.z << "\"/>\n";
    }
    model << "    </vertices>\n";
    model << "    <triangles>\n";
    for (std::size_t triId = 0; triId < triangles.size(); ++triId) {
        const Triangle& tri = triangles[triId];
        const std::size_t texBase = triId * 3U;
        model << "     <triangle v1=\"" << tri.indices[0] << "\" v2=\"" << tri.indices[1]
              << "\" v3=\"" << tri.indices[2] << "\" pid=\"3\" p1=\"" << texBase
              << "\" p2=\"" << (texBase + 1U) << "\" p3=\"" << (texBase + 2U) << "\"/>\n";
    }
    model << "    </triangles>\n";
    model << "   </mesh>\n";
    model << "  </object>\n";
    model << " </resources>\n";
    model << " <build>\n";
    model << "  <item objectid=\"1\"/>\n";
    model << " </build>\n";
    model << "</model>\n";

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }

    zipios::ZipOutputStream zip(out);
    zip.putNextEntry("3D/3dmodel.model");
    const std::string modelXml = model.str();
    zip.write(modelXml.data(), static_cast<std::streamsize>(modelXml.size()));
    zip.closeEntry();

    zip.putNextEntry("_rels/.rels");
    const char* rels =
        "<?xml version='1.0' encoding='UTF-8'?>\n"
        "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">\n"
        " <Relationship Target=\"/3D/3dmodel.model\" Id=\"rel0\" "
        "Type=\"http://schemas.microsoft.com/3dmanufacturing/2013/01/3dmodel\" />\n"
        "</Relationships>\n";
    zip.write(rels, static_cast<std::streamsize>(std::char_traits<char>::length(rels)));
    zip.closeEntry();

    zip.putNextEntry("[Content_Types].xml");
    const char* content =
        "<?xml version='1.0' encoding='UTF-8'?>\n"
        "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">\n"
        " <Default Extension=\"rels\" "
        "ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>\n"
        " <Default Extension=\"model\" "
        "ContentType=\"application/vnd.ms-package.3dmanufacturing-3dmodel+xml\"/>\n"
        " <Default Extension=\"png\" ContentType=\"image/png\"/>\n"
        "</Types>\n";
    zip.write(content, static_cast<std::streamsize>(std::char_traits<char>::length(content)));
    zip.closeEntry();

    zip.putNextEntry("3D/Textures/basecolor.png");
    zip.write(reinterpret_cast<const char*>(basePng.data()), static_cast<std::streamsize>(basePng.size()));
    zip.closeEntry();

    zip.close();

    if (options.jlcWjpPreset) {
        // The preset currently enforces opaque export. Additional wall-thickness validation
        // requires shape-level integration and is handled by higher level commands.
    }

    if (!zip.good()) {
        return false;
    }

    return out.good();
}

}  // namespace Colour
