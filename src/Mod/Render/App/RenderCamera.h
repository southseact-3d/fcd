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

#ifndef RENDER_RENDERCAMERA_H
#define RENDER_RENDERCAMERA_H

#include <Base/Vector3D.h>

#include "RenderAppGlobal.h"

namespace Gui
{
class View3DInventorViewer;
}

namespace Render
{

class RenderAppExport RenderCamera
{
public:
    RenderCamera() = default;

    void captureFromViewer(Gui::View3DInventorViewer* viewer);

    void setPosition(const Base::Vector3d& pos);
    void setTarget(const Base::Vector3d& target);
    void setUp(const Base::Vector3d& up);
    void setFOV(double fov);
    void setOrthographic(bool ortho);
    void setDOF(double aperture, double focalDist);
    void setExposure(double ev100);
    void setResolution(int w, int h);

    Base::Vector3d position() const;
    Base::Vector3d target() const;
    Base::Vector3d up() const;
    double fov() const;
    bool isOrthographic() const;
    double aperture() const;
    double focalDistance() const;
    double exposure() const;
    int width() const;
    int height() const;

private:
    Base::Vector3d m_position{0, -10, 0};
    Base::Vector3d m_target{0, 0, 0};
    Base::Vector3d m_up{0, 0, 1};
    double m_fov{45.0};
    bool m_orthographic{false};
    double m_aperture{0.0};
    double m_focalDistance{10.0};
    double m_exposure{0.0};
    int m_width{1920};
    int m_height{1080};
};

}  // namespace Render

#endif  // RENDER_RENDERCAMERA_H
