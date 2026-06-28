// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 FreeCAD Development Team                           *
 *                                                                         *
 *   This file is part of FreeCAD.                                         *
 *                                                                         *
 *   FreeCAD is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as           *
 *   published by the Free Software Foundation, either version 2.1 of      *
 *   the License, or (at your option) any later version.                   *
 *                                                                         *
 *   FreeCAD is distributed in the hope that it will be useful, but        *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU      *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with FreeCAD. If not, see                               *
 *   <https://www.gnu.org/licenses/>.                                      *
 ***************************************************************************/


#include <cmath>
#include <random>

#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <BRepGProp.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <GProp_GProps.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Solid.hxx>

#include <Base/Console.h>
#include <Base/Exception.h>
#include <Mod/Part/App/TopoShape.h>

#include "FeatureStoneTexture.h"

FC_LOG_LEVEL_INIT("PartDesign", true, true)

using namespace PartDesign;

PROPERTY_SOURCE(PartDesign::StoneTexture, PartDesign::DressUp)

const App::PropertyQuantityConstraint::Constraints StoneTexture::floatSize = {0.1, 10000.0, 1.0};
const App::PropertyIntegerConstraint::Constraints StoneTexture::intPercent = {0, 100, 1};

const char* StoneTexture::ScaleEnums[]
    = {"1:1", "1:2", "1:5", "1:10", "1:20", "1:50", "1:76", "1:100", "1:200", "1:500", "Custom", nullptr};

const char* StoneTexture::PatternEnums[]
    = {"Ashlar", "Rubble", "Fieldstone", nullptr};

const char* StoneTexture::StoneScaleEnums[]
    = {"Small", "Medium", "Large", nullptr};

StoneTexture::StoneTexture()
{
    ADD_PROPERTY_TYPE(
        StonePattern,
        (0L),
        "Stone",
        App::Prop_None,
        "Stone pattern type"
    );
    StonePattern.setEnums(PatternEnums);

    ADD_PROPERTY_TYPE(
        StoneScale,
        (1L),
        "Stone",
        App::Prop_None,
        "Stone size preset"
    );
    StoneScale.setEnums(StoneScaleEnums);

    ADD_PROPERTY_TYPE(
        StoneSize,
        (150.0),
        "Stone",
        App::Prop_None,
        "Average stone size"
    );
    StoneSize.setUnit(Base::Unit::Length);
    StoneSize.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        StoneDepth,
        (10.0),
        "Stone",
        App::Prop_None,
        "Protrusion depth of stones above the face"
    );
    StoneDepth.setUnit(Base::Unit::Length);
    StoneDepth.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        DepthVariation,
        (15),
        "Stone",
        App::Prop_None,
        "Random variation in depth (+/-)"
    );
    DepthVariation.setConstraints(&intPercent);

    ADD_PROPERTY_TYPE(
        Roughness,
        (50),
        "Stone",
        App::Prop_None,
        "Shape irregularity (0% = rectangular, 100% = very irregular)"
    );
    Roughness.setConstraints(&intPercent);

    ADD_PROPERTY_TYPE(
        MortarThickness,
        (5.0),
        "Stone",
        App::Prop_None,
        "Thickness of mortar joints between stones"
    );
    MortarThickness.setUnit(Base::Unit::Length);
    MortarThickness.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        MortarDepth,
        (3.0),
        "Stone",
        App::Prop_None,
        "Recession depth of mortar joints below the face"
    );
    MortarDepth.setUnit(Base::Unit::Length);
    MortarDepth.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        Seed,
        (0L),
        "Stone",
        App::Prop_None,
        "Random seed (0 = random each time)"
    );

    ADD_PROPERTY_TYPE(
        Scale,
        (0L),
        "Stone",
        App::Prop_None,
        "Scale factor for model-making"
    );
    Scale.setEnums(ScaleEnums);
}

short StoneTexture::mustExecute() const
{
    if (StonePattern.isTouched() || StoneScale.isTouched() || StoneSize.isTouched()
        || StoneDepth.isTouched() || DepthVariation.isTouched() || Roughness.isTouched()
        || MortarThickness.isTouched() || MortarDepth.isTouched() || Seed.isTouched()
        || Scale.isTouched()) {
        return 1;
    }
    return DressUp::mustExecute();
}

