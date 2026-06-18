// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten CAD contributors                           *
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
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; see the file COPYING.LIB. If not,    *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,         *
 *   Suite 330, Boston, MA  02111-1307, USA                                *
 *                                                                         *
 ***************************************************************************/

#include <limits>
#include <numbers>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Mod/Part/App/FCBRepAlgoAPI_Common.h>
#include <Mod/Part/App/FCBRepAlgoAPI_Cut.h>
#include <Mod/Part/App/FCBRepAlgoAPI_Fuse.h>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeLine.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <ShapeFix_ShapeTolerance.hxx>
#include <ShapeFix_Solid.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Wire.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <Base/Axis.h>
#include <Base/Console.h>
#include <Base/Exception.h>
#include <Base/Placement.h>
#include <Base/Tools.h>

#include <App/Datums.h>

#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/FaceMakerCheese.h>
#include <Mod/Part/App/DatumFeature.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/PartDesign/App/DatumLine.h>

#include "FeatureCoil.h"

using namespace PartDesign;

const char* Coil::ModeEnums[]
    = {"Revolution+Height", "Revolution+Pitch", "Height+Pitch", "Spiral", nullptr};

const char* Coil::SectionShapeEnums[]
    = {"Circular", "Square", "Triang.External", "Triang.Internal", nullptr};

const char* Coil::SectionPositionEnums[] = {"Inside", "On Center", "Outside", nullptr};

PROPERTY_SOURCE(PartDesign::Coil, PartDesign::FeatureAddSub)

// constraints
const App::PropertyFloatConstraint::Constraints Coil::floatTurns
    = {Precision::Confusion(), std::numeric_limits<int>::max(), 1.0};
const App::PropertyAngle::Constraints Coil::floatAngle = {-89.0, 89.0, 1.0};
const App::PropertyFloatConstraint::Constraints Coil::floatTolerance
    = {0.1, std::numeric_limits<int>::max(), 1.0};

Coil::Coil()
{
    addSubType = FeatureAddSub::Additive;

    const char* group = "Coil";

    ADD_PROPERTY_TYPE(
        Base,
        (Base::Vector3d(0.0, 0.0, 0.0)),
        group,
        App::Prop_ReadOnly,
        QT_TRANSLATE_NOOP("App::Property", "The center point of the coil's start.")
    );
    ADD_PROPERTY_TYPE(
        Axis,
        (Base::Vector3d(0.0, 1.0, 0.0)),
        group,
        App::Prop_ReadOnly,
        QT_TRANSLATE_NOOP("App::Property", "The coil's direction.")
    );
    ADD_PROPERTY_TYPE(
        ReferenceAxis,
        (nullptr),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP("App::Property", "The reference axis of the coil.")
    );
    ADD_PROPERTY_TYPE(
        Mode,
        (long(CoilMode::revolutionHeight)),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP(
            "App::Property",
            "The coil input mode specifies which properties are set by the user.\n"
            "Dependent properties are then calculated."
        )
    );
    Mode.setEnums(ModeEnums);
    ADD_PROPERTY_TYPE(
        Radius,
        (10.0),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP("App::Property", "The radius of the coil.")
    );
    ADD_PROPERTY_TYPE(
        Pitch,
        (5.0),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP("App::Property", "The axial distance between two turns.")
    );
    ADD_PROPERTY_TYPE(
        Height,
        (30.0),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP("App::Property", "The height of the coil.")
    );
    ADD_PROPERTY_TYPE(
        Turns,
        (6.0),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP("App::Property", "The number of revolutions in the coil.")
    );
    Turns.setConstraints(&floatTurns);
    ADD_PROPERTY_TYPE(
        Angle,
        (0.0),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP(
            "App::Property",
            "The taper angle of the coil.\n"
            "Non-zero values create a conical coil.\n"
            "Positive values make the radius grow, negative shrinks."
        )
    );
    Angle.setConstraints(&floatAngle);
    ADD_PROPERTY_TYPE(
        SectionSize,
        (2.0),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP("App::Property", "The diameter of the coil's cross-section.")
    );
    ADD_PROPERTY_TYPE(
        SectionShape,
        (long(CoilSectionShape::circular)),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP("App::Property", "The cross-section shape of the coil wire.")
    );
    SectionShape.setEnums(SectionShapeEnums);
    ADD_PROPERTY_TYPE(
        SectionPosition,
        (long(CoilSectionPosition::onCenter)),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP(
            "App::Property",
            "Where the cross-section is positioned relative to the coil radius.\n"
            "Inside: profile sits inside the coil radius.\n"
            "On Center: profile centered on the coil radius.\n"
            "Outside: profile sits outside the coil radius."
        )
    );
    SectionPosition.setEnums(SectionPositionEnums);
    ADD_PROPERTY_TYPE(
        LeftHanded,
        (false),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP(
            "App::Property",
            "Sets the turning direction to left handed,\n"
            "i.e. counter-clockwise when moving along its axis."
        )
    );
    ADD_PROPERTY_TYPE(
        Reversed,
        (false),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP(
            "App::Property",
            "Determines whether the coil points in the opposite direction of the axis."
        )
    );
    ADD_PROPERTY_TYPE(
        Tolerance,
        (0.1),
        group,
        App::Prop_None,
        QT_TRANSLATE_NOOP(
            "App::Property",
            "Fusion tolerance for the coil. Increase if the coil shape does not merge nicely."
        )
    );
    Tolerance.setConstraints(&floatTolerance);

    setReadWriteStatusForMode(CoilMode::revolutionHeight);
}

