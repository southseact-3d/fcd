// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten CAD Contributors                          *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 *                                                                         *
 **************************************************************************/

#include "RenderEnvironment.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>

namespace Render
{

bool RenderEnvironment::loadHDR(const QString& path)
{
    if (path.isEmpty()) {
        return false;
    }

    // Read the raw HDR file using a simple Radiance HDR parser
    std::string stdPath = path.toStdString();
    std::ifstream file(stdPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    // Read header
    std::string line;
    bool foundFormat = false;
    while (std::getline(file, line)) {
        if (line.empty()) {
            break;  // Empty line marks end of header
        }
        if (line.substr(0, 6) == "FORMAT=") {
            foundFormat = true;
        }
    }

    if (!foundFormat) {
        return false;
    }

    // Read resolution line
    int w = 0;
    int h = 0;
    if (!std::getline(file, line)) {
        return false;
    }

    if (sscanf(line.c_str(), "-Y %d +X %d", &h, &w) != 2) {
        if (sscanf(line.c_str(), "+X %d -Y %d", &w, &h) != 2) {
            return false;
        }
    }

    if (w <= 0 || h <= 0) {
        return false;
    }

    // Read pixel data using run-length encoding
    m_pixels.resize(static_cast<size_t>(w) * static_cast<size_t>(h) * 3);
    std::vector<unsigned char> scanline(static_cast<size_t>(w) * 4);

    for (int y = 0; y < h; ++y) {
        // Read new/old tag
        unsigned char tag[4];
        file.read(reinterpret_cast<char*>(tag), 4);
        if (tag[0] != 2 || tag[1] != 2) {
            // Non-RLE format, not supported for this parser
            m_pixels.clear();
            return false;
        }

        int comp = tag[2] * 256 + tag[3];
        if (comp != w * 4) {
            m_pixels.clear();
            return false;
        }

        // Decode RLE for each component
        for (int c = 0; c < 4; ++c) {
            int x = 0;
            while (x < w) {
                unsigned char byte;
                file.read(reinterpret_cast<char*>(&byte), 1);
                if (byte > 128) {
                    // Run of same values
                    int count = byte - 128;
                    unsigned char value;
                    file.read(reinterpret_cast<char*>(&value), 1);
                    for (int i = 0; i < count && x < w; ++i, ++x) {
                        scanline[static_cast<size_t>(x) * 4 + c] = value;
                    }
                }
                else {
                    // Literal run
                    int count = byte;
                    for (int i = 0; i < count && x < w; ++i, ++x) {
                        unsigned char value;
                        file.read(reinterpret_cast<char*>(&value), 1);
                        scanline[static_cast<size_t>(x) * 4 + c] = value;
                    }
                }
            }
        }

        // Convert to float RGB
        size_t rowOffset = static_cast<size_t>(y) * static_cast<size_t>(w) * 3;
        for (int x = 0; x < w; ++x) {
            size_t srcIdx = static_cast<size_t>(x) * 4;
            for (int c = 0; c < 3; ++c) {
                float val = static_cast<float>(scanline[srcIdx + c]) / 255.0F;
                m_pixels[rowOffset + static_cast<size_t>(x) * 3 + c] = val;
            }
        }
    }

    m_width = w;
    m_height = h;
    m_loadedPath = path;

    buildCDF();
    return true;
}

bool RenderEnvironment::loadHDRFromData(const float* data, int width, int height)
{
    if (!data || width <= 0 || height <= 0) {
        return false;
    }

    size_t numPixels = static_cast<size_t>(width) * static_cast<size_t>(height) * 3;
    m_pixels.assign(data, data + numPixels);
    m_width = width;
    m_height = height;
    m_loadedPath.clear();

    buildCDF();
    return true;
}

Base::Vector3d RenderEnvironment::sampleEnvironment(const Base::Vector3d& direction) const
{
    if (m_pixels.empty() || m_width <= 0 || m_height <= 0) {
        return Base::Vector3d(m_bgColor.x, m_bgColor.y, m_bgColor.z);
    }

    // Normalize the direction
    Base::Vector3d dir = direction;
    double len = dir.Length();
    if (len < 1e-10) {
        return Base::Vector3d(m_bgColor.x, m_bgColor.y, m_bgColor.z);
    }
    dir = dir / len;

    // Apply rotation around the up axis (Z)
    if (m_rotation != 0.0) {
        double rad = m_rotation * M_PI / 180.0;
        double cosR = std::cos(rad);
        double sinR = std::sin(rad);
        double newX = dir.x * cosR - dir.y * sinR;
        double newY = dir.x * sinR + dir.y * cosR;
        dir.x = newX;
        dir.y = newY;
    }

    // Convert to spherical coordinates (equirectangular)
    double theta = std::atan2(dir.y, dir.x);  // longitude [-pi, pi]
    double phi = std::asin(std::clamp(dir.z, -1.0, 1.0));  // latitude [-pi/2, pi/2]

    // Map to UV coordinates
    double u = (theta + M_PI) / (2.0 * M_PI);  // [0, 1]
    double v = (phi + M_PI / 2.0) / M_PI;       // [0, 1]

    // Bilinear sample the HDR image
    double fx = u * static_cast<double>(m_width) - 0.5;
    double fy = v * static_cast<double>(m_height) - 0.5;
    int x0 = static_cast<int>(std::floor(fx));
    int y0 = static_cast<int>(std::floor(fy));
    double fracX = fx - static_cast<double>(x0);
    double fracY = fy - static_cast<double>(y0);

    // Wrap coordinates
    int x1 = (x0 + 1) % m_width;
    int y1 = (y0 + 1) % m_height;
    x0 = ((x0 % m_width) + m_width) % m_width;
    y0 = ((y0 % m_height) + m_height) % m_height;

    // Sample four corners
    auto sample = [&](int x, int y) -> Base::Vector3d {
        size_t idx = (static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x)) * 3;
        return Base::Vector3d(
            static_cast<double>(m_pixels[idx]),
            static_cast<double>(m_pixels[idx + 1]),
            static_cast<double>(m_pixels[idx + 2])
        );
    };

    Base::Vector3d c00 = sample(x0, y0);
    Base::Vector3d c10 = sample(x1, y0);
    Base::Vector3d c01 = sample(x0, y1);
    Base::Vector3d c11 = sample(x1, y1);

    // Bilinear interpolation
    Base::Vector3d result =
        c00 * ((1.0 - fracX) * (1.0 - fracY))
        + c10 * (fracX * (1.0 - fracY))
        + c01 * ((1.0 - fracX) * fracY)
        + c11 * (fracX * fracY);

    // Apply brightness
    result = result * m_brightness;

    // Check if direction is below the ground plane (pointing downward)
    if (m_groundEnabled && dir.z < 0.0) {
        double blend = std::clamp(-dir.z, 0.0, 1.0);
        result = result * (1.0 - blend)
            + Base::Vector3d(m_groundColor.x, m_groundColor.y, m_groundColor.z) * blend;
    }

    return result;
}

double RenderEnvironment::environmentPDF(const Base::Vector3d& direction) const
{
    if (m_pixels.empty() || m_width <= 0 || m_height <= 0) {
        return 1.0 / (4.0 * M_PI);
    }

    Base::Vector3d dir = direction;
    double len = dir.Length();
    if (len < 1e-10) {
        return 1.0 / (4.0 * M_PI);
    }
    dir = dir / len;

    // Apply rotation
    if (m_rotation != 0.0) {
        double rad = m_rotation * M_PI / 180.0;
        double cosR = std::cos(rad);
        double sinR = std::sin(rad);
        double newX = dir.x * cosR - dir.y * sinR;
        double newY = dir.x * sinR + dir.y * cosR;
        dir.x = newX;
        dir.y = newY;
    }

    double theta = std::atan2(dir.y, dir.x);
    double phi = std::asin(std::clamp(dir.z, -1.0, 1.0));
    double u = (theta + M_PI) / (2.0 * M_PI);
    double v = (phi + M_PI / 2.0) / M_PI;

    int x = static_cast<int>(u * static_cast<double>(m_width)) % m_width;
    int y = static_cast<int>(v * static_cast<double>(m_height)) % m_height;
    if (x < 0) {
        x += m_width;
    }
    if (y < 0) {
        y += m_height;
    }

    size_t idx = (static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x)) * 3;
    double luminance = 0.2126 * static_cast<double>(m_pixels[idx])
                     + 0.7152 * static_cast<double>(m_pixels[idx + 1])
                     + 0.0722 * static_cast<double>(m_pixels[idx + 2]);
    luminance *= m_brightness;

