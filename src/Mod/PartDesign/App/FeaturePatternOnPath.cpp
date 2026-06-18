// SPDX-License-Identifier: LGPL-2.1-or-later

/******************************************************************************
 *                                                                            *
 *   Copyright (c) 2025 Tungsten CAD Contributors                             *
 *                                                                            *
 *   This file is part of the FreeCAD CAx development system.                 *
 *                                                                            *
 *   This library is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU Library General Public              *
 *   License as published by the Free Software Foundation; either             *
 *   version 2 of the License, or (at your option) any later version.         *
 *                                                                            *
 *   This library  is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU Library General Public License for more details.                     *
 *                                                                            *
 *   You should have received a copy of the GNU Library General Public        *
 *   License along with this library; see the file COPYING.LIB. If not,       *
 *   write to the Free Software Foundation, Inc., 59 Temple Place,            *
 *   Suite 330, Boston, MA  02111-1307, USA                                   *
 *                                                                            *
 ******************************************************************************/

#include <limits>

#include <BRepAdaptor_Curve.hxx>
#include <BRepLProp_CLProps.hxx>
#include <gp_Quaternion.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Wire.hxx>

#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/Tools.h>

#include "FeaturePatternOnPath.h"


using namespace PartDesign;

namespace PartDesign
{

PROPERTY_SOURCE(PartDesign::PatternOnPath, PartDesign::Transformed)

const App::PropertyIntegerConstraint::Constraints PatternOnPath::intOccurrences
    = {1, std::numeric_limits<int>::max(), 1};

const char* PatternOnPath::ModeEnums[] = {"Extent", "Spacing", nullptr};

const char* PatternOnPath::AlignmentEnums[] = {"None", "Tangent", "Frenet", nullptr};

PatternOnPath::PatternOnPath()
{
    ADD_PROPERTY_TYPE(
        Path,
        (nullptr),
        "Path",
        App::Prop_None,
        "The path to follow. This can be a straight edge, a sketch edge, "
        "or any wire/edge feature."
    );
    ADD_PROPERTY_TYPE(
        Reversed,
        (0),
        "Path",
        App::Prop_None,
        "Reverse the direction along the path"
    );
    ADD_PROPERTY_TYPE(
        Mode,
        (0L),
        "Path",
        App::Prop_None,
        "Selects how the pattern is dimensioned along the path.\n'Extent': Uses the "
        "total distance from the first to the last instance.\n'Spacing': Uses the "
        "distance between consecutive instances."
    );
    Mode.setEnums(ModeEnums);
    ADD_PROPERTY_TYPE(
        Length,
        (100.0),
        "Path",
        App::Prop_None,
        "The total distance along the path, measured from the first to the last "
        "instance. Only used when Mode is 'Extent'."
    );
    ADD_PROPERTY_TYPE(
        Offset,
        (10.0),
        "Path",
        App::Prop_None,
        "The distance between each instance along the path. Only used when Mode "
        "is 'Spacing'."
    );
    ADD_PROPERTY_TYPE(
        Occurrences,
        (2),
        "Path",
        App::Prop_None,
        "The total number of instances, including the original."
    );
    Occurrences.setConstraints(&intOccurrences);
    ADD_PROPERTY_TYPE(
        StartOffset,
        (0.0),
        "Path",
        App::Prop_None,
        "Offset distance from the start of the path to the first instance."
    );
    ADD_PROPERTY_TYPE(
        EndOffset,
        (0.0),
        "Path",
        App::Prop_None,
        "Offset distance from the end of the path to the last instance."
    );
    ADD_PROPERTY_TYPE(
        Alignment,
        (0L),
        "Path",
        App::Prop_None,
        "Controls how instances are oriented along the path.\n'None': Keep the "
        "original orientation.\n'Tangent': Align X axis to path tangent.\n'Frenet': "
        "Align using Frenet frame (tangent + normal)."
    );
    Alignment.setEnums(AlignmentEnums);
    ADD_PROPERTY_TYPE(
        Spacings,
        ({-1.0}),
        "Path",
        App::Prop_None,
        "Custom spacings between instances. -1 uses the global Offset."
    );
    ADD_PROPERTY_TYPE(
        SpacingPattern,
        ({}),
        "Path",
        App::Prop_None,
        "Repeating pattern of spacings. E.g., [10, 20] alternates 10mm and 20mm."
    );

    setReadWriteStatusForMode();
}

short PatternOnPath::mustExecute() const
{
    if (Path.isTouched() || Reversed.isTouched() || Mode.isTouched() || Length.isTouched()
        || Offset.isTouched() || Occurrences.isTouched() || StartOffset.isTouched()
        || EndOffset.isTouched() || Alignment.isTouched() || Spacings.isTouched()
        || SpacingPattern.isTouched() || SuppressedInstances.isTouched()) {
        return 1;
    }
    return Transformed::mustExecute();
}

void PatternOnPath::setReadWriteStatusForMode()
{
    bool isExtent = (Mode.getValue() == static_cast<long>(PathPatternMode::Extent));
    Length.setReadOnly(!isExtent);
    Offset.setReadOnly(isExtent);
}

void PatternOnPath::syncLengthAndOffset()
{
    auto mode = static_cast<PathPatternMode>(Mode.getValue());
    int occurrences = Occurrences.getValue();
    int steps = occurrences <= 1 ? 1 : occurrences - 1;

    if (mode == PathPatternMode::Spacing
        && !Length.testStatus(App::Property::Status::Immutable)) {
        Length.setValue(Offset.getValue() * steps);
    }
    else if (mode == PathPatternMode::Extent
             && !Offset.testStatus(App::Property::Status::Immutable)) {
        Offset.setValue(Length.getValue() / steps);
    }
}

void PatternOnPath::updateSpacings()
{
    std::vector<double> spacings = Spacings.getValues();
    size_t targetCount = Occurrences.getValue() - 1;

    for (auto& spacing : spacings) {
        if (spacing == Offset.getValue()) {
            spacing = -1.0;
        }
    }

    if (spacings.size() == targetCount) {
        return;
    }
    if (spacings.size() < targetCount) {
        spacings.reserve(targetCount);
        while (spacings.size() < targetCount) {
            spacings.push_back(-1.0);
        }
    }
    else {
        spacings.resize(targetCount);
    }

    Spacings.setValues(spacings);
}

gp_Dir PatternOnPath::getPathDirection() const
{
    App::DocumentObject* pathObj = Path.getValue();
    if (!pathObj) {
        throw Base::ValueError("No path specified");
    }

    std::vector<std::string> subStrings = Path.getSubValues();
    if (subStrings.empty()) {
        throw Base::ValueError("No path sub-element specified");
    }

    Part::TopoShape pathShape = Part::Feature::getTopoShape(pathObj, Part::ShapeOption::ResolveLink);
    if (pathShape.isNull()) {
        throw Base::ValueError("Path shape is null");
    }

    TopoDS_Shape ref = pathShape.getShape();
    if (ref.ShapeType() == TopAbs_EDGE) {
        TopoDS_Edge edge = TopoDS::Edge(ref);
        BRepAdaptor_Curve adapt(edge);
        if (adapt.GetType() != GeomAbs_Line) {
            throw Base::TypeError("Path direction edge must be a straight line");
        }
        gp_Dir dir = adapt.Line().Direction();
        if (Reversed.getValue()) {
            dir.Reverse();
        }
        return dir;
    }
    else if (ref.ShapeType() == TopAbs_WIRE) {
        // For a wire, use the direction from start to end
        TopoDS_Wire wire = TopoDS::Wire(ref);
        TopExp_Explorer explorer(wire, TopAbs_EDGE);
        if (explorer.More()) {
            TopoDS_Edge firstEdge = TopoDS::Edge(explorer.Current());
            BRepAdaptor_Curve adapt(firstEdge);
            gp_Dir dir = adapt.Line().Direction();
            if (Reversed.getValue()) {
                dir.Reverse();
            }
            return dir;
        }
    }

    throw Base::ValueError("Could not determine path direction");
}

const std::list<gp_Trsf> PatternOnPath::getTransformations(
    const std::vector<App::DocumentObject*>
)
{
    int occurrences = Occurrences.getValue();
    if (occurrences < 1) {
        throw Base::ValueError("At least one occurrence required");
    }

    if (occurrences == 1) {
        return {gp_Trsf()};
    }

    App::DocumentObject* pathObj = Path.getValue();
    if (!pathObj) {
        throw Base::ValueError("No path specified");
    }

    // Get the path edge
    Part::TopoShape pathShape = Part::Feature::getTopoShape(pathObj, Part::ShapeOption::ResolveLink);
    if (pathShape.isNull()) {
        throw Base::ValueError("Path shape is null");
    }

    // Try to get a single edge from the shape
    TopoDS_Shape rawShape = pathShape.getShape();
    TopoDS_Edge pathEdge;

    if (rawShape.ShapeType() == TopAbs_EDGE) {
        pathEdge = TopoDS::Edge(rawShape);
    }
    else if (rawShape.ShapeType() == TopAbs_WIRE) {
        // For a wire, take the first edge
        TopExp_Explorer explorer(rawShape, TopAbs_EDGE);
        if (explorer.More()) {
            pathEdge = TopoDS::Edge(explorer.Current());
        }
        else {
            throw Base::ValueError("Path wire has no edges");
        }
    }
    else {
        throw Base::ValueError("Path must be an edge or wire");
    }

    BRepAdaptor_Curve adapt(pathEdge);

    // Get total path length
    double totalLength = GCPnts_AbscissaPoint::Length(adapt, Precision::Confusion());

    // Apply start/end offsets
    double startOff = StartOffset.getValue();
    double endOff = EndOffset.getValue();
    double availableLength = totalLength - startOff - endOff;

    if (availableLength <= Precision::Confusion()) {
        throw Base::ValueError("Path length too short after applying offsets");
    }

    auto mode = static_cast<PathPatternMode>(Mode.getValue());
    auto alignment = static_cast<PathAlignment>(Alignment.getValue());
    bool reversed = Reversed.getValue();

    // Build list of distances along the path
    std::vector<double> distances;
    distances.reserve(occurrences);

    if (mode == PathPatternMode::Extent) {
        double totalExtent = Length.getValue();
        if (totalExtent < Precision::Confusion()) {
            totalExtent = availableLength;
        }
        for (int i = 0; i < occurrences; ++i) {
            double fraction = (occurrences <= 1) ? 0.0
                : static_cast<double>(i) / (occurrences - 1);
            distances.push_back(startOff + fraction * totalExtent);
        }
    }
    else {  // Spacing mode
        const std::vector<double> spacings = Spacings.getValues();
        const std::vector<double> pattern = SpacingPattern.getValues();
        bool usePattern = pattern.size() > 1;

        const auto spacingAt = [&](unsigned i) {
            if (!spacings.empty() && i < spacings.size() && spacings[i] != -1.0) {
                return spacings[i];
            }
            if (usePattern) {
                return pattern.at(static_cast<size_t>(fmod(i, pattern.size())));
            }
            return Offset.getValue();
        };

        double cumulative = startOff;
        distances.push_back(cumulative);
        for (int i = 1; i < occurrences; ++i) {
            cumulative += spacingAt(i - 1);
            if (cumulative > totalLength - endOff) {
                break;  // Stop if we exceed the path
            }
            distances.push_back(cumulative);
        }
    }

    // Build transformations
    std::list<gp_Trsf> transformations;

    for (size_t idx = 0; idx < distances.size(); ++idx) {
        double dist = distances[idx];

        // Clamp to valid range
        dist = std::max(startOff, std::min(dist, totalLength - endOff));

        // Get parameter at this distance
        GCPnts_AbscissaPoint abscissa(Precision::Confusion(), adapt, dist, adapt.FirstParameter());
        double param = abscissa.Parameter();

        // Get point on curve
        gp_Pnt point;
        gp_Vec tangent;
        adapt.D1(param, point, tangent);

        if (reversed) {
            tangent.Reverse();
        }

        gp_Trsf trans;

        if (alignment == PathAlignment::None) {
            // Just translate, no rotation
            trans.SetTranslation(gp_Vec(point.XYZ()));
        }
        else {
            // Build rotation from tangent/normal frame
            gp_Dir tangentDir(tangent);
            gp_Dir zDir(0, 0, 1);

            // Compute a normal perpendicular to the tangent
            gp_Dir normal;
            if (alignment == PathAlignment::Frenet) {
                // For Frenet, use curve normal if available
                try {
                    BRepLProp_CLProps props(adapt, param, 1, Precision::Confusion());
                    gp_Dir normalDir;
                    props.Normal(normalDir);
                    normal = normalDir;
                }
                catch (...) {
                    // Fallback: use cross product with Z
                    gp_Vec cross = gp_Vec(tangentDir).Crossed(gp_Vec(zDir));
                    if (cross.Magnitude() > Precision::Confusion()) {
                        normal = gp_Dir(cross);
                    }
                    else {
                        cross = gp_Vec(tangentDir).Crossed(gp_Vec(gp_Dir(1, 0, 0)));
                        normal = gp_Dir(cross);
                    }
                }
            }
            else {  // Tangent alignment
                gp_Vec cross = gp_Vec(tangentDir).Crossed(gp_Vec(zDir));
                if (cross.Magnitude() > Precision::Confusion()) {
                    normal = gp_Dir(cross);
                }
                else {
                    cross = gp_Vec(tangentDir).Crossed(gp_Vec(gp_Dir(1, 0, 0)));
                    normal = gp_Dir(cross);
                }
            }

            gp_Dir binormal = tangentDir.Crossed(normal);

            // Build rotation matrix from frame
            gp_Mat rotationMatrix(
                tangentDir.X(), normal.X(), binormal.X(),
                tangentDir.Y(), normal.Y(), binormal.Y(),
                tangentDir.Z(), normal.Z(), binormal.Z()
            );

            trans.SetRotation(gp_Quaternion(rotationMatrix));
            trans.SetTranslationPart(point.XYZ());
        }

        transformations.push_back(trans);
    }

    return transformations;
}

void PatternOnPath::onChanged(const App::Property* prop)
{
    if (prop == &Mode) {
        setReadWriteStatusForMode();
    }
    else if (prop == &Occurrences) {
        updateSpacings();
        syncLengthAndOffset();
    }
    else if (prop == &Offset && Mode.getValue() == static_cast<long>(PathPatternMode::Spacing)) {
        syncLengthAndOffset();
    }
    else if (prop == &Length && Mode.getValue() == static_cast<long>(PathPatternMode::Extent)) {
        syncLengthAndOffset();
    }

    Transformed::onChanged(prop);
}

}  // namespace PartDesign