short Coil::mustExecute() const
{
    if (Placement.isTouched() || ReferenceAxis.isTouched() || Axis.isTouched() || Base.isTouched()
        || Angle.isTouched()) {
        return 1;
    }
    return FeatureAddSub::mustExecute();
}

App::DocumentObjectExecReturn* Coil::execute()
{
    if (onlyHaveRefined()) {
        return App::DocumentObject::StdReturn;
    }

    // Validate and normalize parameters
    CoilMode mode = static_cast<CoilMode>(Mode.getValue());
    if (mode == CoilMode::revolutionHeight) {
        if (Turns.getValue() < Precision::Confusion()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Error: revolutions too small!")
            );
        }
        if (Height.getValue() < Precision::Confusion()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Error: height too small!")
            );
        }
        Pitch.setValue(Height.getValue() / Turns.getValue());
    }
    else if (mode == CoilMode::revolutionPitch) {
        if (Turns.getValue() < Precision::Confusion()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Error: revolutions too small!")
            );
        }
        if (Pitch.getValue() < Precision::Confusion()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Error: pitch too small!")
            );
        }
        Height.setValue(Turns.getValue() * Pitch.getValue());
    }
    else if (mode == CoilMode::heightPitch) {
        if (Height.getValue() < Precision::Confusion()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Error: height too small!")
            );
        }
        if (Pitch.getValue() < Precision::Confusion()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Error: pitch too small!")
            );
        }
        Turns.setValue(Height.getValue() / Pitch.getValue());
    }
    else if (mode == CoilMode::spiral) {
        // Spiral mode: flat 2D spiral, Height = 0
        Height.setValue(0.0);
        if (Turns.getValue() < Precision::Confusion()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Error: revolutions too small!")
            );
        }
        if (Pitch.getValue() < Precision::Confusion()) {
            return new App::DocumentObjectExecReturn(
                QT_TRANSLATE_NOOP("Exception", "Error: pitch too small!")
            );
        }
    }
    else {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Error: unsupported mode")
        );
    }

    if (Radius.getValue() < Precision::Confusion()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Error: radius too small!")
        );
    }

    if (SectionSize.getValue() < Precision::Confusion()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Error: section size too small!")
        );
    }

    // generate the cross-section profile
    TopoDS_Shape profile;
    try {
        profile = generateProfile();
    }
    catch (const Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    if (profile.IsNull()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Error: Could not generate cross-section profile")
        );
    }

    // if the Base property has a valid shape, fuse the AddShape into it
    TopoShape base;
    try {
        base = getBaseTopoShape();
    }
    catch (const Base::Exception&) {
        base = TopoShape();
    }

    // update Axis from ReferenceAxis
    try {
        updateAxis();
    }
    catch (const Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }

    try {
        // positionByPrevious equivalent: copy placement from base feature if available
        Part::Feature* baseFeat = getBaseObject(/* silent = */ true);
        if (baseFeat) {
            this->Placement.setValue(baseFeat->Placement.getValue());
        }
        TopLoc_Location invObjLoc = this->getLocation().Inverted();

        base.move(invObjLoc);

        TopoDS_Shape result;

        // generate the helix path
        TopoDS_Shape path;
        double angle = Angle.getValue();
        if (fabs(angle) < Precision::Confusion()) {
            path = generateHelixPath();
        }
        else {
            // don't break the path for conical coils
            path = generateHelixPath(1000.);
        }

        Bnd_Box bounds;
        BRepBndLib::Add(path, bounds);
        double size = sqrt(bounds.SquareExtent());
        ShapeFix_ShapeTolerance fix;
        fix.LimitTolerance(path, Precision::Confusion() * 1e-6 * size);

        BRepOffsetAPI_MakePipe
            mkPS(TopoDS::Wire(path), profile, GeomFill_Trihedron::GeomFill_IsFrenet, Standard_False);
        result = mkPS.Shape();

        BRepClass3d_SolidClassifier SC(result);
        SC.PerformInfinitePoint(Precision::Confusion());
        if (SC.State() == TopAbs_IN) {
            result.Reverse();
        }

        fix.LimitTolerance(result, Precision::Confusion() * size * Tolerance.getValue());

        ShapeFix_Solid fixer;
        fixer.Init(TopoDS::Solid(result));
        if (fixer.Perform()) {
            result = fixer.Solid();
        }

        AddSubShape.setValue(result);

        if (base.isNull()) {
            if (getAddSubType() == FeatureAddSub::Subtractive) {
                return new App::DocumentObjectExecReturn(
                    QT_TRANSLATE_NOOP("Exception", "Error: There is nothing to subtract")
                );
            }

            if (!isSingleSolidRuleSatisfied(result)) {
                return new App::DocumentObjectExecReturn(
                    QT_TRANSLATE_NOOP("Exception", "Error: Result has multiple solids")
                );
            }

            this->rawShape = result;
            Shape.setValue(getSolid(result));
            return App::DocumentObject::StdReturn;
        }

        if (getAddSubType() == FeatureAddSub::Additive) {
            FCBRepAlgoAPI_Fuse mkFuse(base.getShape(), result);
            if (!mkFuse.IsDone()) {
                return new App::DocumentObjectExecReturn(
                    QT_TRANSLATE_NOOP("Exception", "Error: Adding the coil failed")
                );
            }
            TopoShape boolOp = this->getSolid(mkFuse.Shape());

            if (boolOp.isNull()) {
                return new App::DocumentObjectExecReturn(
                    QT_TRANSLATE_NOOP("Exception", "Error: Result is not a solid")
                );
            }

            if (!isSingleSolidRuleSatisfied(boolOp.getShape())) {
                return new App::DocumentObjectExecReturn(
                    QT_TRANSLATE_NOOP("Exception", "Error: Result has multiple solids")
                );
            }

            this->rawShape = boolOp;
            boolOp = refineShapeIfActive(boolOp, RefineErrorPolicy::Warn);
            Shape.setValue(getSolid(boolOp));
        }
        else if (getAddSubType() == FeatureAddSub::Subtractive) {
            TopoShape boolOp;
            FCBRepAlgoAPI_Cut mkCut(base.getShape(), result);
            if (!mkCut.IsDone()) {
                return new App::DocumentObjectExecReturn(
                    QT_TRANSLATE_NOOP("Exception", "Error: Subtracting the coil failed")
                );
            }
            boolOp = this->getSolid(mkCut.Shape());

            if (boolOp.isNull()) {
                return new App::DocumentObjectExecReturn(
                    QT_TRANSLATE_NOOP("Exception", "Error: Result is not a solid")
                );
            }

            if (!isSingleSolidRuleSatisfied(boolOp.getShape())) {
                return new App::DocumentObjectExecReturn(
                    QT_TRANSLATE_NOOP("Exception", "Error: Result has multiple solids")
                );
            }

            this->rawShape = boolOp;
            boolOp = refineShapeIfActive(boolOp, RefineErrorPolicy::Warn);
            Shape.setValue(getSolid(boolOp));
        }

        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure& e) {
        return new App::DocumentObjectExecReturn(e.GetMessageString());
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }
}

