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

#include "FeatureWoodPlankTexture.h"

FC_LOG_LEVEL_INIT("PartDesign", true, true)

using namespace PartDesign;

PROPERTY_SOURCE(PartDesign::WoodPlankTexture, PartDesign::DressUp)

const App::PropertyQuantityConstraint::Constraints WoodPlankTexture::floatSize = {0.1, 10000.0, 1.0};
const App::PropertyIntegerConstraint::Constraints WoodPlankTexture::intPercent = {0, 100, 1};

const char* WoodPlankTexture::ScaleEnums[]
    = {"1:1", "1:2", "1:5", "1:10", "1:20", "1:50", "1:76", "1:100", "1:200", "1:500", "Custom", nullptr};

const char* WoodPlankTexture::OrientationEnums[]
    = {"Horizontal", "Vertical", "Diagonal", nullptr};

WoodPlankTexture::WoodPlankTexture()
{
    ADD_PROPERTY_TYPE(
        PlankLength,
        (300.0),
        "Plank",
        App::Prop_None,
        "Length of each plank along the run direction"
    );
    PlankLength.setUnit(Base::Unit::Length);
    PlankLength.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        PlankHeight,
        (100.0),
        "Plank",
        App::Prop_None,
        "Height of each plank (short dimension)"
    );
    PlankHeight.setUnit(Base::Unit::Length);
    PlankHeight.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        PlankDepth,
        (8.0),
        "Plank",
        App::Prop_None,
        "Protrusion depth of planks above the face"
    );
    PlankDepth.setUnit(Base::Unit::Length);
    PlankDepth.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        GapWidth,
        (3.0),
        "Plank",
        App::Prop_None,
        "Width of gap between planks (0 for tight-fitting)"
    );
    GapWidth.setUnit(Base::Unit::Length);
    GapWidth.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        GapDepth,
        (2.0),
        "Plank",
        App::Prop_None,
        "Recession depth of gaps below the face"
    );
    GapDepth.setUnit(Base::Unit::Length);
    GapDepth.setConstraints(&floatSize);

    ADD_PROPERTY_TYPE(
        Orientation,
        (0L),
        "Plank",
        App::Prop_None,
        "Orientation of planks on the face"
    );
    Orientation.setEnums(OrientationEnums);

    ADD_PROPERTY_TYPE(
        EndOffset,
        (50),
        "Plank",
        App::Prop_None,
        "Vertical offset of end joints between rows as percentage of plank height"
    );
    EndOffset.setConstraints(&intPercent);

    ADD_PROPERTY_TYPE(
        Scale,
        (0L),
        "Plank",
        App::Prop_None,
        "Scale factor for model-making"
    );
    Scale.setEnums(ScaleEnums);
}

