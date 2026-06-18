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

#ifndef RENDER_RENDERENVIRONMENT_H
#define RENDER_RENDERENVIRONMENT_H

#include <vector>

#include <QString>

#include <Base/Vector3D.h>

#include "RenderAppGlobal.h"

namespace Render
{

class RenderAppExport RenderEnvironment
{
public:
    RenderEnvironment() = default;

    bool loadHDR(const QString& path);
    bool loadHDRFromData(const float* data, int width, int height);

    Base::Vector3d sampleEnvironment(const Base::Vector3d& direction) const;
    double environmentPDF(const Base::Vector3d& direction) const;

    void setRotation(double degrees);
    void setBrightness(double brightness);
    void setBackgroundVisible(bool visible);
    void setBackgroundColor(const Base::Vector3d& color);
    void setGroundPlane(bool enabled, double height, const Base::Vector3d& color);

    QString loadedPath() const;
    bool isLoaded() const;
    int width() const;
    int height() const;
    const float* data() const;

    const std::vector<double>& marginalPDF() const;
    const std::vector<std::vector<double>>& conditionalPDF() const;

private:
    std::vector<float> m_pixels;
    int m_width{0};
    int m_height{0};
    double m_rotation{0.0};
    double m_brightness{1.0};
    bool m_bgVisible{true};
    Base::Vector3d m_bgColor{0.5, 0.5, 0.5};
    bool m_groundEnabled{false};
    double m_groundHeight{0.0};
    Base::Vector3d m_groundColor{0.8, 0.8, 0.8};
    QString m_loadedPath;

    std::vector<double> m_marginalPDF;
    std::vector<double> m_marginalCDF;
    std::vector<std::vector<double>> m_conditionalCDF;

    void buildCDF();
};

}  // namespace Render

#endif  // RENDER_RENDERENVIRONMENT_H