TopoDS_Shape Coil::generateProfile()
{
    double sectionSize = SectionSize.getValue();
    double radius = Radius.getValue();
    CoilSectionShape shape = static_cast<CoilSectionShape>(SectionShape.getValue());
    CoilSectionPosition pos = static_cast<CoilSectionPosition>(SectionPosition.getValue());

    // Calculate the profile center offset from the coil center axis
    double profileCenterRadius;
    switch (pos) {
        case CoilSectionPosition::inside:
            profileCenterRadius = radius;
            break;
        case CoilSectionPosition::onCenter:
            profileCenterRadius = radius + sectionSize / 2.0;
            break;
        case CoilSectionPosition::outside:
            profileCenterRadius = radius + sectionSize;
            break;
        default:
            profileCenterRadius = radius;
            break;
    }

    // Profile is created in the XZ plane, centered at (profileCenterRadius, 0, 0)
    // The helix path starts at this point
    gp_Pnt center(profileCenterRadius, 0.0, 0.0);
    gp_Dir xDir(1.0, 0.0, 0.0);
    gp_Dir yDir(0.0, 1.0, 0.0);
    gp_Dir zDir(0.0, 0.0, 1.0);

    TopoDS_Shape profile;

    if (shape == CoilSectionShape::circular) {
        // Circular cross-section: circle in XZ plane at the profile center
        gp_Ax2 axis(center, yDir);  // Y-axis is normal to XZ plane
        gp_Circ circ(axis, sectionSize / 2.0);
        BRepBuilderAPI_MakeEdge makeEdge(circ);
        if (!makeEdge.IsDone()) {
            throw Base::RuntimeError("Error: Could not create circular edge");
        }
        BRepBuilderAPI_MakeWire makeWire(makeEdge.Edge());
        if (!makeWire.IsDone()) {
            throw Base::RuntimeError("Error: Could not create circular wire");
        }
        profile = makeWire.Wire();
    }
    else if (shape == CoilSectionShape::square) {
        // Square cross-section: square in XZ plane centered at profile center
        double half = sectionSize / 2.0;
        gp_Pnt p1(center.X() - half, center.Y(), center.Z() - half);
        gp_Pnt p2(center.X() + half, center.Y(), center.Z() - half);
        gp_Pnt p3(center.X() + half, center.Y(), center.Z() + half);
        gp_Pnt p4(center.X() - half, center.Y(), center.Z() + half);

        BRepBuilderAPI_MakeEdge e1(p1, p2);
        BRepBuilderAPI_MakeEdge e2(p2, p3);
        BRepBuilderAPI_MakeEdge e3(p3, p4);
        BRepBuilderAPI_MakeEdge e4(p4, p1);
        BRepBuilderAPI_MakeWire makeWire(e1.Edge(), e2.Edge(), e3.Edge(), e4.Edge());
        if (!makeWire.IsDone()) {
            throw Base::RuntimeError("Error: Could not create square wire");
        }
        profile = makeWire.Wire();
    }
    else if (shape == CoilSectionShape::triangularExternal || shape == CoilSectionShape::triangularInternal) {
        // Equilateral triangle cross-section
        double half = sectionSize / 2.0;
        // Height of equilateral triangle with side = sectionSize
        double triHeight = sectionSize * sqrt(3.0) / 2.0;

        gp_Pnt p1, p2, p3;
        if (shape == CoilSectionShape::triangularExternal) {
            // Triangle pointing outward (away from axis)
            p1 = gp_Pnt(center.X() + half, center.Y(), center.Z());
            p2 = gp_Pnt(center.X() - half, center.Y(), center.Z() - triHeight / 2.0);
            p3 = gp_Pnt(center.X() - half, center.Y(), center.Z() + triHeight / 2.0);
        }
        else {
            // Triangle pointing inward (toward axis)
            p1 = gp_Pnt(center.X() - half, center.Y(), center.Z());
            p2 = gp_Pnt(center.X() + half, center.Y(), center.Z() - triHeight / 2.0);
            p3 = gp_Pnt(center.X() + half, center.Y(), center.Z() + triHeight / 2.0);
        }

        BRepBuilderAPI_MakeEdge e1(p1, p2);
        BRepBuilderAPI_MakeEdge e2(p2, p3);
        BRepBuilderAPI_MakeEdge e3(p3, p1);
        BRepBuilderAPI_MakeWire makeWire(e1.Edge(), e2.Edge(), e3.Edge());
        if (!makeWire.IsDone()) {
            throw Base::RuntimeError("Error: Could not create triangular wire");
        }
        profile = makeWire.Wire();
    }

    return profile;
}

