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

#ifndef RENDER_OUTPUTWRITER_H
#define RENDER_OUTPUTWRITER_H

#include <QString>
#include <QImage>
#include <Base/Vector3D.h>
#include "RenderRaytracerGlobal.h"

namespace Render
{

class RenderRaytracerExport OutputWriter
{
public:
    static bool writePNG(const QString& path, const QImage& image);
    static bool writeJPEG(const QString& path, const QImage& image, int quality = 95);
    static bool writeEXR(const QString& path, const float* data, int width, int height,
                         int channels);

    static Base::Vector3d toneMap(const Base::Vector3d& color, int method);
    static Base::Vector3d gammaCorrect(const Base::Vector3d& color, double gamma);

    enum ToneMapMethod
    {
        ToneMapReinhard = 0,
        ToneMapACES = 1,
        ToneMapFilmic = 2
    };
};

}  // namespace Render

#endif  // RENDER_OUTPUTWRITER_H
