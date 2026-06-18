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

#include "RenderCamera.h"

#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

#include <Gui/View3DInventorViewer.h>

namespace Render
{

void RenderCamera::captureFromViewer(Gui::View3DInventorViewer* viewer)
{
    if (!viewer) {
        return;
    }

    SoCamera* camera = viewer->getCamera();
    if (!camera) {
        return;
    }

    // Read camera position from Coin3D
    const SbVec3f& coinPos = camera->position.getValue();
    m_position = Base::Vector3d(
        static_cast<double>(coinPos[0]),
        static_cast<double>(coinPos[1]),
        static_cast<double>(coinPos[2])
    );

    // Read camera orientation and extract target/up from it
    const SbRotation& coinOrient = camera->orientation.getValue();
    SbVec3f coinDir;
    coinOrient.multVec(SbVec3f(0, 0, -1), coinDir);

    // The target is along the view direction from the position
    SbVec3f coinTarget = coinPos + coinDir * camera->nearDistance.getValue();
    m_target = Base::Vector3d(
        static_cast<double>(coinTarget[0]),
        static_cast<double>(coinTarget[1]),
        static_cast<double>(coinTarget[2])
    );

    // Extract up vector from orientation
    SbVec3f coinUp;
    coinOrient.multVec(SbVec3f(0, 1, 0), coinUp);
    m_up = Base::Vector3d(
        static_cast<double>(coinUp[0]),
        static_cast<double>(coinUp[1]),
        static_cast<double>(coinUp[2])
    );

    // Detect projection type
    m_orthographic = camera->getTypeId().isOfType(SoOrthographicCamera::getClassTypeId());

    // Read FOV for perspective cameras
    if (!m_orthographic) {
        auto* perspCam = static_cast<SoPerspectiveCamera*>(camera);
        double coinFOV = static_cast<double>(perspCam->heightAngle.getValue());
        m_fov = coinFOV * 180.0 / M_PI;
    }
    else {
        m_fov = 45.0;
    }

    // Read viewport dimensions
    const SbViewportRegion& vp = viewer->getViewportRegion();
    SbVec2s vpSize = vp.getViewportSizePixels();
    m_width = static_cast<int>(vpSize[0]);
    m_height = static_cast<int>(vpSize[1]);
}

void RenderCamera::setPosition(const Base::Vector3d& pos)
{
    m_position = pos;
}

void RenderCamera::setTarget(const Base::Vector3d& target)
{
    m_target = target;
}

void RenderCamera::setUp(const Base::Vector3d& up)
{
    m_up = up;
}

void RenderCamera::setFOV(double fov)
{
    m_fov = fov;
}

void RenderCamera::setOrthographic(bool ortho)
{
    m_orthographic = ortho;
}

void RenderCamera::setDOF(double aperture, double focalDist)
{
    m_aperture = aperture;
    m_focalDistance = focalDist;
}

void RenderCamera::setExposure(double ev100)
{
    m_exposure = ev100;
}

void RenderCamera::setResolution(int w, int h)
{
    m_width = w;
    m_height = h;
}

Base::Vector3d RenderCamera::position() const
{
    return m_position;
}

Base::Vector3d RenderCamera::target() const
{
    return m_target;
}

Base::Vector3d RenderCamera::up() const
{
    return m_up;
}

double RenderCamera::fov() const
{
    return m_fov;
}

bool RenderCamera::isOrthographic() const
{
    return m_orthographic;
}

double RenderCamera::aperture() const
{
    return m_aperture;
}

double RenderCamera::focalDistance() const
{
    return m_focalDistance;
}

double RenderCamera::exposure() const
{
    return m_exposure;
}

int RenderCamera::width() const
{
    return m_width;
}

int RenderCamera::height() const
{
    return m_height;
}

}  // namespace Render
