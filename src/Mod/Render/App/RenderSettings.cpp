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

#include "RenderSettings.h"

namespace Render
{

int RenderSettings::width() const
{
    return m_width;
}

int RenderSettings::height() const
{
    return m_height;
}

void RenderSettings::setResolution(int w, int h)
{
    m_width = w;
    m_height = h;
}

void RenderSettings::setAspectPreset(const QString& preset)
{
    if (preset == QStringLiteral("16:9")) {
        m_width = 1920;
        m_height = 1080;
    }
    else if (preset == QStringLiteral("4:3")) {
        m_width = 1600;
        m_height = 1200;
    }
    else if (preset == QStringLiteral("1:1")) {
        m_width = 1080;
        m_height = 1080;
    }
    else if (preset == QStringLiteral("21:9")) {
        m_width = 2560;
        m_height = 1080;
    }
    else if (preset == QStringLiteral("HD")) {
        m_width = 1280;
        m_height = 720;
    }
    else if (preset == QStringLiteral("4K")) {
        m_width = 3840;
        m_height = 2160;
    }
    else if (preset == QStringLiteral("2K")) {
        m_width = 2048;
        m_height = 1080;
    }
}

int RenderSettings::maxSamples() const
{
    return m_maxSamples;
}

void RenderSettings::setMaxSamples(int samples)
{
    m_maxSamples = samples;
}

double RenderSettings::maxBounces() const
{
    return m_maxBounces;
}

void RenderSettings::setMaxBounces(int bounces)
{
    m_maxBounces = bounces;
}

ToneMapping RenderSettings::toneMapping() const
{
    return m_toneMapping;
}

void RenderSettings::setToneMapping(ToneMapping tm)
{
    m_toneMapping = tm;
}

OutputFormat RenderSettings::outputFormat() const
{
    return m_outputFormat;
}

void RenderSettings::setOutputFormat(OutputFormat fmt)
{
    m_outputFormat = fmt;
}

QString RenderSettings::outputPath() const
{
    return m_outputPath;
}

void RenderSettings::setOutputPath(const QString& path)
{
    m_outputPath = path;
}

int RenderSettings::jpegQuality() const
{
    return m_jpegQuality;
}

void RenderSettings::setJPEGQuality(int q)
{
    m_jpegQuality = q;
}

double RenderSettings::noiseThreshold() const
{
    return m_noiseThreshold;
}

void RenderSettings::setNoiseThreshold(double threshold)
{
    m_noiseThreshold = threshold;
}

}  // namespace Render
