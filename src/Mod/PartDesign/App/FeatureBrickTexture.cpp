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
    RowOffset.setConstraints(0, 100, 1);

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
            gp_Pnt loc = plane.Location();

            Bnd_Box bbox;
            BRepBndLib::Add(bbox, face);
            double xMin, yMin, zMin, xMax, yMax, zMax;
            bbox.Get(xMin, yMin, zMin, xMax, yMax, zMax);

            double faceW = xMax - xMin;
            double faceH = yMax - yMin;

            int cols = static_cast<int>(std::ceil(faceW / (brickW + mortarT)));
            int rows = static_cast<int>(std::ceil(faceH / (brickH + mortarT)));

            if (cols <= 0 || rows <= 0) {
                continue;
            }

            gp_Trsf faceTrsf;
            faceTrsf.SetTransformation(gp_Ax3(gp_Pnt(0, 0, 0), normal), gp_Ax3(loc, normal));

            double offsetDx = xMin;
            double offsetDy = yMin;

            for (int row = 0; row < rows; row++) {
                double rowOffsetX = (row % 2 == 1) ? rowOff * brickW : 0.0;

                for (int col = -1; col <= cols; col++) {
                    double bx = offsetDx + col * (brickW + mortarT) + rowOffsetX;
                    double by = offsetDy + row * (brickH + mortarT);

                    gp_Pnt brickOrigin(bx, by, 0.0);
                    TopoDS_Shape brick2D = makeBrickFace(brickOrigin, brickW, brickH);

                    try {
                        BRepAlgoAPI_Common common(face, brick2D);
                        if (!common.IsDone()) {
                            continue;
                        }
                        TopoDS_Shape clipped = common.Shape();

                        if (clipped.IsNull() || !clipped.HasSubShape(TopAbs_FACE)) {
                            continue;
                        }

                        gp_Vec extrudeVec(normal);
                        extrudeVec.Multiply(brickD);
                        TopoDS_Shape brick3D = BRepPrimAPI_MakePrism(clipped, extrudeVec).Shape();

                        builder.Add(compound, brick3D);
                    }
                    catch (Standard_Failure&) {
                        continue;
                    }
                }
            }

            if (mortarD > 0) {
                for (int row = 0; row <= rows; row++) {
                    double rowOffsetX = (row % 2 == 1) ? rowOff * brickW : 0.0;

                    double hx = offsetDx - brickD;
                    double hy = offsetDy + row * (brickH + mortarT) - mortarT / 2.0;
                    double hw = faceW + 2 * brickD;
                    double hh = mortarT;

                    if (row == 0) {
                        hy = offsetDy - mortarT;
                    }
                    else if (row == rows) {
                        hy = offsetDy + rows * (brickH + mortarT);
                    }

                    gp_Pnt mortarOrigin(hx, hy, 0.0);
                    TopoDS_Shape mortar2D = makeBrickFace(mortarOrigin, hw, hh);

                    try {
                        BRepAlgoAPI_Common common(face, mortar2D);
                        if (!common.IsDone()) {
                            continue;
                        }
                        TopoDS_Shape clipped = common.Shape();

                        if (clipped.IsNull() || !clipped.HasSubShape(TopAbs_FACE)) {
                            continue;
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

                for (int row = 0; row < rows; row++) {
                    double rowOffsetX = (row % 2 == 1) ? rowOff * brickW : 0.0;

                    for (int col = 0; col <= cols; col++) {
                        double vx = offsetDx + col * (brickW + mortarT) + rowOffsetX - mortarT / 2.0;
                        double vy = offsetDy + row * (brickH + mortarT);
                        double vw = mortarT;
                        double vh = brickH + mortarT;

                        gp_Pnt mortarOrigin(vx, vy, 0.0);
                        TopoDS_Shape mortar2D = makeBrickFace(mortarOrigin, vw, vh);

                        try {
                            BRepAlgoAPI_Common common(face, mortar2D);
                            if (!common.IsDone()) {
                                continue;
                            }
                            TopoDS_Shape clipped = common.Shape();

                            if (clipped.IsNull() || !clipped.HasSubShape(TopAbs_FACE)) {
                                continue;
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

        Part::TopoShape result(compound);
        result = refineShapeIfActive(result);
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
