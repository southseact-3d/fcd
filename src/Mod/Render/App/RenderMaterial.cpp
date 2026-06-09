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

#include "RenderMaterial.h"

#include <cmath>

#include <Mod/Material/App/Materials.h>

namespace Render
{

RenderMaterial::RenderMaterial(const RenderMaterialData& data)
    : m_data(data)
{}

RenderMaterialData RenderMaterial::fromAppMaterial(const App::Material& mat)
{
    RenderMaterialData data;

    // Map diffuse color to base color
    data.baseColor = Base::Vector3d(
        static_cast<double>(mat.diffuseColor.r),
        static_cast<double>(mat.diffuseColor.g),
        static_cast<double>(mat.diffuseColor.b)
    );

    // Map specular color: use luminance as specular intensity
    data.specular = colorLuminance(mat.specularColor);

    // Map emissive color
    data.emissive = Base::Vector3d(
        static_cast<double>(mat.emissiveColor.r),
        static_cast<double>(mat.emissiveColor.g),
        static_cast<double>(mat.emissiveColor.b)
    );

    // Map shininess to roughness (invert: rough = not shiny)
    data.roughness = 1.0 - static_cast<double>(mat.shininess) / 100.0;
    if (data.roughness < 0.0) {
        data.roughness = 0.0;
    }
    if (data.roughness > 1.0) {
        data.roughness = 1.0;
    }

    // Map transparency to opacity
    data.opacity = 1.0 - static_cast<double>(mat.transparency);
    if (data.opacity < 0.0) {
        data.opacity = 0.0;
    }
    if (data.opacity > 1.0) {
        data.opacity = 1.0;
    }

    // Map image path to base color texture
    if (!mat.imagePath.empty()) {
        data.baseColorTexture = QString::fromStdString(mat.imagePath);
    }

    // Set metallic based on material type
    if (isMetallicType(mat.getType())) {
        data.metallic = 1.0;
    }
    else {
        data.metallic = 0.0;
    }

    return data;
}

RenderMaterialData RenderMaterial::fromPBRProperties(const Materials::Material& mat)
{
    RenderMaterialData data;

    // Read Render.Substance_PBR properties
    // BaseColor
    auto baseColorProp = mat.getAppearanceProperty(
        QStringLiteral("Render.Substance_PBR.BaseColor")
    );
    if (baseColorProp && !baseColorProp->isNull()) {
        Base::Color color = baseColorProp->getColor();
        data.baseColor = Base::Vector3d(
            static_cast<double>(color.r),
            static_cast<double>(color.g),
            static_cast<double>(color.b)
        );
    }

    // Metallic
    auto metallicProp = mat.getAppearanceProperty(
        QStringLiteral("Render.Substance_PBR.Metallic")
    );
    if (metallicProp && !metallicProp->isNull()) {
        data.metallic = metallicProp->getFloat();
    }

    // Roughness
    auto roughnessProp = mat.getAppearanceProperty(
        QStringLiteral("Render.Substance_PBR.Roughness")
    );
    if (roughnessProp && !roughnessProp->isNull()) {
        data.roughness = roughnessProp->getFloat();
    }

    // Specular
    auto specularProp = mat.getAppearanceProperty(
        QStringLiteral("Render.Substance_PBR.Specular")
    );
    if (specularProp && !specularProp->isNull()) {
        data.specular = specularProp->getFloat();
    }

    // Normal
    auto normalProp = mat.getAppearanceProperty(
        QStringLiteral("Render.Substance_PBR.Normal")
    );
    if (normalProp && !normalProp->isNull()) {
        data.normalTexture = normalProp->getURL();
    }

    // Opacity
    auto opacityProp = mat.getAppearanceProperty(
        QStringLiteral("Render.Substance_PBR.Opacity")
    );
    if (opacityProp && !opacityProp->isNull()) {
        data.opacity = opacityProp->getFloat();
    }

    // BaseColor texture
    auto baseColorTexProp = mat.getAppearanceProperty(
        QStringLiteral("Render.Substance_PBR.BaseColorTexture")
    );
    if (baseColorTexProp && !baseColorTexProp->isNull()) {
        data.baseColorTexture = baseColorTexProp->getURL();
    }

    // Metallic texture
    auto metallicTexProp = mat.getAppearanceProperty(
        QStringLiteral("Render.Substance_PBR.MetallicTexture")
    );
    if (metallicTexProp && !metallicTexProp->isNull()) {
        data.metallicTexture = metallicTexProp->getURL();
    }

    // Roughness texture
    auto roughnessTexProp = mat.getAppearanceProperty(
        QStringLiteral("Render.Substance_PBR.RoughnessTexture")
    );
    if (roughnessTexProp && !roughnessTexProp->isNull()) {
        data.roughnessTexture = roughnessTexProp->getURL();
    }

    return data;
}

App::Material RenderMaterial::toAppMaterial() const
{
    App::Material mat;

    // Map base color to diffuse
    mat.diffuseColor.set(
        static_cast<float>(m_data.baseColor.x),
        static_cast<float>(m_data.baseColor.y),
        static_cast<float>(m_data.baseColor.z)
    );

    // Map specular: distribute luminance across RGB channels
    auto specVal = static_cast<float>(m_data.specular);
    mat.specularColor.set(specVal, specVal, specVal);

    // Map emissive
    mat.emissiveColor.set(
        static_cast<float>(m_data.emissive.x),
        static_cast<float>(m_data.emissive.y),
        static_cast<float>(m_data.emissive.z)
    );

    // Map roughness back to shininess (invert)
    mat.shininess = static_cast<float>((1.0 - m_data.roughness) * 100.0);

    // Map opacity back to transparency
    mat.transparency = static_cast<float>(1.0 - m_data.opacity);

    // Map base color texture back to image path
    if (!m_data.baseColorTexture.isEmpty()) {
        mat.imagePath = m_data.baseColorTexture.toStdString();
    }

    // Set material type based on metallic
    if (m_data.metallic >= 0.5) {
        mat.setType(App::Material::STEEL);
    }
    else {
        mat.setType(App::Material::PLASTIC);
    }

    return mat;
}

const RenderMaterialData& RenderMaterial::data() const
{
    return m_data;
}

void RenderMaterial::setData(const RenderMaterialData& data)
{
    m_data = data;
}

bool RenderMaterial::isMetallicType(App::Material::MaterialType type)
{
    switch (type)
    {
        case App::Material::BRASS:
        case App::Material::BRONZE:
        case App::Material::COPPER:
        case App::Material::GOLD:
        case App::Material::PEWTER:
        case App::Material::SILVER:
        case App::Material::STEEL:
        case App::Material::CHROME:
        case App::Material::ALUMINIUM:
        case App::Material::METALIZED:
            return true;
        default:
            return false;
    }
}

double RenderMaterial::colorLuminance(const Base::Color& color)
{
    // ITU-R BT.709 luminance weights
    return 0.2126 * static_cast<double>(color.r)
         + 0.7152 * static_cast<double>(color.g)
         + 0.0722 * static_cast<double>(color.b);
}

}  // namespace Render
