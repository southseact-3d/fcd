// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten CAD contributors                          *
 *                                                                         *
 *   This file is part of the FreeCAD CAx development system.              *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library  is distributed in the hope that it will be useful,      *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Library General Public License for more details.                  *
 *                                                                         *
 *   Contributors:                                                         *
 *                                                                         *
 ***************************************************************************/

#include "PrismaticReconstruction.h"

#include <algorithm>
#include <cmath>
#include <map>
#include <set>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_Sewing.hxx>

#include <Base/Console.h>
#include <Base/Converter.h>
#include <Base/Vector3D.h>
#include <Mod/Mesh/App/Core/Algorithm.h>
#include <Mod/Mesh/App/Core/Grid.h>
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <Mod/Mesh/App/Core/Segmentation.h>


using namespace MeshPart;

TopoDS_Shape PrismaticReconstruction::reconstruct(
    const MeshCore::MeshKernel& mesh,
    const Params& params)
{
    Base::Console().message("PrismaticReconstruction: starting reconstruction\n");

    // Step 1: Segment mesh into surface regions
    std::vector<Region> regions = segmentMesh(mesh);
    Base::Console().message("PrismaticReconstruction: found %d surface regions\n", (int)regions.size());

    // Step 2: Determine loft direction
    Base::Vector3f direction;
    if (params.autoDetectDirection) {
        direction = detectPrimaryDirection(regions);
        Base::Console().message(
            "PrismaticReconstruction: auto-detected direction (%.3f, %.3f, %.3f)\n",
            direction.x, direction.y, direction.z
        );
    }
    else {
        direction = Base::convertTo<Base::Vector3f>(params.direction);
    }

    // Step 3: Compute slice positions along the direction
    std::vector<float> slicePositions = computeSlicePositions(mesh, direction, params.sectionCount);
    Base::Console().message("PrismaticReconstruction: %d slice positions\n", (int)slicePositions.size());

    // Step 4: Extract cross-sections
    std::vector<TopoDS_Shape> sections = extractCrossSections(mesh, direction, slicePositions);
    Base::Console().message("PrismaticReconstruction: %d cross-sections extracted\n", (int)sections.size());

    if (sections.size() < 2) {
        Base::Console().warning("PrismaticReconstruction: not enough cross-sections for loft\n");
        return {};
    }

    // Step 5: Fit curves and loft
    TopoDS_Shape result = fitAndLoftSections(sections, true, params.ruled);

    if (result.IsNull()) {
        Base::Console().warning("PrismaticReconstruction: loft failed\n");
        return {};
    }

    Base::Console().message("PrismaticReconstruction: reconstruction complete\n");
    return result;
}

std::vector<PrismaticReconstruction::Region> PrismaticReconstruction::segmentMesh(
    const MeshCore::MeshKernel& mesh)
{
    std::vector<Region> regions;

    // Use planar segmentation to identify flat regions
    MeshCore::MeshDistancePlanarSegment segmentor(mesh, 10, 0.5f);
    std::vector<MeshCore::MeshSurfaceSegmentPtr> segments;
    segments.push_back(std::make_shared<MeshCore::MeshDistancePlanarSegment>(mesh, 10, 0.5f));

    MeshCore::MeshSegmentAlgorithm algo(mesh);
    algo.FindSegments(segments);

    for (const auto& seg : segments) {
        const auto& segIndices = seg->GetSegments();
        for (const auto& facetIndices : segIndices) {
            if (facetIndices.size() < 10) {
                continue;  // skip tiny regions
            }

            Region region;
            region.facets = facetIndices;
            region.count = static_cast<int>(facetIndices.size());

            // Compute centroid and average normal
            Base::Vector3f center(0, 0, 0);
            Base::Vector3f normal(0, 0, 0);
            for (auto idx : facetIndices) {
                const auto& facet = mesh.GetFacet(idx);
                center += facet._aclPoints[0];
                center += facet._aclPoints[1];
                center += facet._aclPoints[2];

                Base::Vector3f n = facet.GetNormal();
                normal += n;
            }
            float n = static_cast<float>(facetIndices.size() * 3);
            center /= n;
            normal.Normalize();

            region.center = center;
            region.normal = normal;
            regions.push_back(region);
        }
    }

    // If segmentation found nothing useful, create a single region for the whole mesh
    if (regions.empty()) {
        Region region;
        region.count = mesh.CountFacets();
        Base::Vector3f center(0, 0, 0);
        Base::Vector3f normal(0, 0, 1);
        unsigned long count = 0;

        for (unsigned long i = 0; i < mesh.CountFacets(); i++) {
            region.facets.push_back(i);
            const auto& facet = mesh.GetFacet(i);
            center += facet._aclPoints[0];
            center += facet._aclPoints[1];
            center += facet._aclPoints[2];
            count += 3;

            Base::Vector3f n = facet.GetNormal();
            normal += n;
        }
        center /= static_cast<float>(count);
        normal.Normalize();

        region.center = center;
        region.normal = normal;
        regions.push_back(region);
    }

    return regions;
}

