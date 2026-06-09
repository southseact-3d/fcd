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

#ifndef RENDER_TEXTURERT_H
#define RENDER_TEXTURERT_H

#include <vector>
#include <Base/Vector3D.h>
#include <QImage>
#include "RenderRaytracerGlobal.h"

namespace Render
{

class RenderRaytracerExport TextureRT
{
public:
    TextureRT() = default;

    Base::Vector3d sample(double u, double v) const;
    Base::Vector3d sampleEnvironment(const Base::Vector3d& direction) const;

    void loadFromHDR(const float* data, int w, int h, int ch);
    void loadFromQImage(const QImage& img);

    bool hasData() const { return !pixels.empty(); }
    bool isHDRTexture() const { return isHDR; }

    void buildImportanceMap();
    void sampleImportance(double u, double v, double& outU, double& outV, double& pdf) const;

private:
    std::vector<float> pixels;
    int width = 0;
    int height = 0;
    int channels = 0;
    bool isHDR = false;

    // Importance sampling CDFs
    std::vector<double> marginalCDF;
    std::vector<double> conditionalCDF;
    std::vector<double> marginalPDF;
    bool importanceMapBuilt = false;
};

}  // namespace Render

#endif  // RENDER_TEXTURERT_H
