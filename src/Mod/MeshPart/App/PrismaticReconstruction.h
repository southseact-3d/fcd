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

#ifndef MESHPART_PRISMATICRECONSTRUCTION_H
#define MESHPART_PRISMATICRECONSTRUCTION_H

#include <vector>

#include <Base/Vector3D.h>
#include <Mod/Mesh/App/Core/MeshKernel.h>
#include <TopoDS_Shape.hxx>

namespace MeshPart
{

class PrismaticReconstruction
{
public:
    struct Params
    {
        Base::Vector3d direction{0, 0, 0};
        bool autoDetectDirection = true;
        int sectionCount = 30;
        bool ruled = false;
        double tolerance = 0.1;
    };

    struct Region
    {
        std::vector<MeshCore::FacetIndex> facets;
        Base::Vector3f normal{0, 0, 1};
        Base::Vector3f center{0, 0, 0};
        int count = 0;
    };

    struct DirectionGroup
    {
        Base::Vector3f direction{0, 0, 1};
        std::vector<Region> regions;
    };

    PrismaticReconstruction() = default;

    TopoDS_Shape reconstruct(
        const MeshCore::MeshKernel& mesh,
        const Params& params
    );

private:
    std::vector<Region> segmentMesh(const MeshCore::MeshKernel& mesh);
    std::vector<DirectionGroup> clusterByDirection(const std::vector<Region>& regions);
    Base::Vector3f detectPrimaryDirection(const std::vector<Region>& regions);
    std::vector<float> computeSlicePositions(
        const MeshCore::MeshKernel& mesh,
        const Base::Vector3f& direction,
        int count
    );
    std::vector<TopoDS_Shape> extractCrossSections(
        const MeshCore::MeshKernel& mesh,
        const Base::Vector3f& direction,
        const std::vector<float>& positions
    );
    TopoDS_Shape fitAndLoftSections(
        const std::vector<TopoDS_Shape>& sections,
        bool solid,
        bool ruled
    );
    TopoDS_Shape fuseSolids(const std::vector<TopoDS_Shape>& solids);
};

}  // namespace MeshPart

#endif  // MESHPART_PRISMATICRECONSTRUCTION_H
