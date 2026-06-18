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

#ifndef RENDER_RENDERSETTINGS_H
#define RENDER_RENDERSETTINGS_H

#include <QString>

#include "RenderAppGlobal.h"

namespace Render
{

enum class ToneMapping
{
    Reinhard,
    ACES,
    Filmic,
    None
};

enum class OutputFormat
{
    PNG,
    JPEG,
    EXR
};

class RenderAppExport RenderSettings
{
public:
    RenderSettings() = default;

    int width() const;
    int height() const;
    void setResolution(int w, int h);
    void setAspectPreset(const QString& preset);

    int maxSamples() const;
    void setMaxSamples(int samples);
    double maxBounces() const;
    void setMaxBounces(int bounces);

    ToneMapping toneMapping() const;
    void setToneMapping(ToneMapping tm);
    OutputFormat outputFormat() const;
    void setOutputFormat(OutputFormat fmt);

    QString outputPath() const;
    void setOutputPath(const QString& path);

    int jpegQuality() const;
    void setJPEGQuality(int q);

    double noiseThreshold() const;
    void setNoiseThreshold(double threshold);

private:
    int m_width{1920};
    int m_height{1080};
    int m_maxSamples{256};
    int m_maxBounces{8};
    ToneMapping m_toneMapping{ToneMapping::ACES};
    OutputFormat m_outputFormat{OutputFormat::PNG};
    QString m_outputPath;
    int m_jpegQuality{95};
    double m_noiseThreshold{0.01};
};

}  // namespace Render

#endif  // RENDER_RENDERSETTINGS_H
