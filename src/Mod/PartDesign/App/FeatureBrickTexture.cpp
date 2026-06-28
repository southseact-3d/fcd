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
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeHalfSpace.hxx>
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

#include "FeatureBrickTexture.h"

FC_LOG_LEVEL_INIT("PartDesign", true, true)

using namespace PartDesign;

PROPERTY_SOURCE(PartDesign::BrickTexture, PartDesign::DressUp)

const App::PropertyQuantityConstraint::Constraints BrickTexture::floatSize = {0.1, 10000.0, 1.0};
const App::PropertyIntegerConstraint::Constraints BrickTexture::intPercent = {0, 100, 1};

const char* BrickTexture::ScaleEnums[]
    = {"1:1", "1:2", "1:5", "1:10", "1:20", "1:50", "1:76", "1:100", "1:200", "1:500", "Custom", nullptr};

BrickTexture::BrickTexture()
{
    ADD_PROPERTY_TYPE(
        BrickWidth,
        (215.0),
        "Brick",
        App::Prop_None,
        "Width of each brick"
    );
    BrickWidth.setUnit(Base::Unit::Length);
    BrickWidth.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        BrickHeight,
        (65.0),
        "Brick",
        App::Prop_None,
        "Height of each brick"
    );
    BrickHeight.setUnit(Base::Unit::Length);
    BrickHeight.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        BrickDepth,
        (10.0),
        "Brick",
        App::Prop_None,
        "Protrusion depth of bricks above the face"
    );
    BrickDepth.setUnit(Base::Unit::Length);
    BrickDepth.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        MortarThickness,
        (10.0),
        "Brick",
        App::Prop_None,
        "Thickness of mortar joints between bricks"
    );
    MortarThickness.setUnit(Base::Unit::Length);
    MortarThickness.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        MortarDepth,
        (3.0),
        "Brick",
        App::Prop_None,
        "Recession depth of mortar joints below the face"
    );
    MortarDepth.setUnit(Base::Unit::Length);
    MortarDepth.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        RowOffset,
        (50),
        "Brick",
        App::Prop_None,
        "Horizontal offset of alternating rows as percentage of brick width"
    );
    RowOffset.setConstraints(&intPercent);

    ADD_PROPERTY_TYPE(
        Scale,
        (0L),
        "Brick",
        App::Prop_None,
        "Scale factor for model-making"
    );
    Scale.setEnums(ScaleEnums);
}