    // PDF = luminance / total luminance over sphere
    // For importance sampling, we use the marginal + conditional CDF
    if (!m_marginalPDF.empty() && y < static_cast<int>(m_marginalPDF.size())) {
        return m_marginalPDF[static_cast<size_t>(y)] / M_PI;
    }

    return luminance * std::cos(phi) / M_PI + 1e-10;
}

void RenderEnvironment::setRotation(double degrees)
{
    m_rotation = degrees;
}

void RenderEnvironment::setBrightness(double brightness)
{
    m_brightness = brightness;
}

void RenderEnvironment::setBackgroundVisible(bool visible)
{
    m_bgVisible = visible;
}

void RenderEnvironment::setBackgroundColor(const Base::Vector3d& color)
{
    m_bgColor = color;
}

void RenderEnvironment::setGroundPlane(bool enabled, double height, const Base::Vector3d& color)
{
    m_groundEnabled = enabled;
    m_groundHeight = height;
    m_groundColor = color;
}

QString RenderEnvironment::loadedPath() const
{
    return m_loadedPath;
}

bool RenderEnvironment::isLoaded() const
{
    return !m_pixels.empty() && m_width > 0 && m_height > 0;
}

int RenderEnvironment::width() const
{
    return m_width;
}

int RenderEnvironment::height() const
{
    return m_height;
}

