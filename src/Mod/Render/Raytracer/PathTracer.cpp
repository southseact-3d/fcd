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

#include "PathTracer.h"
#include "RenderScene.h"
#include "OutputWriter.h"
#include "RenderSettings.h"

#include <thread>
#include <mutex>
#include <random>
#include <cmath>
#include <algorithm>
#include <atomic>

namespace Render
{

static constexpr double PI = 3.14159265358979323846;
static constexpr int TILE_SIZE = 32;
static constexpr int RUSSIAN_ROLLOUT_DEPTH = 3;
static constexpr double THROUGHPUT_THRESHOLD = 0.001;

static thread_local std::mt19937 rng(42);

static double randomDouble()
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

Base::Vector3d PathTracer::sampleEnvironment(const Base::Vector3d& dir, const RenderScene& scene)
{
    const TextureRT& envMap = scene.environmentMap();
    if (envMap.hasData()) {
        return envMap.sampleEnvironment(dir);
    }

    // Default gradient sky
    double y = dir.y;
    if (y < 0.0) y = 0.0;
    Base::Vector3d skyColor = Base::Vector3d(0.5, 0.7, 1.0) * y
                            + Base::Vector3d(1.0, 1.0, 1.0) * (1.0 - y);
    return skyColor * 0.3;
}

bool PathTracer::isOccluded(const Base::Vector3d& origin, const Base::Vector3d& target,
                            const RenderScene& scene)
{
    Ray shadowRay;
    shadowRay.origin = origin;
    Base::Vector3d dir = target - origin;
    double dist = dir.Length();
    if (dist < 1e-10) return false;
    shadowRay.direction = dir / dist;
    shadowRay.tmin = 1e-4;
    shadowRay.tmax = dist - 1e-4;

    return scene.bvh().intersectAny(shadowRay);
}

Base::Vector3d PathTracer::estimateDirectLight(const HitInfo& hit, const ShadingParams& params,
                                               const Base::Vector3d& wo,
                                               const RenderScene& scene)
{
    Base::Vector3d directLight(0.0, 0.0, 0.0);
    const auto& lights = scene.lights();
    if (lights.empty()) return directLight;

    ShadingSystem shading;

    for (const Light& light : lights) {
        if (light.type == LightType::Environment) continue;

        double u = randomDouble();
        double v = randomDouble();
        LightSample lightSample = sampleLight(light, hit.point, u, v);

        Base::Vector3d wi = lightSample.point - hit.point;
        double dist = wi.Length();
        if (dist < 1e-6) continue;
        wi = wi / dist;

        // Check if light is visible
        if (isOccluded(hit.point + hit.normal * 1e-4, lightSample.point, scene)) {
            continue;
        }

        // Evaluate BSDF
        ShadingResult bsdfResult = shading.evaluate(params, wo, wi);
        if (bsdfResult.pdf <= 0.0) continue;

        double lightPdfVal = lightPDF(light, hit.point, lightSample);
        if (lightPdfVal <= 0.0) continue;

        // MIS: balance heuristic
        double misWeight = 1.0;
        if (light.type == LightType::Area) {
            double bsdfPdf = bsdfResult.pdf;
            double lPdf = lightPdfVal;
            misWeight = lPdf * lPdf / (lPdf * lPdf + bsdfPdf * bsdfPdf + 1e-30);
        }

        double nDotL = std::max(0.0, hit.normal.Dot(wi));
        directLight = directLight + lightSample.radiance * bsdfResult.brdf * nDotL
                     * misWeight / (lightPdfVal + 1e-30);
    }

    return directLight;
}

Base::Vector3d PathTracer::traceRay(const Ray& ray, const RenderScene& scene,
                                    int maxBounces, int depth, Base::Vector3d& normalOut)
{
    if (depth >= maxBounces) {
        return Base::Vector3d(0.0, 0.0, 0.0);
    }

    HitInfo hit;
    hit.t = 1e30;
    if (!scene.bvh().intersect(ray, hit)) {
        normalOut = Base::Vector3d(0.0, 0.0, 1.0);
        return sampleEnvironment(ray.direction, scene);
    }

    normalOut = hit.normal;

    // Get material for this hit
    const auto& materials = scene.materials();
    if (hit.materialIndex >= materials.size()) {
        return Base::Vector3d(0.5, 0.5, 0.5);
    }

    const ShadingParams& params = materials[hit.materialIndex];

    // Check if emissive
    if (params.emissive.x > 0.0 || params.emissive.y > 0.0 || params.emissive.z > 0.0) {
        if (depth == 0) {
            return params.emissive;
        }
        return params.emissive;
    }

    ShadingSystem shading;
    Base::Vector3d wo = -ray.direction;

    // Ensure shading normal faces the right way
    if (wo.Dot(hit.normal) < 0.0) {
        hit.normal = -hit.normal;
    }

    // Accumulate direct light (NEE)
    Base::Vector3d radiance = estimateDirectLight(hit, params, wo, scene);

    // Sample BSDF for indirect illumination
    Base::Vector3d tangent, bitangent;
    double bsdfPdf = 0.0;
    Base::Vector3d wi = shading.sampleBSDF(params, wo, bsdfPdf, tangent, bitangent);

    if (bsdfPdf <= 0.0) {
        return radiance;
    }

    // Evaluate BSDF at sampled direction
    ShadingResult bsdfResult = shading.evaluate(params, wo, wi);
    if (bsdfResult.pdf <= 0.0) {
        return radiance;
    }

    double nDotWi = std::max(0.0, hit.normal.Dot(wi));

    // Compute throughput
    Base::Vector3d throughput = bsdfResult.brdf * (nDotWi / bsdfPdf);

    // Russian roulette
    double maxComponent = std::max(throughput.x, std::max(throughput.y, throughput.z));
    double rrProbability = 1.0;
    if (depth >= RUSSIAN_ROLLOUT_DEPTH) {
        rrProbability = std::min(1.0, maxComponent);
    }

    if (randomDouble() > rrProbability) {
        return radiance;
    }

    // Clamp throughput for numerical stability
    if (maxComponent < THROUGHPUT_THRESHOLD && depth > 0) {
        return radiance;
    }

    // Trace indirect ray
    Ray indirectRay;
    indirectRay.origin = hit.point + hit.normal * 1e-4;
    indirectRay.direction = wi;

    Base::Vector3d dummyNormal;
    Base::Vector3d indirectRadiance = traceRay(indirectRay, scene, maxBounces, depth + 1,
                                               dummyNormal);

    radiance = radiance + throughput * indirectRadiance / rrProbability;

    return radiance;
}

void PathTracer::render(RenderScene& scene, QImage& output,
                        const RenderSettings& settings,
                        std::function<void(float)> progressCallback)
{
    cancelled = false;

    int width = settings.width();
    int height = settings.height();
    int maxSamples = settings.maxSamples();
    int maxBounces = static_cast<int>(settings.maxBounces());

    output = QImage(width, height, QImage::Format_RGB32);
    output.fill(Qt::black);

    // Accumulation buffers
    std::vector<Base::Vector3d> accumBuffer(width * height,
                                            Base::Vector3d(0.0, 0.0, 0.0));
    std::vector<int> sampleCount(width * height, 0);

    const RenderCameraRT& camera = scene.camera();

    // Build tile list
    std::vector<Tile> tiles;
    for (int ty = 0; ty < height; ty += TILE_SIZE) {
        for (int tx = 0; tx < width; tx += TILE_SIZE) {
            Tile tile;
            tile.x = tx;
            tile.y = ty;
            tile.w = std::min(TILE_SIZE, width - tx);
            tile.h = std::min(TILE_SIZE, height - ty);
            tiles.push_back(tile);
        }
    }

    int totalTiles = static_cast<int>(tiles.size());
    std::atomic<int> tilesCompleted{0};
    std::mutex outputMutex;

    // Determine thread count
    int numThreads = std::max(1, static_cast<int>(std::thread::hardware_concurrency()));

    // Progressive rendering: render in passes
    for (int sample = 0; sample < maxSamples; ++sample) {
        if (cancelled) break;

        auto processTile = [&](const Tile& tile) {
            if (cancelled) return;

            thread_local std::mt19937 tileRng(std::random_device{}());

            for (int py = tile.y; py < tile.y + tile.h; ++py) {
                for (int px = tile.x; px < tile.x + tile.w; ++px) {
                    if (cancelled) return;

                    double u = (px + randomDouble()) / width;
                    double v = (py + randomDouble()) / height;
                    double lensU = randomDouble();
                    double lensV = randomDouble();

                    CameraRay camRay;
                    camera.generateRay(u, v, lensU, lensV, camRay);

                    Base::Vector3d normal;
                    Base::Vector3d radiance = traceRay(Ray{camRay.origin, camRay.direction},
                                                      scene, maxBounces, 0, normal);

                    int idx = py * width + px;
                    accumBuffer[idx] = accumBuffer[idx] + radiance;
                    sampleCount[idx]++;

                    // Update pixel with running average
                    Base::Vector3d avg = accumBuffer[idx] * (1.0 / sampleCount[idx]);

                    // Tone map
                    int tmMethod = static_cast<int>(settings.toneMapping());
                    Base::Vector3d tmColor = OutputWriter::toneMap(avg, tmMethod);
                    Base::Vector3d gammaColor = OutputWriter::gammaCorrect(tmColor, 2.2);

                    int r = static_cast<int>(std::min(1.0, std::max(0.0, gammaColor.x)) * 255.0);
                    int g = static_cast<int>(std::min(1.0, std::max(0.0, gammaColor.y)) * 255.0);
                    int b = static_cast<int>(std::min(1.0, std::max(0.0, gammaColor.z)) * 255.0);

                    output.setPixel(px, py, qRgb(r, g, b));
                }
            }
        };

        // Process tiles with thread pool
        std::vector<std::thread> threads;
        std::atomic<size_t> tileIdx{0};

        for (int t = 0; t < numThreads; ++t) {
            threads.emplace_back([&]() {
                while (true) {
                    size_t idx = tileIdx.fetch_add(1);
                    if (idx >= tiles.size()) break;
                    processTile(tiles[idx]);
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        // Report progress
        if (progressCallback) {
            float progress = static_cast<float>(sample + 1) / maxSamples;
            progressCallback(progress);
        }
    }
}

}  // namespace Render