short WoodPlankTexture::mustExecute() const
{
    if (PlankLength.isTouched() || PlankHeight.isTouched() || PlankDepth.isTouched()
        || GapWidth.isTouched() || GapDepth.isTouched() || Orientation.isTouched()
        || EndOffset.isTouched() || Scale.isTouched()) {
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

static TopoDS_Shape makePlankFace(const gp_Pnt& origin, const gp_Vec& uDir, const gp_Vec& vDir)
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

App::DocumentObjectExecReturn* WoodPlankTexture::execute()
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
    double plankL = PlankLength.getValue() * scaleFactor;
    double plankH = PlankHeight.getValue() * scaleFactor;
    double plankD = PlankDepth.getValue() * scaleFactor;
    double gapW = GapWidth.getValue() * scaleFactor;
    double gapD = GapDepth.getValue() * scaleFactor;
    double endOff = EndOffset.getValue() / 100.0;
    long orientation = Orientation.getValue();

    if (plankL <= 0 || plankH <= 0) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Plank dimensions must be positive")
        );
    }
    if (plankD < 0 || gapW < 0 || gapD < 0) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Gap and depth values must be non-negative")
        );
    }

    // Small overlap so outward-extruded planks share 3D volume with the base
    // solid. Without this, BRepAlgoAPI_Fuse produces a disjoint compound when
    // plank bottoms sit exactly on the base face.
    const double overlap = 10.0 * Precision::Confusion();
    const double plankExtrudeTotal = plankD + overlap;

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

            // For diagonal orientation, rotate the axes by 45 degrees
            gp_Vec runAxis, stackAxis;
            if (orientation == 0) {
                // Horizontal: planks run along uAxis, stack along vAxis
                runAxis = uAxis;
                stackAxis = vAxis;
            }
            else if (orientation == 1) {
                // Vertical: planks run along vAxis, stack along uAxis
                runAxis = vAxis;
                stackAxis = uAxis;
            }
            else {
                // Diagonal: planks run at 45 degrees
                runAxis = uAxis * (1.0 / std::sqrt(2.0)) + vAxis * (1.0 / std::sqrt(2.0));
                stackAxis = vAxis * (1.0 / std::sqrt(2.0)) - uAxis * (1.0 / std::sqrt(2.0));
            }

            // Treat degenerate gaps as a tight fit to keep integer math well-behaved.
            double effGapW = std::max(gapW, Precision::Confusion());

            int cols = static_cast<int>(std::ceil(faceW / (plankL + effGapW)));
            int rows = static_cast<int>(std::ceil(faceH / (plankH + effGapW)));

            // Sanity cap to prevent runaway geometry on tiny faces / huge planks.
            constexpr int kMaxPlankGrid = 10000;
            if (cols <= 0 || rows <= 0 || cols > kMaxPlankGrid || rows > kMaxPlankGrid) {
                FC_WARN(getFullName() << ": skipping face, plank grid out of bounds ("
                                      << cols << "x" << rows << ")");
                continue;
            }

            gp_Vec runStep = runAxis * (plankL + effGapW);
            gp_Vec stackStep = stackAxis * (plankH + effGapW);
            gp_Vec runPlank = runAxis * plankL;
            gp_Vec stackPlank = stackAxis * plankH;

            // End offset vector for staggering end joints
            gp_Vec endOffVec = stackAxis * (endOff * plankH);

            for (int row = 0; row < rows; row++) {
                gp_Vec rowOffset = (row % 2 == 1) ? endOffVec : gp_Vec(0, 0, 0);

                for (int col = 0; col < cols; col++) {
                    gp_Pnt plankOrigin(p00.XYZ()
                        + runStep.XYZ() * col
                        + stackStep.XYZ() * row
                        + rowOffset.XYZ());
                    TopoDS_Shape plank2D = makePlankFace(plankOrigin, runPlank, stackPlank);

                    try {
                        BRepAlgoAPI_Common common(face, plank2D);
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

                        // Translate the clipped profile down by `overlap` so
                        // its bottom face sits inside the base solid, then
                        // extrude by `plankD + overlap` outward. This gives a
                        // single fused solid rather than disjoint ones.
                        gp_Trsf translation;
                        translation.SetTranslation(gp_Vec(normal) * (-overlap));
                        TopoDS_Shape shiftedClipped = clipped.Moved(translation);
                        gp_Vec extrudeVec(normal);
                        extrudeVec.Multiply(plankExtrudeTotal);
                        TopoDS_Shape plank3D =
                            BRepPrimAPI_MakePrism(shiftedClipped, extrudeVec).Shape();

                        builder.Add(compound, plank3D);
                    }
                    catch (Standard_Failure&) {
                        continue;
                    }
                }
            }

            if (gapD > 0 && gapW > 0) {
                // Horizontal gap strips (between rows)
                for (int row = 0; row <= rows; row++) {
                    double gapOffsetFrac = 0.0;
                    if (row == 0) {
                        gapOffsetFrac = -1.0;
                    }
                    else if (row == rows) {
                        gapOffsetFrac = 1.0;
                    }

                    gp_Pnt gapOrigin(p00.XYZ()
                        + stackStep.XYZ() * row
                        - stackAxis.XYZ() * (gapW / 2.0)
                        + gapOffsetFrac * stackAxis.XYZ() * (gapW / 2.0)
                        - runAxis.XYZ() * plankD);

                    gp_Vec gapLength = runAxis * (faceW + 2 * plankD);
                    gp_Vec gapHeight = stackAxis * gapW;
                    TopoDS_Shape gap2D = makePlankFace(gapOrigin, gapLength, gapHeight);

                    try {
                        BRepAlgoAPI_Common common(face, gap2D);
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
                        extrudeVec.Multiply(-gapD);
                        TopoDS_Shape gap3D = BRepPrimAPI_MakePrism(clipped, extrudeVec).Shape();

                        builder.Add(compound, gap3D);
                    }
                    catch (Standard_Failure&) {
                        continue;
                    }
                }

                // Vertical gap strips (between planks within rows)
                for (int row = 0; row < rows; row++) {
                    gp_Vec rowOffset = (row % 2 == 1) ? endOffVec : gp_Vec(0, 0, 0);

                    for (int col = 0; col <= cols; col++) {
                        gp_Pnt gapOrigin(p00.XYZ()
                            + runStep.XYZ() * col
                            + stackStep.XYZ() * row
                            + rowOffset.XYZ()
                            - runAxis.XYZ() * (gapW / 2.0));

                        gp_Vec gapLength = runAxis * gapW;
                        gp_Vec gapHeight = stackAxis * (plankH + effGapW);
                        TopoDS_Shape gap2D = makePlankFace(gapOrigin, gapLength, gapHeight);

                        try {
                            BRepAlgoAPI_Common common(face, gap2D);
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
                            extrudeVec.Multiply(-gapD);
                            TopoDS_Shape gap3D = BRepPrimAPI_MakePrism(clipped, extrudeVec).Shape();

                            builder.Add(compound, gap3D);
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
                QT_TRANSLATE_NOOP("Exception", "Failed to fuse wood planks with base shape")
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

void WoodPlankTexture::onChanged(const App::Property* prop)
{
    DressUp::onChanged(prop);
}
