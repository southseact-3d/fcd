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

#include "ShadingSystem.h"
#include <cmath>
#include <algorithm>

namespace Render
{

static constexpr double PI = 3.14159265358979323846;

void ShadingSystem::buildCoordinateSystem(const Base::Vector3d& normal,
                                          Base::Vector3d& tangent,
                                          Base::Vector3d& bitangent)
{
    if (std::abs(normal.x) > std::abs(normal.z)) {
        double invLen = 1.0 / std::sqrt(normal.x * normal.x + normal.y * normal.y);
        tangent = Base::Vector3d(-normal.y * invLen, normal.x * invLen, 0.0);
    }
    else {
        double invLen = 1.0 / std::sqrt(normal.y * normal.y + normal.z * normal.z);
        tangent = Base::Vector3d(0.0, -normal.z * invLen, normal.y * invLen);
    }
    bitangent = normal.Cross(tangent);
}

double ShadingSystem::computeDielectricF0(double ior)
{
    double r = (ior - 1.0) / (ior + 1.0);
    return r * r;
}

double ShadingSystem::ggxD(const Base::Vector3d& normal, const Base::Vector3d& halfVec, double alpha)
{
    double nDotH = std::max(0.0, normal.Dot(halfVec));
    double a2 = alpha * alpha;
    double denom = nDotH * nDotH * (a2 - 1.0) + 1.0;
    return a2 / (PI * denom * denom + 1e-30);
}

double ShadingSystem::smithG1(const Base::Vector3d& normal, const Base::Vector3d& direction, double k)
{
    double nDotDir = std::max(0.0, normal.Dot(direction));
    return nDotDir / (nDotDir * (1.0 - k) + k + 1e-30);
}

double ShadingSystem::smithG(const Base::Vector3d& normal, const Base::Vector3d& wo,
                             const Base::Vector3d& wi, double k)
{
    return smithG1(normal, wo, k) * smithG1(normal, wi, k);
}

Base::Vector3d ShadingSystem::schlickFresnel(const Base::Vector3d& f0, double cosTheta)
{
    double t = 1.0 - std::max(0.0, cosTheta);
    double t2 = t * t;
    double t5 = t2 * t2 * t;
    return f0 + (Base::Vector3d(1.0, 1.0, 1.0) - f0) * t5;
}

double ShadingSystem::powerHeuristic(double pdfA, double pdfB)
{
    double a2 = pdfA * pdfA;
    double b2 = pdfB * pdfB;
    return a2 / (a2 + b2 + 1e-30);
}

ShadingResult ShadingSystem::evaluate(const ShadingParams& params,
                                      const Base::Vector3d& wo,
                                      const Base::Vector3d& wi) const
{
    ShadingResult result;
    result.isEmissive = (params.emissive.x > 0.0 || params.emissive.y > 0.0 || params.emissive.z > 0.0);

    Base::Vector3d n = params.normal;
    double nDotWo = std::max(0.0, n.Dot(wo));
    double nDotWi = std::max(0.0, n.Dot(wi));

    if (nDotWo <= 0.0 || nDotWi <= 0.0) {
        result.brdf = Base::Vector3d(0.0, 0.0, 0.0);
        result.pdf = 0.0;
        return result;
    }

    Base::Vector3d halfVec = (wo + wi);
    double halfLen = halfVec.Length();
    if (halfLen < 1e-10) {
        result.brdf = Base::Vector3d(0.0, 0.0, 0.0);
        result.pdf = 0.0;
        return result;
    }
    halfVec = halfVec / halfLen;

    double alpha = params.roughness * params.roughness;
    alpha = std::max(alpha, 0.001);

    double k = (params.roughness + 1.0) * (params.roughness + 1.0) / 8.0;

    // GGX Normal Distribution
    double D = ggxD(n, halfVec, alpha);

    // Smith Geometry
    double G = smithG(n, wo, wi, k);

    // Fresnel
    double nDotH = std::max(0.0, n.Dot(halfVec));
    double nDotWoAbs = nDotWo;

    // F0 for dielectrics based on specular
    double dielectricF0 = 0.16 * params.specular * params.specular;
    Base::Vector3d f0Dielectric(dielectricF0, dielectricF0, dielectricF0);
    Base::Vector3d f0Metal = params.baseColor;
    Base::Vector3d f0 = f0Dielectric * (1.0 - params.metallic) + f0Metal * params.metallic;

    Base::Vector3d F = schlickFresnel(f0, nDotWoAbs);

    // Specular BRDF: D * G * F / (4 * nDotWo * nDotWi)
    Base::Vector3d specular = F * (D * G / (4.0 * nDotWo * nDotWi + 1e-30));

    // Diffuse: Lambertian for dielectrics, zero for metals (energy conservation)
    Base::Vector3d diffuse(0.0, 0.0, 0.0);
    if (params.metallic < 1.0) {
        Base::Vector3d diffuseColor = params.baseColor * (1.0 - params.metallic);
        Base::Vector3d kD = Base::Vector3d(1.0, 1.0, 1.0) - F;
        diffuse = kD * (diffuseColor / PI);
    }

    result.brdf = diffuse + specular;
    result.pdf = nDotWi / PI;

    return result;
}

Base::Vector3d ShadingSystem::sampleBSDF(const ShadingParams& params,
                                         const Base::Vector3d& wo,
                                         double& pdf,
                                         Base::Vector3d& tangent,
                                         Base::Vector3d& bitangent) const
{
    Base::Vector3d n = params.normal;
    buildCoordinateSystem(n, tangent, bitangent);

    double alpha = params.roughness * params.roughness;
    alpha = std::max(alpha, 0.001);

    double nDotWo = std::max(0.0, n.Dot(wo));

    // Compute F0
    double dielectricF0 = 0.16 * params.specular * params.specular;
    Base::Vector3d f0Dielectric(dielectricF0, dielectricF0, dielectricF0);
    Base::Vector3d f0Metal = params.baseColor;
    Base::Vector3d f0 = f0Dielectric * (1.0 - params.metallic) + f0Metal * params.metallic;

    // Average Fresnel for importance sampling decision
    double Favg = (f0.x + f0.y + f0.z) / 3.0;
    double nDotHForF = nDotWo;
    double t = 1.0 - nDotHForF;
    double fresnelWeight = Favg + (1.0 - Favg) * t * t * t * t * t;

    // Random numbers
    double r1 = static_cast<double>(rand()) / RAND_MAX;
    double r2 = static_cast<double>(rand()) / RAND_MAX;

    // Decide specular vs diffuse based on Fresnel
    if (r1 < fresnelWeight && params.metallic < 1.0) {
        // GGX importance sampling
        double phi = 2.0 * PI * r2;
        double cosTheta2 = (1.0 - r1) / (1.0 + (alpha * alpha - 1.0) * r1 + 1e-30);
        cosTheta2 = std::min(1.0, std::max(0.0, cosTheta2));
        double cosTheta = std::sqrt(cosTheta2);
        double sinTheta = std::sqrt(1.0 - cosTheta2);

        Base::Vector3d halfVec(
            std::cos(phi) * sinTheta,
            std::sin(phi) * sinTheta,
            cosTheta
        );
        // Transform to world space
        Base::Vector3d halfVecWorld(
            halfVec.x * tangent.x + halfVec.y * bitangent.x + halfVec.z * n.x,
            halfVec.x * tangent.y + halfVec.y * bitangent.y + halfVec.z * n.y,
            halfVec.x * tangent.z + halfVec.y * bitangent.z + halfVec.z * n.z
        );

        // Reflect wo around halfVec
        Base::Vector3d wi = wo * (-2.0) * n.Dot(halfVecWorld) + halfVecWorld * 2.0;
        // Actually: wi = reflect(-wo, halfVec) but in our convention wo is outgoing
        // wi = 2 * (wo . halfVec) * halfVec - wo
        double woDotH = wo.Dot(halfVecWorld);
        wi = halfVecWorld * (2.0 * woDotH) - wo;

        // Check if below surface
        if (n.Dot(wi) <= 0.0) {
            pdf = 0.0;
            return Base::Vector3d(0.0, 0.0, 0.0);
        }

        // PDF for GGX specular sampling
        double D = ggxD(n, halfVecWorld, alpha);
        pdf = D * std::max(0.0, n.Dot(halfVecWorld)) / (4.0 * woDotH + 1e-30);
        pdf = fresnelWeight * pdf;

        return wi;
    }
    else {
        // Cosine-weighted hemisphere sampling for diffuse
        double phi = 2.0 * PI * r2;
        double cosTheta = std::sqrt(r1);
        double sinTheta = std::sqrt(1.0 - r1);

        Base::Vector3d wi(
            std::cos(phi) * sinTheta,
            std::sin(phi) * sinTheta,
            cosTheta
        );
        Base::Vector3d wiWorld(
            wi.x * tangent.x + wi.y * bitangent.x + wi.z * n.x,
            wi.x * tangent.y + wi.y * bitangent.y + wi.z * n.y,
            wi.x * tangent.z + wi.y * bitangent.z + wi.z * n.z
        );

        pdf = (1.0 - fresnelWeight) * n.Dot(wiWorld) / PI;
        return wiWorld;
    }
}

ShadingParams ShadingSystem::fromAppMaterial(const App::Material& mat)
{
    ShadingParams params;

    // FreeCAD Base::Color uses float components [0,1]
    params.baseColor = Base::Vector3d(
        static_cast<double>(mat.diffuseColor.r),
        static_cast<double>(mat.diffuseColor.g),
        static_cast<double>(mat.diffuseColor.b)
    );

    params.emissive = Base::Vector3d(
        static_cast<double>(mat.emissiveColor.r),
        static_cast<double>(mat.emissiveColor.g),
        static_cast<double>(mat.emissiveColor.b)
    );

    params.specular = static_cast<double>(mat.shininess) / 128.0;
    params.specular = std::min(1.0, std::max(0.0, params.specular));

    params.opacity = 1.0 - static_cast<double>(mat.transparency);
    params.opacity = std::min(1.0, std::max(0.0, params.opacity));

    params.metallic = 0.0;
    params.roughness = 0.5;

    switch (mat.getType()) {
        case App::Material::GOLD:
        case App::Material::COPPER:
        case App::Material::SILVER:
        case App::Material::ALUMINIUM:
            params.metallic = 1.0;
            params.roughness = 0.2;
            break;
        case App::Material::CHROME:
            params.metallic = 1.0;
            params.roughness = 0.05;
            break;
        case App::Material::BRASS:
        case App::Material::BRONZE:
            params.metallic = 1.0;
            params.roughness = 0.35;
            break;
        case App::Material::PEWTER:
            params.metallic = 1.0;
            params.roughness = 0.4;
            break;
        case App::Material::STEEL:
            params.metallic = 1.0;
            params.roughness = 0.3;
            break;
        case App::Material::PLASTIC:
        case App::Material::SHINY_PLASTIC:
            params.metallic = 0.0;
            params.roughness = 0.25;
            break;
        case App::Material::SATIN:
            params.metallic = 0.0;
            params.roughness = 0.6;
            break;
        case App::Material::STONE:
        case App::Material::PLASTER:
            params.metallic = 0.0;
            params.roughness = 0.8;
            break;
        default:
            break;
    }

    return params;
}

}  // namespace Render