static double getScaleFactor(long scaleIndex)
{
    switch (scaleIndex) {
        case 0: return 1.0;      // 1:1
        case 1: return 0.5;      // 1:2
        case 2: return 0.2;      // 1:5
        case 3: return 0.1;      // 1:10
        case 4: return 0.05;     // 1:20
        case 5: return 0.02;     // 1:50
        case 6: return 1.0/76.0; // 1:76
        case 7: return 0.01;     // 1:100
        case 8: return 0.005;    // 1:200
        case 9: return 0.002;    // 1:500
        default: return 1.0;
    }
}

static TopoDS_Shape makeStoneFace(const std::vector<gp_Pnt>& points)
{
    if (points.size() < 3) {
        return TopoDS_Shape();
    }

    BRep_Builder builder;
    TopoDS_Wire wire;
    builder.MakeWire(wire);

    for (size_t i = 0; i < points.size(); ++i) {
        const gp_Pnt& p1 = points[i];
        const gp_Pnt& p2 = points[(i + 1) % points.size()];
        builder.Add(wire, BRepBuilderAPI_MakeEdge(p1, p2).Edge());
    }

    return BRepBuilderAPI_MakeFace(wire).Face();
}

static TopoDS_Shape makeRectFace(const gp_Pnt& origin, const gp_Vec& uDir, const gp_Vec& vDir)
{
    std::vector<gp_Pnt> points = {
        origin,
        origin.XYZ() + uDir.XYZ(),
        origin.XYZ() + uDir.XYZ() + vDir.XYZ(),
        origin.XYZ() + vDir.XYZ()
    };
    return makeStoneFace(points);
}

App::DocumentObjectExecReturn* StoneTexture::execute()
{
    if (onlyHaveRefined()) {
        return App::DocumentObject::StdReturn;
    }

    Part::TopoShape TopShape;
    try {
        TopShape = getBaseTopoShape();
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    if (TopShape.isNull()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "No base shape available")
        );
    }

    // Strip any placement on the base shape so all geometry we build is in
    // world coordinates with Location = identity. Matches the pattern used by
    // Chamfer/Fillet/Draft.
    TopShape.setTransform(Base::Matrix4D());

    auto faces = getFaces(TopShape);
    if (faces.empty()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "No faces selected")
        );
    }

    this->positionByBaseFeature();

    double scaleFactor = getScaleFactor(Scale.getValue());

    long pattern = StonePattern.getValue();
    if (pattern == 0) {
        return executeAshlar(TopShape, faces, scaleFactor);
    }
    else {
        return executeRubble(TopShape, faces, scaleFactor);
    }
}