Base::Vector3f PrismaticReconstruction::detectPrimaryDirection(
    const std::vector<Region>& regions)
{
    // Cluster regions by normal direction and find the dominant cluster
    if (regions.empty()) {
        return Base::Vector3f(0, 0, 1);
    }

    // Simple approach: find the most common normal direction
    // by averaging normals weighted by region size
    Base::Vector3f avgNormal(0, 0, 0);
    float totalWeight = 0;

    for (const auto& region : regions) {
        float weight = static_cast<float>(region.count);
        avgNormal += region.normal * weight;
        totalWeight += weight;
    }

    if (totalWeight > 0) {
        avgNormal /= totalWeight;
    }

    // The loft direction should be perpendicular to the average normal
    // (since cross-sections are cut perpendicular to the loft direction)
    // Find the axis most aligned with the normal
    Base::Vector3f dirs[3] = {
        Base::Vector3f(1, 0, 0),
        Base::Vector3f(0, 1, 0),
        Base::Vector3f(0, 0, 1)
    };

    float maxDot = 0;
    Base::Vector3f primaryDir = dirs[0];

    for (auto& d : dirs) {
        float dot = std::abs(avgNormal * d);
        if (dot > maxDot) {
            maxDot = dot;
            primaryDir = d;
        }
    }

    return primaryDir;
}

std::vector<float> PrismaticReconstruction::computeSlicePositions(
    const MeshCore::MeshKernel& mesh,
    const Base::Vector3f& direction,
    int count)
{
    // Compute bounding box extent along the direction
    float minProj = std::numeric_limits<float>::max();
    float maxProj = std::numeric_limits<float>::lowest();

    for (unsigned long i = 0; i < mesh.CountPoints(); i++) {
        Base::Vector3f pt = mesh.GetPoint(i);
        float proj = pt * direction;
        minProj = std::min(minProj, proj);
        maxProj = std::max(maxProj, proj);
    }

    // Add small margin
    float margin = (maxProj - minProj) * 0.01f;
    minProj -= margin;
    maxProj += margin;

    std::vector<float> positions;
    for (int i = 0; i < count; i++) {
        float t = static_cast<float>(i) / static_cast<float>(count - 1);
        positions.push_back(minProj + t * (maxProj - minProj));
    }

    return positions;
}

std::vector<TopoDS_Shape> PrismaticReconstruction::extractCrossSections(
    const MeshCore::MeshKernel& mesh,
    const Base::Vector3f& direction,
    const std::vector<float>& positions)
{
    std::vector<TopoDS_Shape> sections;

    MeshCore::MeshFacetGrid grid(mesh);

    for (float pos : positions) {
        Base::Vector3f point = direction * pos;
        MeshCore::MeshAlgorithm algo(mesh);

        std::list<std::vector<Base::Vector3f>> polylines;
        algo.CutWithPlane(point, direction, grid, polylines, 0.001f, true);

        if (polylines.empty()) {
            continue;
        }

        // Convert polylines to BRep wires and create a face
        TopoDS_Compound comp;
        BRep_Builder builder;
        builder.MakeCompound(comp);

        for (const auto& polyline : polylines) {
            if (polyline.size() < 3) {
                continue;
            }

            BRepBuilderAPI_MakePolygon mkPoly;
            for (const auto& pt : polyline) {
                mkPoly.Add(gp_Pnt(pt.x, pt.y, pt.z));
            }
            mkPoly.Close();

            if (mkPoly.IsDone()) {
                TopoDS_Wire wire = mkPoly.Wire();
                try {
                    TopoDS_Face face = BRepBuilderAPI_MakeFace(wire);
                    if (!face.IsNull()) {
                        builder.Add(comp, face);
                    }
                }
                catch (...) {
                    // If face creation fails, just add the wire
                    builder.Add(comp, wire);
                }
            }
        }

        if (!comp.IsNull()) {
            sections.push_back(comp);
        }
    }

    return sections;
}

