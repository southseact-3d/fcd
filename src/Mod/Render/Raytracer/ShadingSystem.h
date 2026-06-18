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

#ifndef RENDER_SHADINGSYSTEM_H
#define RENDER_SHADINGSYSTEM_H

#include <Base/Vector3D.h>
#include <App/Material.h>
#include "RenderRaytracerGlobal.h"

namespace Render
{

struct ShadingParams
{
    Base::Vector3d baseColor{0.8, 0.8, 0.8};
    double metallic = 0.0;
    double roughness = 0.5;
    double specular = 0.5;
    Base::Vector3d normal{0.0, 0.0, 1.0};
    Base::Vector3d emissive{0.0, 0.0, 0.0};
    double opacity = 1.0;
};

struct ShadingResult
{
    Base::Vector3d brdf{0.0, 0.0, 0.0};
    double pdf = 0.0;
    bool isEmissive = false;
};

class RenderRaytracerExport ShadingSystem
{
public:
    ShadingResult evaluate(const ShadingParams& params,
                           const Base::Vector3d& wo,
                           const Base::Vector3d& wi) const;

    Base::Vector3d sampleBSDF(const ShadingParams& params,
                              const Base::Vector3d& wo,
                              double& pdf,
                              Base::Vector3d& tangent,
                              Base::Vector3d& bitangent) const;

    static ShadingParams fromAppMaterial(const App::Material& mat);

    static double computeDielectricF0(double ior);

private:
    static double ggxD(const Base::Vector3d& normal, const Base::Vector3d& halfVec, double alpha);
    static double smithG1(const Base::Vector3d& normal, const Base::Vector3d& direction, double k);
    static double smithG(const Base::Vector3d& normal, const Base::Vector3d& wo,
                         const Base::Vector3d& wi, double k);
    static Base::Vector3d schlickFresnel(const Base::Vector3d& f0, double cosTheta);
    static double powerHeuristic(double pdfA, double pdfB);
    static void buildCoordinateSystem(const Base::Vector3d& normal,
                                      Base::Vector3d& tangent,
                                      Base::Vector3d& bitangent);
};

}  // namespace Render

#endif  // RENDER_SHADINGSYSTEM_H