void Coil::updateAxis()
{
    App::DocumentObject* pcReferenceAxis = ReferenceAxis.getValue();
    const std::vector<std::string>& subReferenceAxis = ReferenceAxis.getSubValues();
    Base::Vector3d base;
    Base::Vector3d dir(0.0, 0.0, 0.0);

    if (!pcReferenceAxis || subReferenceAxis.empty()) {
        base = Base::Vector3d(0.0, 0.0, 0.0);
        dir = Base::Vector3d(0.0, 1.0, 0.0);
        Base.setValue(base.x, base.y, base.z);
        Axis.setValue(dir.x, dir.y, dir.z);
        return;
    }

    // Handle PartDesign::Line datum
    if (pcReferenceAxis->isDerivedFrom<PartDesign::Line>()) {
        const auto* line = static_cast<const PartDesign::Line*>(pcReferenceAxis);
        base = line->getBasePoint();
        dir = line->getDirection();
        Base.setValue(base.x, base.y, base.z);
        Axis.setValue(dir.x, dir.y, dir.z);
        return;
    }

    // Handle App::Line datum
    if (pcReferenceAxis->isDerivedFrom<App::Line>()) {
        const auto* line = static_cast<const App::Line*>(pcReferenceAxis);
        base = line->getBasePoint();
        dir = line->getDirection();
        Base.setValue(base.x, base.y, base.z);
        Axis.setValue(dir.x, dir.y, dir.z);
        return;
    }

    // Handle Part::Feature with edge sub-reference
    if (pcReferenceAxis->isDerivedFrom<Part::Feature>()) {
        if (subReferenceAxis.empty()) {
            throw Base::ValueError("No rotation axis reference specified");
        }
        const auto* refFeature = static_cast<const Part::Feature*>(pcReferenceAxis);
        Part::TopoShape refShape = refFeature->Shape.getShape();
        TopoDS_Shape ref;
        try {
            ref = refShape.getSubShape(subReferenceAxis[0].c_str());
        }
        catch (const Standard_Failure& e) {
            throw Base::RuntimeError(e.GetMessageString());
        }

        if (ref.ShapeType() == TopAbs_EDGE) {
            TopoDS_Edge edge = TopoDS::Edge(ref);
            if (edge.IsNull()) {
                throw Base::ValueError("Failed to extract rotation edge");
            }
            BRepAdaptor_Curve adapt(edge);
            gp_Pnt b;
            gp_Dir d;
            if (adapt.GetType() == GeomAbs_Line) {
                b = adapt.Line().Location();
                d = adapt.Line().Direction();
            }
            else if (adapt.GetType() == GeomAbs_Circle) {
                b = adapt.Circle().Location();
                d = adapt.Circle().Axis().Direction();
            }
            else {
                throw Base::TypeError("Edge must be a straight line, circle or arc of circle");
            }
            base = Base::Vector3d(b.X(), b.Y(), b.Z());
            dir = Base::Vector3d(d.X(), d.Y(), d.Z());
            Base.setValue(base.x, base.y, base.z);
            Axis.setValue(dir.x, dir.y, dir.z);
            return;
        }
    }

    throw Base::TypeError("Unsupported geometry type to get reference axis");
}