App::DocumentObjectExecReturn* StoneTexture::executeAshlar(
    const Part::TopoShape& TopShape,
    const std::vector<Part::TopoShape>& faces,
    double scaleFactor)
{
    double stoneS = StoneSize.getValue() * scaleFactor;
    double stoneD = StoneDepth.getValue() * scaleFactor;
    double depthVar = DepthVariation.getValue() / 100.0;
    double roughness = Roughness.getValue() / 100.0;
    double mortarT = MortarThickness.getValue() * scaleFactor;
    double mortarD = MortarDepth.getValue() * scaleFactor;

    if (stoneS <= 0) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Stone size must be positive")
        );
    }
    if (stoneD < 0 || mortarT < 0 || mortarD < 0) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Depth and mortar values must be non-negative")
        );
    }

    // Use seed for reproducible randomness
    unsigned int seed = Seed.getValue();
    if (seed == 0) {
        seed = static_cast<unsigned int>(std::random_device{}());
    }
    std::mt19937 rng(seed);

    // Small overlap so outward-extruded stones share 3D volume with the base
    // solid. Without this, BRepAlgoAPI_Fuse produces a disjoint compound when
    // stone bottoms sit exactly on the base face.
    const double overlap = 10.0 * Precision::Confusion();
    const double stoneExtrudeTotalBase = stoneD + overlap;

    try {
        BRep_Builder builder;
        TopoDS_Compound compound;
        builder.MakeCompound(compound);

        for (auto& faceShape : faces) {
            TopoDS_Face face = TopoDS::Face(faceShape.getShape());

            if (face.IsNull()) {
                continue;
            }

            BRepAdaptor_Surface surface(face);
            if (surface.GetType() != GeomAbs_Plane) {
                FC_WARN(getFullName() << ": skipping non-planar face");
                continue;
            }

            gp_Pln plane = surface.Plane();
            gp_Dir normal = plane.Axis().Direction();

            double uMin = surface.FirstUParameter();
            double uMax = surface.LastUParameter();
            double vMin = surface.FirstVParameter();
            double vMax = surface.LastVParameter();

            gp_Pnt p00 = surface.Value(uMin, vMin);
            gp_Pnt p10 = surface.Value(uMax, vMin);
            gp_Pnt p01 = surface.Value(uMin, vMax);

            gp_Vec uAxis(p00, p10);
            gp_Vec vAxis(p00, p01);
            double faceW = uAxis.Magnitude();
            double faceH = vAxis.Magnitude();

            if (faceW < 1e-7 || faceH < 1e-7) {
                continue;
            }

            uAxis.Normalize();
            vAxis.Normalize();

            // Ashlar uses running bond pattern like bricks
            // Size variation controlled by roughness
            std::uniform_real_distribution<> sizeVar(-roughness * 0.2, roughness * 0.2);

            // Treat degenerate mortar as a tight fit to keep integer math well-behaved.
            double effMortarT = std::max(mortarT, Precision::Confusion());

            int cols = static_cast<int>(std::ceil(faceW / (stoneS + effMortarT)));
            int rows = static_cast<int>(std::ceil(faceH / (stoneS + effMortarT)));

            // Sanity cap to prevent runaway geometry on tiny faces / huge stones.
            constexpr int kMaxStoneGrid = 10000;
            if (cols <= 0 || rows <= 0 || cols > kMaxStoneGrid || rows > kMaxStoneGrid) {
                FC_WARN(getFullName() << ": skipping face, stone grid out of bounds ("
                                      << cols << "x" << rows << ")");
                continue;
            }

            gp_Vec uStep = uAxis * (stoneS + effMortarT);
            gp_Vec vStep = vAxis * (stoneS + effMortarT);
            gp_Vec rowOffVec = uAxis * (0.5 * stoneS);

            std::uniform_real_distribution<> depthDist(-depthVar * stoneD, depthVar * stoneD);

            for (int row = 0; row < rows; row++) {
                gp_Vec rowOffset = (row % 2 == 1) ? rowOffVec : gp_Vec(0, 0, 0);

                for (int col = 0; col < cols; col++) {
                    // Add size variation
                    double varW = stoneS * (1.0 + sizeVar(rng));
                    double varH = stoneS * (1.0 + sizeVar(rng));

                    gp_Vec uStone = uAxis * varW;
                    gp_Vec vStone = vAxis * varH;

                    gp_Pnt stoneOrigin(p00.XYZ()
                        + uStep.XYZ() * col
                        + vStep.XYZ() * row
                        + rowOffset.XYZ());
                    TopoDS_Shape stone2D = makeRectFace(stoneOrigin, uStone, vStone);

                    try {
                        BRepAlgoAPI_Common common(face, stone2D);
                        if (!common.IsDone()) {
                            continue;
                        }
                        TopoDS_Shape clipped = common.Shape();

                        if (clipped.IsNull()) {
                            continue;
                        }
                        {
                            TopExp_Explorer anExplorer(clipped, TopAbs_FACE);
                            if (!anExplorer.More()) {
                                continue;
                            }
                        }

                        // Translate clipped profile down by `overlap` so its
                        // bottom sits inside the base solid, then extrude
                        // by `stoneD + overlap` outward. This guarantees a
                        // 3D overlap with the base for a single fused solid.
                        double extrudeD = stoneD + depthDist(rng);
                        gp_Trsf translation;
                        translation.SetTranslation(gp_Vec(normal) * (-overlap));
                        TopoDS_Shape shiftedClipped = clipped.Moved(translation);
                        gp_Vec extrudeVec(normal);
                        extrudeVec.Multiply(extrudeD + overlap);
                        TopoDS_Shape stone3D =
                            BRepPrimAPI_MakePrism(shiftedClipped, extrudeVec).Shape();

                        builder.Add(compound, stone3D);
                    }
                    catch (Standard_Failure&) {
                        continue;
                    }
                }
            }

            // Generate mortar joints
            if (mortarD > 0) {
                // Horizontal mortar strips
                for (int row = 0; row <= rows; row++) {
                    double mortarOffsetFrac = 0.0;
                    if (row == 0) {
                        mortarOffsetFrac = -1.0;
                    }
                    else if (row == rows) {
                        mortarOffsetFrac = 1.0;
                    }

                    gp_Pnt mortarOrigin(p00.XYZ()
                        + vStep.XYZ() * row
                        - vAxis.XYZ() * (mortarT / 2.0)
                        + mortarOffsetFrac * vAxis.XYZ() * (mortarT / 2.0)
                        - uAxis.XYZ() * stoneD);

                    gp_Vec mortarWidth = uAxis * (faceW + 2 * stoneD);
                    gp_Vec mortarHeight = vAxis * mortarT;
                    TopoDS_Shape mortar2D = makeRectFace(mortarOrigin, mortarWidth, mortarHeight);

                    try {
                        BRepAlgoAPI_Common common(face, mortar2D);
                        if (!common.IsDone()) {
                            continue;
                        }
                        TopoDS_Shape clipped = common.Shape();

                        if (clipped.IsNull()) {
                            continue;
                        }
                        {
                            TopExp_Explorer anExplorer(clipped, TopAbs_FACE);
                            if (!anExplorer.More()) {
                                continue;
                            }
                        }

                        gp_Vec extrudeVec(normal);
                        extrudeVec.Multiply(-mortarD);
                        TopoDS_Shape mortar3D = BRepPrimAPI_MakePrism(clipped, extrudeVec).Shape();

                        builder.Add(compound, mortar3D);
                    }
                    catch (Standard_Failure&) {
                        continue;
                    }
                }

                // Vertical mortar strips
                for (int row = 0; row < rows; row++) {
                    gp_Vec rowOffset = (row % 2 == 1) ? rowOffVec : gp_Vec(0, 0, 0);

                    for (int col = 0; col <= cols; col++) {
                        gp_Pnt mortarOrigin(p00.XYZ()
                            + uStep.XYZ() * col
                            + vStep.XYZ() * row
                            + rowOffset.XYZ()
                            - uAxis.XYZ() * (mortarT / 2.0));

gp_Vec mortarWidth = uAxis * mortarT;
                            gp_Vec mortarHeight = vAxis * (stoneS + effMortarT);
                        TopoDS_Shape mortar2D = makeRectFace(mortarOrigin, mortarWidth, mortarHeight);

                        try {
                            BRepAlgoAPI_Common common(face, mortar2D);
                            if (!common.IsDone()) {
                                continue;
                            }
                            TopoDS_Shape clipped = common.Shape();

                            if (clipped.IsNull()) {
                                continue;
                            }
                            {
                                TopExp_Explorer anExplorer(clipped, TopAbs_FACE);
                                if (!anExplorer.More()) {
                                    continue;
                                }
                            }

                            gp_Vec extrudeVec(normal);
                            extrudeVec.Multiply(-mortarD);
                            TopoDS_Shape mortar3D = BRepPrimAPI_MakePrism(clipped, extrudeVec).Shape();

                            builder.Add(compound, mortar3D);
                        }
                        catch (Standard_Failure&) {
                            continue;
                        }
                    }
                }
            }
        }

        BRepAlgoAPI_Fuse fuse(TopShape.getShape(), compound);
        if (!fuse.IsDone()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Failed to fuse stones with base shape")
            );
        }

        Part::TopoShape result(fuse.Shape());

        // Enforce single-solid body rule (matches Chamfer/Fillet/Draft).
        if (!isSingleSolidRuleSatisfied(result.getShape())) {
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP(
                "Exception",
                "Result has multiple solids: enable 'Allow Compound' in the active body."
            ));
        }

        result = refineShapeIfActive(result);
        result = getSolid(result);
        this->Shape.setValue(result);
        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure& e) {
        return new App::DocumentObjectExecReturn(e.GetMessageString());
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }
}

