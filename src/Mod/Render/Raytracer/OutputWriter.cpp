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

#include "OutputWriter.h"

#define TINYEXR_IMPLEMENTATION
#include <tinyexr.h>

#include <cmath>
#include <algorithm>

namespace Render
{

bool OutputWriter::writePNG(const QString& path, const QImage& image)
{
    if (image.isNull()) return false;
    return image.save(path, "PNG");
}

bool OutputWriter::writeJPEG(const QString& path, const QImage& image, int quality)
{
    if (image.isNull()) return false;
    return image.save(path, "JPEG", quality);
}

bool OutputWriter::writeEXR(const QString& path, const float* data, int width, int height,
                            int channels)
{
    if (!data || width <= 0 || height <= 0) return false;

    EXRHeader header;
    InitEXRHeader(&header);

    EXRImage image;
    InitEXRImage(&image);

    image.num_channels = channels;

    std::vector<const char*> channelNames;
    if (channels >= 3) {
        channelNames.push_back("R");
        channelNames.push_back("G");
        channelNames.push_back("B");
        if (channels == 4) {
            channelNames.push_back("A");
        }
    }
    else if (channels == 2) {
        channelNames.push_back("R");
        channelNames.push_back("G");
    }
    else {
        channelNames.push_back("R");
    }

    header.num_channels = channels;
    header.channels = static_cast<EXRChannelInfo*>(malloc(sizeof(EXRChannelInfo) * channels));
    for (int i = 0; i < channels; ++i) {
        strncpy(header.channels[i].name, channelNames[i], 255);
        header.channels[i].name[255] = '\0';
    }

    header.pixel_type = TINYEXR_PIXELTYPE_FLOAT;
    header.requested_pixel_type = TINYEXR_PIXELTYPE_FLOAT;
    header.flags = 0;

    image.images = static_cast<unsigned char**>(malloc(sizeof(unsigned char*) * channels));
    image.width = width;
    image.height = height;

    int pixelCount = width * height;
    for (int c = 0; c < channels; ++c) {
        float* channelData = static_cast<float*>(malloc(sizeof(float) * pixelCount));
        for (int i = 0; i < pixelCount; ++i) {
            channelData[i] = data[i * channels + c];
        }
        image.images[c] = reinterpret_cast<unsigned char*>(channelData);
    }

    const char* err = nullptr;
    int ret = SaveEXRImageToFile(&image, &header, path.toStdString().c_str(), &err);

    for (int c = 0; c < channels; ++c) {
        free(image.images[c]);
    }
    free(image.images);
    free(header.channels);

    if (err) {
        free(const_cast<char*>(err));
    }

    return ret == TINYEXR_SUCCESS;
}

Base::Vector3d OutputWriter::toneMap(const Base::Vector3d& color, int method)
{
    switch (method) {
        case ToneMapReinhard: {
            double luminance = 0.2126 * color.x + 0.7152 * color.y + 0.0722 * color.z;
            double mapped = luminance / (1.0 + luminance);
            double scale = (luminance > 0.0) ? mapped / luminance : 0.0;
            return Base::Vector3d(color.x * scale, color.y * scale, color.z * scale);
        }
        case ToneMapACES: {
            // ACES filmic tone mapping
            const double a = 2.51;
            const double b = 0.03;
            const double c = 2.43;
            const double d = 0.59;
            const double e = 0.14;

            auto acesMap = [&](double x) -> double {
                x = std::max(0.0, x);
                return std::max(0.0, std::min(1.0,
                    (x * (a * x + b)) / (x * (c * x + d) + e)));
            };

            return Base::Vector3d(
                acesMap(color.x),
                acesMap(color.y),
                acesMap(color.z)
            );
        }
        case ToneMapFilmic: {
            // Uncharted 2 filmic tone mapping
            auto filmicMap = [](double x) -> double {
                double A = 0.15;
                double B = 0.50;
                double C = 0.10;
                double D = 0.20;
                double E = 0.02;
                double F = 0.30;
                return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
            };

            double exposureBias = 2.0;
            double W = 11.2;
            double scale = 1.0 / filmicMap(W);

            return Base::Vector3d(
                filmicMap(color.x * exposureBias) * scale,
                filmicMap(color.y * exposureBias) * scale,
                filmicMap(color.z * exposureBias) * scale
            );
        }
        default:
            return color;
    }
}

Base::Vector3d OutputWriter::gammaCorrect(const Base::Vector3d& color, double gamma)
{
    double invGamma = 1.0 / gamma;
    return Base::Vector3d(
        std::pow(std::max(0.0, color.x), invGamma),
        std::pow(std::max(0.0, color.y), invGamma),
        std::pow(std::max(0.0, color.z), invGamma)
    );
}

}  // namespace Render