TopoDS_Shape Coil::generateHelixPath(double breakAtTurn)
{
    double turns = Turns.getValue();
    double height = Height.getValue();
    bool leftHanded = LeftHanded.getValue();
    bool reversed = Reversed.getValue();
    double angle = Angle.getValue();
    double radius = Radius.getValue();

    if (fabs(angle) < Precision::Confusion()) {
        angle = 0.0;
    }

    Base::Vector3d baseVector = Base.getValue();
    gp_Pnt pnt(baseVector.x, baseVector.y, baseVector.z);
    Base::Vector3d axisVector = Axis.getValue();
    gp_Dir dir(axisVector.x, axisVector.y, axisVector.z);

    // Calculate the profile center for the starting point
    CoilSectionShape shape = static_cast<CoilSectionShape>(SectionShape.getValue());
    CoilSectionPosition pos = static_cast<CoilSectionPosition>(SectionPosition.getValue());
    double sectionSize = SectionSize.getValue();

    double profileCenterRadius;
    switch (pos) {
        case CoilSectionPosition::inside:
            profileCenterRadius = radius;
            break;
        case CoilSectionPosition::onCenter:
            profileCenterRadius = radius + sectionSize / 2.0;
            break;
        case CoilSectionPosition::outside:
            profileCenterRadius = radius + sectionSize;
            break;
        default:
            profileCenterRadius = radius;
            break;
    }

    // Calculate the radius at the top of the coil (for conical coils)
    double radiusTop;
    if (fabs(angle) > Precision::Confusion()) {
        radiusTop = radius + height * tan(Base::toRadians(angle));
    }
    else {
        radiusTop = radius;
    }

    // Build the helix path using TopoShape::makeSpiralHelix
    TopoDS_Shape path
        = TopoShape().makeSpiralHelix(profileCenterRadius, radiusTop, height, turns, breakAtTurn, leftHanded);

    // Move the helix to the correct position
    gp_Pnt origo(0.0, 0.0, 0.0);
    gp_Dir dir_axis1(0.0, 0.0, 1.0);
    gp_Dir dir_axis2(1.0, 0.0, 0.0);
    gp_Trsf mov;

    if (reversed) {
        mov.SetRotation(gp_Ax1(origo, dir_axis2), std::numbers::pi);
        TopLoc_Location loc(mov);
        path.Move(loc);
    }

    gp_Ax3 sourceCS(origo, dir_axis1, dir_axis2);
    gp_Ax3 targetCS(pnt, dir, dir_axis2);

    mov.SetTransformation(sourceCS, targetCS);
    TopLoc_Location loc(mov);
    path.Move(loc.Inverted());

    TopLoc_Location invObjLoc = this->getLocation().Inverted();
    path.Move(invObjLoc);

    return path;
}