App::DocumentObjectExecReturn* StoneTexture::executeRubble(
    const Part::TopoShape& TopShape,
    const std::vector<Part::TopoShape>& faces,
    double scaleFactor)
{
    double stoneS = StoneSize.getValue() * scaleFactor;
    double stoneD = StoneDepth.getValue() * scaleFactor;
    double depthVar = DepthVariation.getValue() / 100.0;
    double roughness = Roughness.getValue() / 100.0;
    double mortarT = MortarThickness.getValue() * scaleFactor;
    double mortarD = MortarDepth.getValue() * scaleFactor;

    if (stoneS <= 0) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Stone size must be positive")
        );
    }
    if (stoneD < 0 || mortarT < 0 || mortarD < 0) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Depth and mortar values must be non-negative")
        );
    }

    // Use seed for reproducible randomness
    unsigned int seed = Seed.getValue();
    if (seed == 0) {
        seed = static_cast<unsigned int>(std::random_device{}());
    }
    std::mt19937 rng(seed);

    // Small overlap so outward-extruded stones share 3D volume with the base
    // solid. Without this, BRepAlgoAPI_Fuse produces a disjoint compound when
    // stone bottoms sit exactly on the base face.
    const double overlap = 10.0 * Precision::Confusion();
    const double stoneExtrudeTotalBase = stoneD + overlap;

    try {
        BRep_Builder builder;
        TopoDS_Compound compound;
        builder.MakeCompound(compound);

        for (auto& faceShape : faces) {
            TopoDS_Face face = TopoDS::Face(faceShape.getShape());

            if (face.IsNull()) {
                continue;
            }

            BRepAdaptor_Surface surface(face);
            if (surface.GetType() != GeomAbs_Plane) {
                FC_WARN(getFullName() << ": skipping non-planar face");
                continue;
            }

            gp_Pln plane = surface.Plane();
            gp_Dir normal = plane.Axis().Direction();

            double uMin = surface.FirstUParameter();
            double uMax = surface.LastUParameter();
            double vMin = surface.FirstVParameter();
            double vMax = surface.LastVParameter();

            gp_Pnt p00 = surface.Value(uMin, vMin);
            gp_Pnt p10 = surface.Value(uMax, vMin);
            gp_Pnt p01 = surface.Value(uMin, vMax);

            gp_Vec uAxis(p00, p10);
            gp_Vec vAxis(p00, p01);
            double faceW = uAxis.Magnitude();
            double faceH = vAxis.Magnitude();

            if (faceW < 1e-7 || faceH < 1e-7) {
                continue;
            }

            uAxis.Normalize();
            vAxis.Normalize();

            // For rubble/fieldstone, use grid-based approach with vertex perturbation
            // Cell size is based on stone size
            double effMortarT = std::max(mortarT, Precision::Confusion());
            double cellSize = stoneS + effMortarT;
            int cols = static_cast<int>(std::ceil(faceW / cellSize));
            int rows = static_cast<int>(std::ceil(faceH / cellSize));

            // Sanity cap to prevent runaway geometry on tiny faces / huge stones.
            constexpr int kMaxStoneGrid = 10000;
            if (cols <= 0 || rows <= 0 || cols > kMaxStoneGrid || rows > kMaxStoneGrid) {
                FC_WARN(getFullName() << ": skipping face, stone grid out of bounds ("
                                      << cols << "x" << rows << ")");
                continue;
            }

            // Perturbation magnitude based on roughness
            double perturb = roughness * cellSize * 0.3;

            std::uniform_real_distribution<> perturbDist(-perturb, perturb);
            std::uniform_real_distribution<> depthDist(-depthVar * stoneD, depthVar * stoneD);

            // Generate perturbed grid vertices
            // We need (cols+1) x (rows+1) vertices
            std::vector<std::vector<gp_Pnt>> vertices(rows + 1, std::vector<gp_Pnt>(cols + 1));

            for (int r = 0; r <= rows; r++) {
                for (int c = 0; c <= cols; c++) {
                    gp_Pnt basePt(p00.XYZ()
                        + uAxis.XYZ() * cellSize * c
                        + vAxis.XYZ() * cellSize * r);

                    // Perturb interior vertices, keep boundary vertices on grid
                    if (r > 0 && r < rows && c > 0 && c < cols) {
                        double pu = perturbDist(rng);
                        double pv = perturbDist(rng);
                        basePt.Translate(uAxis * pu);
                        basePt.Translate(vAxis * pv);
                    }

                    vertices[r][c] = basePt;
                }
            }

            // Generate stone polygons from perturbed grid cells
            std::uniform_real_distribution<> scaleDist(0.85, 1.0);

            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    // Get the four corners of this cell
                    gp_Pnt bl = vertices[r][c];
                    gp_Pnt br = vertices[r][c + 1];
                    gp_Pnt tr = vertices[r + 1][c + 1];
                    gp_Pnt tl = vertices[r + 1][c];

                    // Scale inward to create mortar gaps
                    gp_Vec centerVec = (bl.XYZ() + br.XYZ() + tr.XYZ() + tl.XYZ()) / 4.0
                        - bl.XYZ();
                    double scale = scaleDist(rng);
                    double inset = mortarT * 0.5 * scale;

                    // Compute inset points
                    gp_Vec edgeBL_BR = br.XYZ() - bl.XYZ();
                    gp_Vec edgeBR_TR = tr.XYZ() - br.XYZ();
                    gp_Vec edgeTR_TL = tl.XYZ() - tr.XYZ();
                    gp_Vec edgeTL_BL = bl.XYZ() - tl.XYZ();

                    double lenBL_BR = edgeBL_BR.Magnitude();
                    double lenBR_TR = edgeBR_TR.Magnitude();
                    double lenTR_TL = edgeTR_TL.Magnitude();
                    double lenTL_BL = edgeTL_BL.Magnitude();

                    if (lenBL_BR < 1e-7 || lenBR_TR < 1e-7 || lenTR_TL < 1e-7 || lenTL_BL < 1e-7) {
                        continue;
                    }

                    edgeBL_BR.Normalize();
                    edgeBR_TR.Normalize();
                    edgeTR_TL.Normalize();
                    edgeTL_BL.Normalize();

                    // Inset each vertex toward center
                    gp_Pnt insetBL = bl.XYZ()
                        + edgeBL_BR.XYZ() * inset
                        + edgeTL_BL.XYZ() * (-inset);
                    gp_Pnt insetBR = br.XYZ()
                        + edgeBR_TR.XYZ() * inset
                        + edgeBL_BR.XYZ() * (-inset);
                    gp_Pnt insetTR = tr.XYZ()
                        + edgeTR_TL.XYZ() * inset
                        + edgeBR_TR.XYZ() * (-inset);
                    gp_Pnt insetTL = tl.XYZ()
                        + edgeTL_BL.XYZ() * inset
                        + edgeTR_TL.XYZ() * (-inset);

                    std::vector<gp_Pnt> stonePoints = {insetBL, insetBR, insetTR, insetTL};
                    TopoDS_Shape stone2D = makeStoneFace(stonePoints);

                    if (stone2D.IsNull()) {
                        continue;
                    }

                    try {
                        BRepAlgoAPI_Common common(face, stone2D);
                        if (!common.IsDone()) {
                            continue;
                        }
                        TopoDS_Shape clipped = common.Shape();

                        if (clipped.IsNull()) {
                            continue;
                        }
                        {
                            TopExp_Explorer anExplorer(clipped, TopAbs_FACE);
                            if (!anExplorer.More()) {
                                continue;
                            }
                        }

                        // Translate clipped profile down by `overlap` so its
                        // bottom sits inside the base solid, then extrude
                        // by `stoneD + overlap` outward. Guarantees 3D
                        // overlap with the base for a single fused solid.
                        double extrudeD = stoneD + depthDist(rng);
                        gp_Trsf translation;
                        translation.SetTranslation(gp_Vec(normal) * (-overlap));
                        TopoDS_Shape shiftedClipped = clipped.Moved(translation);
                        gp_Vec extrudeVec(normal);
                        extrudeVec.Multiply(extrudeD + overlap);
                        TopoDS_Shape stone3D =
                            BRepPrimAPI_MakePrism(shiftedClipped, extrudeVec).Shape();

                        builder.Add(compound, stone3D);
                    }
                    catch (Standard_Failure&) {
                        continue;
                    }
                }
            }

            // Generate mortar geometry (the gaps between stones)
            if (mortarD > 0) {
                // Create mortar strips along horizontal grid lines
                for (int r = 0; r <= rows; r++) {
                    for (int c = 0; c < cols; c++) {
                        gp_Pnt left = vertices[r][c];
                        gp_Pnt right = vertices[r][c + 1];

                        gp_Vec edgeVec(left, right);
                        double edgeLen = edgeVec.Magnitude();
                        if (edgeLen < 1e-7) {
                            continue;
                        }
                        edgeVec.Normalize();

                        // Mortar strip along this edge
                        gp_Vec perpVec = vAxis * mortarT;
                        gp_Pnt mOrigin = left.XYZ() - perpVec.XYZ() * 0.5 - edgeVec.XYZ() * stoneD;
                        gp_Vec mLength = edgeVec * (edgeLen + 2 * stoneD);

                        TopoDS_Shape mortar2D = makeRectFace(mOrigin, mLength, perpVec);

                        try {
                            BRepAlgoAPI_Common common(face, mortar2D);
                            if (!common.IsDone()) {
                                continue;
                            }
                            TopoDS_Shape clipped = common.Shape();

                            if (clipped.IsNull()) {
                                continue;
                            }
                            {
                                TopExp_Explorer anExplorer(clipped, TopAbs_FACE);
                                if (!anExplorer.More()) {
                                    continue;
                                }
                            }

                            gp_Vec extrudeVec(normal);
                            extrudeVec.Multiply(-mortarD);
                            TopoDS_Shape mortar3D = BRepPrimAPI_MakePrism(clipped, extrudeVec).Shape();

                            builder.Add(compound, mortar3D);
                        }
                        catch (Standard_Failure&) {
                            continue;
                        }
                    }
                }

                // Create mortar strips along vertical grid lines
                for (int r = 0; r < rows; r++) {
                    for (int c = 0; c <= cols; c++) {
                        gp_Pnt bottom = vertices[r][c];
                        gp_Pnt top = vertices[r + 1][c];

                        gp_Vec edgeVec(bottom, top);
                        double edgeLen = edgeVec.Magnitude();
                        if (edgeLen < 1e-7) {
                            continue;
                        }
                        edgeVec.Normalize();

                        // Mortar strip along this edge
                        gp_Vec perpVec = uAxis * mortarT;
                        gp_Pnt mOrigin = bottom.XYZ() - perpVec.XYZ() * 0.5 - edgeVec.XYZ() * stoneD;
                        gp_Vec mLength = edgeVec * (edgeLen + 2 * stoneD);

                        TopoDS_Shape mortar2D = makeRectFace(mOrigin, perpVec, mLength);

                        try {
                            BRepAlgoAPI_Common common(face, mortar2D);
                            if (!common.IsDone()) {
                                continue;
                            }
                            TopoDS_Shape clipped = common.Shape();

                            if (clipped.IsNull()) {
                                continue;
                            }
                            {
                                TopExp_Explorer anExplorer(clipped, TopAbs_FACE);
                                if (!anExplorer.More()) {
                                    continue;
                                }
                            }

                            gp_Vec extrudeVec(normal);
                            extrudeVec.Multiply(-mortarD);
                            TopoDS_Shape mortar3D = BRepPrimAPI_MakePrism(clipped, extrudeVec).Shape();

                            builder.Add(compound, mortar3D);
                        }
                        catch (Standard_Failure&) {
                            continue;
                        }
                    }
                }
            }
        }

        BRepAlgoAPI_Fuse fuse(TopShape.getShape(), compound);
        if (!fuse.IsDone()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Failed to fuse stones with base shape")
            );
        }

        Part::TopoShape result(fuse.Shape());

        // Enforce single-solid body rule (matches Chamfer/Fillet/Draft).
        if (!isSingleSolidRuleSatisfied(result.getShape())) {
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP(
                "Exception",
                "Result has multiple solids: enable 'Allow Compound' in the active body."
            ));
        }

        result = refineShapeIfActive(result);
        result = getSolid(result);
        this->Shape.setValue(result);
        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure& e) {
        return new App::DocumentObjectExecReturn(e.GetMessageString());
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }
}

void StoneTexture::onChanged(const App::Property* prop)
{
    // Update stone size and roughness based on pattern preset
    if (prop == &StonePattern) {
        long pattern = StonePattern.getValue();
        if (pattern == 0) {
            // Ashlar
            StoneSize.setValue(200.0);
            Roughness.setValue(10);
        }
        else if (pattern == 1) {
            // Rubble
            StoneSize.setValue(150.0);
            Roughness.setValue(50);
        }
        else {
            // Fieldstone
            StoneSize.setValue(100.0);
            Roughness.setValue(80);
        }
    }

    // Update stone size based on scale preset
    if (prop == &StoneScale) {
        long scale = StoneScale.getValue();
        if (scale == 0) {
            StoneSize.setValue(50.0);
        }
        else if (scale == 1) {
            StoneSize.setValue(150.0);
        }
        else {
            StoneSize.setValue(300.0);
        }
    }

    DressUp::onChanged(prop);
}