short BrickTexture::mustExecute() const
{
    if (BrickWidth.isTouched() || BrickHeight.isTouched() || BrickDepth.isTouched()
        || MortarThickness.isTouched() || MortarDepth.isTouched() || RowOffset.isTouched()
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

static TopoDS_Shape makeBrickFace(const gp_Pnt& origin, double width, double height)
{
    gp_Pnt p1(origin);
    gp_Pnt p2(origin.X() + width, origin.Y(), origin.Z());
    gp_Pnt p3(origin.X() + width, origin.Y() + height, origin.Z());
    gp_Pnt p4(origin.X(), origin.Y() + height, origin.Z());

    BRep_Builder builder;
    TopoDS_Wire wire;
    builder.MakeWire(wire);

    auto makeEdge = [](const gp_Pnt& a, const gp_Pnt& b) -> TopoDS_Edge {
        return BRepBuilderAPI_MakeEdge(a, b).Edge();
    };

    builder.Add(wire, makeEdge(p1, p2));
    builder.Add(wire, makeEdge(p2, p3));
    builder.Add(wire, makeEdge(p3, p4));
    builder.Add(wire, makeEdge(p4, p1));

    return BRepBuilderAPI_MakeFace(wire).Face();
}

static TopoDS_Shape makeBrickFace(
    const gp_Pnt& origin, const gp_Vec& uDir, const gp_Vec& vDir)
{
    gp_Pnt p1(origin);
    gp_Pnt p2(origin.XYZ() + uDir.XYZ());
    gp_Pnt p3(origin.XYZ() + uDir.XYZ() + vDir.XYZ());
    gp_Pnt p4(origin.XYZ() + vDir.XYZ());

    BRep_Builder builder;
    TopoDS_Wire wire;
    builder.MakeWire(wire);

    auto makeEdge = [](const gp_Pnt& a, const gp_Pnt& b) -> TopoDS_Edge {
        return BRepBuilderAPI_MakeEdge(a, b).Edge();
    };

    builder.Add(wire, makeEdge(p1, p2));
    builder.Add(wire, makeEdge(p2, p3));
    builder.Add(wire, makeEdge(p3, p4));
    builder.Add(wire, makeEdge(p4, p1));

    return BRepBuilderAPI_MakeFace(wire).Face();
}

App::DocumentObjectExecReturn* BrickTexture::execute()
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
    // Chamfer/Fillet/Draft. Safe even when the body-baked shape already has
    // identity location.
    TopShape.setTransform(Base::Matrix4D());

    auto faces = getFaces(TopShape);
    if (faces.empty()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "No faces selected")
        );
    }

    this->positionByBaseFeature();

    double scaleFactor = getScaleFactor(Scale.getValue());
    double brickW = BrickWidth.getValue() * scaleFactor;
    double brickH = BrickHeight.getValue() * scaleFactor;
    double brickD = BrickDepth.getValue() * scaleFactor;
    double mortarT = MortarThickness.getValue() * scaleFactor;
    double mortarD = MortarDepth.getValue() * scaleFactor;
    double rowOff = RowOffset.getValue() / 100.0;

    if (brickW <= 0 || brickH <= 0) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Brick dimensions must be positive")
        );
    }
    if (mortarT < 0 || brickD < 0 || mortarD < 0) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Mortar and depth values must be non-negative")
        );
    }

    try {
        BRep_Builder builder;
        TopoDS_Compound allShapes;
        builder.MakeCompound(allShapes);

        // Outward brick extrusion starts a small distance below the face plane
        // (overlapping the base solid by `overlap`) and extends `brickD` above
        // the plane. The overlap guarantees the resulting prism shares
        // 3D volume with the base so BRepAlgoAPI_Fuse produces a single
        // connected solid rather than a disjoint compound.
        const double overlap = 10.0 * Precision::Confusion();
        const double brickExtrudeTotal = brickD + overlap;

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

            // Use surface parameterization to get face extent and orientation
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

            // Treat degenerate mortar as a tight fit. This keeps the
            // integer math below well-behaved (no div-by-zero in
            // std::ceil(faceW / (brickW + 0))).
            double effMortarT = std::max(mortarT, Precision::Confusion());

            int cols = static_cast<int>(std::ceil(faceW / (brickW + effMortarT)));
            int rows = static_cast<int>(std::ceil(faceH / (brickH + effMortarT)));

            // Sanity cap to prevent runaway geometry on tiny faces / huge bricks.
            constexpr int kMaxBrickGrid = 10000;
            if (cols <= 0 || rows <= 0 || cols > kMaxBrickGrid || rows > kMaxBrickGrid) {
                FC_WARN(getFullName() << ": skipping face, brick grid out of bounds ("
                                      << cols << "x" << rows << ")");
                continue;
            }

            // Direction vectors scaled by brick+mortar pitch
            gp_Vec uStep = uAxis * (brickW + effMortarT);
            gp_Vec vStep = vAxis * (brickH + effMortarT);
            gp_Vec uBrick = uAxis * brickW;
            gp_Vec vBrick = vAxis * brickH;

            // Row offset vector for running bond pattern
            gp_Vec rowOffVec = uAxis * (rowOff * brickW);

            // -----------------------------------------------------------
            //  Batch 1: Build all brick 2D faces into a compound, then
            //  clip and extrude the entire batch in one operation.
            //  The prism starts at `-overlap` (inside the base) and ends
            //  at `+brickD` (above the face), guaranteeing overlap with
            //  the base solid for a clean fuse.
            // -----------------------------------------------------------
            {
                BRep_Builder brickBuilder;
                TopoDS_Compound brickCompound;
                brickBuilder.MakeCompound(brickCompound);

                for (int row = 0; row < rows; row++) {
                    gp_Vec rowOffset = (row % 2 == 1) ? rowOffVec : gp_Vec(0, 0, 0);

                    for (int col = 0; col < cols; col++) {
                        gp_Pnt brickOrigin(p00.XYZ()
                            + uStep.XYZ() * col
                            + vStep.XYZ() * row
                            + rowOffset.XYZ());
                        TopoDS_Shape brick2D = makeBrickFace(brickOrigin, uBrick, vBrick);
                        brickBuilder.Add(brickCompound, brick2D);
                    }
                }

                // Clip all brick faces against the face in one operation
                BRepAlgoAPI_Common common(face, brickCompound);
                if (common.IsDone()) {
                    TopoDS_Shape clipped = common.Shape();
                    if (!clipped.IsNull()) {
                        // Translate the clipped profile down by `overlap` so its
                        // bottom face sits inside the base solid, then extrude
                        // by `brickD + overlap` outward. The resulting prism
                        // spans [-overlap, +brickD] along the normal, which
                        // guarantees a 3D overlap with the base solid and
                        // produces a single fused solid (rather than two
                        // disjoint solids when brickD == 0 or when the brick
                        // bottom sits exactly on the base face).
                        gp_Trsf translation;
                        translation.SetTranslation(gp_Vec(normal) * (-overlap));
                        TopoDS_Shape shiftedClipped = clipped.Moved(translation);
                        gp_Vec extrudeVec(normal);
                        extrudeVec.Multiply(brickExtrudeTotal);
                        TopoDS_Shape brick3D =
                            BRepPrimAPI_MakePrism(shiftedClipped, extrudeVec).Shape();
                        builder.Add(allShapes, brick3D);
                    }
                }
            }

            // -----------------------------------------------------------
            //  Batch 2: Horizontal mortar strips
            // -----------------------------------------------------------
            if (mortarD > 0) {
                {
                    BRep_Builder mortarBuilder;
                    TopoDS_Compound mortarCompound;
                    mortarBuilder.MakeCompound(mortarCompound);

                    for (int row = 0; row <= rows; row++) {
                        double mortarOffsetFrac = 0.0;
                        if (row == 0) {
                            mortarOffsetFrac = -1.0;
                        }
                        else if (row == rows) {
                            mortarOffsetFrac = 1.0;
                        }

                        // Center the strip in the mortar gap between bricks:
                        // gap is at vStep*row - vAxis*mortarT/2 .. +vAxis*mortarT/2
                        gp_Pnt mortarOrigin(p00.XYZ()
                            + vStep.XYZ() * row
                            - vAxis.XYZ() * (mortarT / 2.0)
                            + mortarOffsetFrac * vAxis.XYZ() * (mortarT / 2.0)
                            - uAxis.XYZ() * brickD);

                        gp_Vec mortarWidth = uAxis * (faceW + 2 * brickD);
                        gp_Vec mortarHeight = vAxis * mortarT;
                        TopoDS_Shape mortar2D =
                            makeBrickFace(mortarOrigin, mortarWidth, mortarHeight);
                        mortarBuilder.Add(mortarCompound, mortar2D);
                    }

                    BRepAlgoAPI_Common common(face, mortarCompound);
                    if (common.IsDone()) {
                        TopoDS_Shape clipped = common.Shape();
                        if (!clipped.IsNull()) {
                            gp_Vec extrudeVec(normal);
                            extrudeVec.Multiply(-mortarD);
                            TopoDS_Shape mortar3D =
                                BRepPrimAPI_MakePrism(clipped, extrudeVec).Shape();
                            builder.Add(allShapes, mortar3D);
                        }
                    }
                }

                // -----------------------------------------------------------
                //  Batch 3: Vertical mortar strips
                //  Loop bounds aligned with brick loop (col in [0, cols]) and
                //  offset by full mortarT so the strip is centered in the gap.
                // -----------------------------------------------------------
                {
                    BRep_Builder mortarBuilder;
                    TopoDS_Compound mortarCompound;
                    mortarBuilder.MakeCompound(mortarCompound);

                    for (int row = 0; row < rows; row++) {
                        gp_Vec rowOffset = (row % 2 == 1) ? rowOffVec : gp_Vec(0, 0, 0);

                        for (int col = 0; col <= cols; col++) {
                            // Center the strip in the gap between brick[col-1] and brick[col].
                            // The gap is at uStep*col - uAxis*mortarT/2 .. +uAxis*mortarT/2.
                            gp_Pnt mortarOrigin(p00.XYZ()
                                + uStep.XYZ() * col
                                + vStep.XYZ() * row
                                + rowOffset.XYZ()
                                - uAxis.XYZ() * (mortarT / 2.0));

                            gp_Vec mortarWidth = uAxis * mortarT;
                            gp_Vec mortarHeight = vAxis * (brickH + effMortarT);
                            TopoDS_Shape mortar2D =
                                makeBrickFace(mortarOrigin, mortarWidth, mortarHeight);
                            mortarBuilder.Add(mortarCompound, mortar2D);
                        }
                    }

                    BRepAlgoAPI_Common common(face, mortarCompound);
                    if (common.IsDone()) {
                        TopoDS_Shape clipped = common.Shape();
                        if (!clipped.IsNull()) {
                            gp_Vec extrudeVec(normal);
                            extrudeVec.Multiply(-mortarD);
                            TopoDS_Shape mortar3D =
                                BRepPrimAPI_MakePrism(clipped, extrudeVec).Shape();
                            builder.Add(allShapes, mortar3D);
                        }
                    }
                }
            }
        }

        // Fuse the base shape with the batched result compound.
        // The compound contains only a few shapes (one per face per batch),
        // so this fuse is fast regardless of brick count.
        BRepAlgoAPI_Fuse fuse(TopShape.getShape(), allShapes);
        if (!fuse.IsDone()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Failed to fuse bricks with base shape")
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

void BrickTexture::onChanged(const App::Property* prop)
{
    DressUp::onChanged(prop);
}
