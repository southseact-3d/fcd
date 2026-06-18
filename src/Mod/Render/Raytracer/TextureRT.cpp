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

#include "TextureRT.h"
#include <cmath>
#include <algorithm>

namespace Render
{

static constexpr double PI = 3.14159265358979323846;

Base::Vector3d TextureRT::sample(double u, double v) const
{
    if (pixels.empty() || width <= 0 || height <= 0) {
        return Base::Vector3d(0.0, 0.0, 0.0);
    }

    // Wrap UVs
    u = u - std::floor(u);
    v = v - std::floor(v);

    // Bilinear sampling
    double fx = u * (width - 1);
    double fy = v * (height - 1);
    int x0 = static_cast<int>(fx);
    int y0 = static_cast<int>(fy);
    int x1 = std::min(x0 + 1, width - 1);
    int y1 = std::min(y0 + 1, height - 1);
    double dx = fx - x0;
    double dy = fy - y0;

    auto getPixel = [&](int x, int y, int c) -> double {
        int idx = (y * width + x) * channels + c;
        if (idx < 0 || idx >= static_cast<int>(pixels.size())) return 0.0;
        return static_cast<double>(pixels[idx]);
    };

    Base::Vector3d result(0.0, 0.0, 0.0);
    int ch = std::min(channels, 3);

    for (int c = 0; c < ch; ++c) {
        double v00 = getPixel(x0, y0, c);
        double v10 = getPixel(x1, y0, c);
        double v01 = getPixel(x0, y1, c);
        double v11 = getPixel(x1, y1, c);

        double val = v00 * (1.0 - dx) * (1.0 - dy)
                   + v10 * dx * (1.0 - dy)
                   + v01 * (1.0 - dx) * dy
                   + v11 * dx * dy;

        result[c] = val;
    }

    // If LDR (8-bit), linearize
    if (!isHDR) {
        result.x = std::pow(result.x, 2.2);
        result.y = std::pow(result.y, 2.2);
        result.z = std::pow(result.z, 2.2);
    }

    return result;
}

Base::Vector3d TextureRT::sampleEnvironment(const Base::Vector3d& direction) const
{
    if (pixels.empty()) {
        return Base::Vector3d(0.0, 0.0, 0.0);
    }

    // Spherical mapping: direction -> lat/long UV
    double len = direction.Length();
    if (len < 1e-10) {
        return Base::Vector3d(0.0, 0.0, 0.0);
    }

    Base::Vector3d dir = direction / len;
    double theta = std::acos(std::max(-1.0, std::min(1.0, dir.z)));
    double phi = std::atan2(dir.y, dir.x);
    if (phi < 0.0) phi += 2.0 * PI;

    double u = phi / (2.0 * PI);
    double v = theta / PI;

    return sample(u, v);
}

void TextureRT::loadFromHDR(const float* data, int w, int h, int ch)
{
    if (!data || w <= 0 || h <= 0 || ch <= 0) return;

    width = w;
    height = h;
    channels = ch;
    isHDR = true;

    int total = width * height * channels;
    pixels.resize(total);
    std::copy(data, data + total, pixels.begin());
}

void TextureRT::loadFromQImage(const QImage& img)
{
    if (img.isNull()) return;

    QImage converted = img.convertToFormat(QImage::Format_RGB32);
    width = converted.width();
    height = converted.height();
    channels = 4;
    isHDR = false;

    int total = width * height * channels;
    pixels.resize(total);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            QRgb pixel = converted.pixel(x, y);
            int idx = (y * width + x) * channels;
            pixels[idx + 0] = static_cast<float>(qRed(pixel)) / 255.0f;
            pixels[idx + 1] = static_cast<float>(qGreen(pixel)) / 255.0f;
            pixels[idx + 2] = static_cast<float>(qBlue(pixel)) / 255.0f;
            pixels[idx + 3] = static_cast<float>(qAlpha(pixel)) / 255.0f;
        }
    }
}

void TextureRT::buildImportanceMap()
{
    if (pixels.empty() || width <= 0 || height <= 0) {
        importanceMapBuilt = false;
        return;
    }

    int w = width;
    int h = height;

    // Build 2D conditional CDF from luminance
    std::vector<double> luminance(w * h);
    std::vector<double> conditionalRow(h * w);

    for (int y = 0; y < h; ++y) {
        double rowSum = 0.0;
        for (int x = 0; x < w; ++x) {
            int idx = (y * w + x) * channels;
            double r = (channels > 0) ? static_cast<double>(pixels[idx + 0]) : 0.0;
            double g = (channels > 1) ? static_cast<double>(pixels[idx + 1]) : 0.0;
            double b = (channels > 2) ? static_cast<double>(pixels[idx + 2]) : 0.0;

            // Luminance for importance sampling
            double lum = 0.2126 * r + 0.7152 * g + 0.0722 * b;

            // Apply sin(theta) weighting for spherical mapping
            double theta = PI * static_cast<double>(y) / h;
            lum *= std::sin(theta);

            luminance[y * w + x] = lum;
            rowSum += lum;
            conditionalRow[y * w + x] = rowSum;
        }

        // Normalize row CDF
        if (rowSum > 0.0) {
            for (int x = 0; x < w; ++x) {
                conditionalRow[y * w + x] /= rowSum;
            }
        }
    }

    // Build marginal CDF
    marginalPDF.resize(h);
    marginalCDF.resize(h);
    double totalSum = 0.0;
    for (int y = 0; y < h; ++y) {
        double rowSum = 0.0;
        for (int x = 0; x < w; ++x) {
            rowSum += luminance[y * w + x];
        }
        totalSum += rowSum;
        marginalCDF[y] = totalSum;
        marginalPDF[y] = rowSum;
    }

    // Normalize
    if (totalSum > 0.0) {
        for (int y = 0; y < h; ++y) {
            marginalCDF[y] /= totalSum;
            marginalPDF[y] /= totalSum;
        }
    }

    conditionalCDF = conditionalRow;
    importanceMapBuilt = true;
}

void TextureRT::sampleImportance(double u, double v, double& outU, double& outV,
                                 double& pdf) const
{
    if (!importanceMapBuilt || marginalCDF.empty() || conditionalCDF.empty()) {
        // Fallback: uniform sampling
        outU = u;
        outV = v;
        pdf = 1.0;
        return;
    }

    int h = height;
    int w = width;

    // Sample marginal CDF for row
    int row = 0;
    for (int i = 0; i < h - 1; ++i) {
        if (v < marginalCDF[i]) {
            row = i;
            break;
        }
        row = i + 1;
    }

    // Sample conditional CDF for column
    int col = 0;
    for (int i = 0; i < w - 1; ++i) {
        if (u < conditionalCDF[row * w + i]) {
            col = i;
            break;
        }
        col = i + 1;
    }

    outU = (static_cast<double>(col) + 0.5) / w;
    outV = (static_cast<double>(row) + 0.5) / h;

    // PDF = 1 / total luminance (constant for discrete case)
    double totalLum = 0.0;
    for (int i = 0; i < h; ++i) {
        totalLum += marginalPDF[i];
    }
    pdf = (totalLum > 0.0) ? (1.0 / (w * h * totalLum)) : (1.0 / (w * h));
}

}  // namespace Render