const float* RenderEnvironment::data() const
{
    if (m_pixels.empty()) {
        return nullptr;
    }
    return m_pixels.data();
}

const std::vector<double>& RenderEnvironment::marginalPDF() const
{
    return m_marginalPDF;
}

const std::vector<std::vector<double>>& RenderEnvironment::conditionalPDF() const
{
    return m_conditionalCDF;
}

void RenderEnvironment::buildCDF()
{
    if (m_pixels.empty() || m_width <= 0 || m_height <= 0) {
        return;
    }

    // Compute per-row luminance and build marginal/conditional CDFs
    // for environment importance sampling (Vitteritsky / Hierarchical sampling)

    m_marginalPDF.resize(static_cast<size_t>(m_height));
    m_conditionalCDF.resize(static_cast<size_t>(m_height));

    double totalLuminance = 0.0;

    // Step 1: Compute conditional CDF for each row (horizontal CDF)
    for (int y = 0; y < m_height; ++y) {
        m_conditionalCDF[static_cast<size_t>(y)].resize(static_cast<size_t>(m_width));
        double rowLuminance = 0.0;

        for (int x = 0; x < m_width; ++x) {
            size_t idx = (static_cast<size_t>(y) * static_cast<size_t>(m_width) + static_cast<size_t>(x)) * 3;
            double lum = 0.2126 * static_cast<double>(m_pixels[idx])
                       + 0.7152 * static_cast<double>(m_pixels[idx + 1])
                       + 0.0722 * static_cast<double>(m_pixels[idx + 2]);
            lum = std::max(lum, 0.0);

            // Multiply by sin(theta) for solid angle weighting
            double v = (static_cast<double>(y) + 0.5) / static_cast<double>(m_height);
            double phi = v * M_PI - M_PI / 2.0;
            double sinPhi = std::cos(phi);  // cos(theta) in spherical coords, where theta is polar angle from equator
            lum *= sinPhi;

            rowLuminance += lum;
            m_conditionalCDF[static_cast<size_t>(y)][static_cast<size_t>(x)] = rowLuminance;
        }

        // Normalize the conditional CDF
        if (rowLuminance > 1e-10) {
            for (int x = 0; x < m_width; ++x) {
                m_conditionalCDF[static_cast<size_t>(y)][static_cast<size_t>(x)] /= rowLuminance;
            }
        }

        m_marginalPDF[static_cast<size_t>(y)] = rowLuminance;
        totalLuminance += rowLuminance;
    }

    // Step 2: Build marginal CDF from row luminances
    m_marginalCDF.resize(static_cast<size_t>(m_height));
    double cumLuminance = 0.0;
    for (int y = 0; y < m_height; ++y) {
        cumLuminance += m_marginalPDF[static_cast<size_t>(y)];
        m_marginalCDF[static_cast<size_t>(y)] = cumLuminance;
    }

    // Normalize the marginal CDF
    if (totalLuminance > 1e-10) {
        for (int y = 0; y < m_height; ++y) {
            m_marginalCDF[static_cast<size_t>(y)] /= totalLuminance;
            m_marginalPDF[static_cast<size_t>(y)] /= totalLuminance;
        }
    }
}

}  // namespace Render
