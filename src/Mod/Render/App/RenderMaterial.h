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

#ifndef RENDER_RENDERMATERIAL_H
#define RENDER_RENDERMATERIAL_H

#include <QString>

#include <App/Material.h>
#include <Base/Vector3D.h>

#include "RenderAppGlobal.h"

namespace Materials
{
class Material;
}

namespace Render
{

struct RenderMaterialData
{
    Base::Vector3d baseColor{0.8, 0.8, 0.8};
    double metallic{0.0};
    double roughness{0.5};
    double specular{0.5};
    Base::Vector3d emissive{0, 0, 0};
    double opacity{1.0};
    QString baseColorTexture;
    QString metallicTexture;
    QString roughnessTexture;
    QString normalTexture;
};

class RenderAppExport RenderMaterial
{
public:
    RenderMaterial() = default;
    explicit RenderMaterial(const RenderMaterialData& data);

    static RenderMaterialData fromAppMaterial(const App::Material& mat);
    static RenderMaterialData fromPBRProperties(const Materials::Material& mat);

    App::Material toAppMaterial() const;

    const RenderMaterialData& data() const;
    void setData(const RenderMaterialData& data);

private:
    static bool isMetallicType(App::Material::MaterialType type);
    static double colorLuminance(const Base::Color& color);

    RenderMaterialData m_data;
};

}  // namespace Render

#endif  // RENDER_RENDERMATERIAL_H