TopoDS_Shape PrismaticReconstruction::fitAndLoftSections(
    const std::vector<TopoDS_Shape>& sections,
    bool solid,
    bool ruled)
{
    if (sections.size() < 2) {
        return {};
    }

    // Try lofting using ThruSections
    try {
        BRepOffsetAPI_ThruSections lofter(solid, ruled);

        for (const auto& section : sections) {
            // Extract the first wire from each section
            TopoDS_Wire wire;
            for (TopExp_Explorer exp(section, TopAbs_WIRE); exp.More(); exp.Next()) {
                wire = TopoDS::Wire(exp.Current());
                break;
            }

            if (wire.IsNull()) {
                // Try to extract wire from face
                for (TopExp_Explorer exp(section, TopAbs_FACE); exp.More(); exp.Next()) {
                    TopoDS_Face face = TopoDS::Face(exp.Current());
                    TopExp_Explorer wireExp(face, TopAbs_WIRE);
                    if (wireExp.More()) {
                        wire = TopoDS::Wire(wireExp.Current());
                        break;
                    }
                }
            }

            if (!wire.IsNull()) {
                lofter.AddWire(wire);
            }
        }

        lofter.Build();
        if (lofter.IsDone()) {
            TopoDS_Shape result = lofter.Shape();
            if (!result.IsNull()) {
                return result;
            }
        }
    }
    catch (const Standard_Failure& e) {
        Base::Console().warning("PrismaticReconstruction: ThruSections failed: %s\n",
            e.GetMessageString());
    }
    catch (...) {
        Base::Console().warning("PrismaticReconstruction: ThruSections failed with unknown error\n");
    }

    // Fallback: try to create a prism from the first section
    if (!sections.empty()) {
        try {
            TopoDS_Shape firstSection = sections.front();

            // Get the bounding box extent for prism height
            Bnd_Box bbox;
            BRepBndLib::Add(firstSection, bbox);
            Standard_Real xMin, yMin, zMin, xMax, yMax, zMax;
            bbox.Get(xMin, yMin, zMin, xMax, yMax, zMax);

            // Create a simple extrusion
            gp_Vec extrudeVec(0, 0, zMax - zMin);
            BRepPrimAPI_MakePrism prismMaker(
                BRepBuilderAPI_MakeFace(
                    TopoDS::Wire(TopExp_Explorer(firstSection, TopAbs_WIRE).Current())
                ).Face(),
                extrudeVec
            );

            if (prismMaker.IsDone()) {
                return prismMaker.Shape();
            }
        }
        catch (...) {
            Base::Console().warning("PrismaticReconstruction: prism fallback failed\n");
        }
    }

    return {};
}

TopoDS_Shape PrismaticReconstruction::fuseSolids(const std::vector<TopoDS_Shape>& solids)
{
    if (solids.empty()) {
        return {};
    }

    if (solids.size() == 1) {
        return solids.front();
    }

    TopoDS_Shape result = solids.front();
    for (size_t i = 1; i < solids.size(); i++) {
        try {
            BRepAlgoAPI_Fuse fuse(result, solids[i]);
            if (fuse.IsDone()) {
                result = fuse.Shape();
            }
        }
        catch (...) {
            Base::Console().warning("PrismaticReconstruction: boolean fuse failed\n");
        }
    }

    return result;
}