void Coil::onChanged(const App::Property* prop)
{
    if (prop == &Mode) {
        auto inputMode = static_cast<CoilMode>(Mode.getValue());
        setReadWriteStatusForMode(inputMode);
    }

    FeatureAddSub::onChanged(prop);
}

void Coil::setReadWriteStatusForMode(CoilMode inputMode)
{
    switch (inputMode) {
        case CoilMode::revolutionHeight:
            Turns.setStatus(App::Property::ReadOnly, false);
            Height.setStatus(App::Property::ReadOnly, false);
            Pitch.setStatus(App::Property::ReadOnly, true);
            break;

        case CoilMode::revolutionPitch:
            Turns.setStatus(App::Property::ReadOnly, false);
            Pitch.setStatus(App::Property::ReadOnly, false);
            Height.setStatus(App::Property::ReadOnly, true);
            break;

        case CoilMode::heightPitch:
            Height.setStatus(App::Property::ReadOnly, false);
            Pitch.setStatus(App::Property::ReadOnly, false);
            Turns.setStatus(App::Property::ReadOnly, true);
            break;

        case CoilMode::spiral:
            Turns.setStatus(App::Property::ReadOnly, false);
            Pitch.setStatus(App::Property::ReadOnly, false);
            Height.setStatus(App::Property::ReadOnly, true);
            break;

        default:
            Turns.setStatus(App::Property::ReadOnly, false);
            Height.setStatus(App::Property::ReadOnly, false);
            Pitch.setStatus(App::Property::ReadOnly, false);
            break;
    }
}

PROPERTY_SOURCE(PartDesign::AdditiveCoil, PartDesign::Coil)
AdditiveCoil::AdditiveCoil()
{
    addSubType = Additive;
}

PROPERTY_SOURCE(PartDesign::SubtractiveCoil, PartDesign::Coil)
SubtractiveCoil::SubtractiveCoil()
{
    addSubType = Subtractive;
}
