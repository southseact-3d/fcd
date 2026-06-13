// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten3D contributors                            *
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

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepOffsetAPI_MakePipeShell.hxx>
#include <BRepClass3d_SolidClassifier.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>

#include <BRepAdaptor_Curve.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>

#include <App/DocumentObject.h>
#include <Base/Exception.h>
#include <Base/Tools.h>
#include <Mod/Part/App/TopoShape.h>
#include <Mod/Part/App/Tools.h>
#include <Mod/Part/App/FaceMakerCheese.h>
#include <Mod/Part/App/FCBRepAlgoAPI_Fuse.h>

#include "FeatureThread.h"
#include "FeatureHole.h"

FC_LOG_LEVEL_INIT("PartDesign", true, true);

static gp_Pnt toPnt(gp_Vec dir)
{
    return {dir.X(), dir.Y(), dir.Z()};
}

static const char* ThreadClass_None_Local[] = {"None", nullptr};

using namespace PartDesign;

PROPERTY_SOURCE(PartDesign::Thread, PartDesign::DressUp)

const char* Thread::ThreadTypeEnums[] = {
    "ISOMetricProfile",
    "ISOMetricFineProfile",
    "UNC",
    "UNF",
    "UNEF",
    "NPT",
    "BSP",
    "BSW",
    "BSF",
    "ISOTyre",
    nullptr
};

const char* Thread::ThreadDirectionEnums[] = {"Right", "Left", nullptr};
const char* Thread::ThreadModeEnums[] = {"Auto", "External", "Internal", nullptr};
const char* Thread::ThreadDepthTypeEnums[] = {"Dimension", "ThroughFace", nullptr};

Thread::Thread()
{
    ADD_PROPERTY_TYPE(ThreadType, (0L), "Thread", App::Prop_None, "Thread standard");
    ThreadType.setEnums(ThreadTypeEnums);

    ADD_PROPERTY_TYPE(ThreadSize, (0L), "Thread", App::Prop_None, "Thread size designation");
    ThreadSize.setEnums(getThreadDesignations(ThreadType.getValue()));

    ADD_PROPERTY_TYPE(ThreadClass, (0L), "Thread", App::Prop_None, "Thread class");
    ThreadClass.setEnums(ThreadClass_None_Local);

    ADD_PROPERTY_TYPE(ThreadPitch, (0.0), "Thread", App::Prop_None, "Thread pitch (0 = auto from standard)");
    ThreadPitch.setUnit(Base::Unit::Length);

    ADD_PROPERTY_TYPE(ThreadDirection, (0L), "Thread", App::Prop_None, "Thread direction");
    ThreadDirection.setEnums(ThreadDirectionEnums);

    ADD_PROPERTY_TYPE(ThreadMode, (0L), "Thread", App::Prop_None, "Thread mode");
    ThreadMode.setEnums(ThreadModeEnums);

    ADD_PROPERTY_TYPE(ThreadLength, (10.0), "Thread", App::Prop_None, "Thread length along face");
    ThreadLength.setUnit(Base::Unit::Length);

    ADD_PROPERTY_TYPE(ThreadDepthType, (0L), "Thread", App::Prop_None, "Thread depth type");
    ThreadDepthType.setEnums(ThreadDepthTypeEnums);

    ADD_PROPERTY_TYPE(ThreadDepth, (0.0), "Thread", App::Prop_None, "Thread depth (0 = auto)");
    ThreadDepth.setUnit(Base::Unit::Length);

    ADD_PROPERTY_TYPE(UseCustomClearance, (false), "Thread", App::Prop_None, "Use custom thread clearance");
    ADD_PROPERTY_TYPE(CustomClearance, (0.0), "Thread", App::Prop_None, "Custom thread clearance");

    ADD_PROPERTY_TYPE(TaperAngle, (0.0), "Thread", App::Prop_None, "Taper angle (0 = straight)");
    TaperAngle.setUnit(Base::Unit::Angle);
}

short Thread::mustExecute() const
{
    if (ThreadType.isTouched() || ThreadSize.isTouched() || ThreadPitch.isTouched()
        || ThreadDirection.isTouched() || ThreadMode.isTouched() || ThreadLength.isTouched()
        || ThreadDepth.isTouched() || ThreadDepthType.isTouched()
        || UseCustomClearance.isTouched() || CustomClearance.isTouched()
        || TaperAngle.isTouched() || Base.isTouched()) {
        return 1;
    }
    return DressUp::mustExecute();
}

void Thread::onChanged(const App::Property* prop)
{
    if (prop == &ThreadType) {
        updateThreadSizeEnums();
        updateThreadClassEnums();
    }
    else if (prop == &ThreadSize) {
        int type = ThreadType.getValue();
        int size = ThreadSize.getValue();
        if (type >= 0 && type < 11 && size >= 0
            && size < static_cast<int>(Hole::threadDescription[type].size())) {
            const auto& desc = Hole::threadDescription[type][size];
            if (ThreadPitch.getValue() == 0.0) {
                ThreadPitch.setValue(desc.pitch);
            }
        }
    }

    DressUp::onChanged(prop);
}

void Thread::updateThreadSizeEnums()
{
    ThreadSize.setEnums(getThreadDesignations(ThreadType.getValue()));
}

void Thread::updateThreadClassEnums()
{
    ThreadClass.setEnums(ThreadClass_None_Local);
}

App::DocumentObjectExecReturn* Thread::execute()
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

    TopShape.setTransform(Base::Matrix4D());

    auto faces = getFaces(TopShape);

    if (faces.empty()) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "No faces selected for thread")
        );
    }

    this->positionByBaseFeature();

    int threadType = ThreadType.getValue();
    int threadSize = ThreadSize.getValue();

    if (threadType < 0 || threadType >= 11) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Invalid thread type")
        );
    }
    if (threadSize < 0
        || threadSize >= static_cast<int>(Hole::threadDescription[threadType].size())) {
        return new App::DocumentObjectExecReturn(
            QT_TRANSLATE_NOOP("Exception", "Invalid thread size")
        );
    }

    try {
        TopoShape result = TopShape;

        for (const auto& faceShape : faces) {
            TopoDS_Face face = TopoDS::Face(faceShape.getShape());
            if (face.IsNull()) {
                continue;
            }

            BRepAdaptor_Surface surface(face);
            if (surface.GetType() != GeomAbs_Cylinder && surface.GetType() != GeomAbs_Cone) {
                continue;
            }

            gp_Ax1 faceAxis;
            double faceRadius;
            if (surface.GetType() == GeomAbs_Cylinder) {
                faceAxis = surface.Cylinder().Axis();
                faceRadius = surface.Cylinder().Radius();
            }
            else {
                faceAxis = surface.Cone().Axis();
                faceRadius = surface.Cone().Radius();
            }

            gp_Dir axisDir = faceAxis.Direction();
            gp_Pnt axisLoc = faceAxis.Location();

            FaceThreadInfo info;
            info.axis = axisDir;
            info.axisLocation = axisLoc;
            info.radius = faceRadius;

            bool isInternal = false;
            int mode = ThreadMode.getValue();
            if (mode == 1) {
                isInternal = false;
            }
            else if (mode == 2) {
                isInternal = true;
            }
            else {
                GProp_GProps props;
                BRepGProp::VolumeProperties(TopDS::Face(faceShape.getShape()), props);
                gp_Pnt center = props.CentreOfMass();

                gp_Vec toCenter(axisLoc.X() - center.X(), axisLoc.Y() - center.Y(),
                                axisLoc.Z() - center.Z());
                gp_Dir faceNormal = axisDir;
                if (faceNormal.Dot(toCenter) > 0) {
                    isInternal = true;
                }
            }
            info.isInternal = isInternal;

            if (isInternal) {
                TopExp_Explorer exp(face, TopAbs_EDGE);
                if (exp.More()) {
                    TopoDS_Edge edge = TopoDS::Edge(exp.Current());
                    BRepAdaptor_Curve curve(edge);
                    if (curve.GetType() == GeomAbs_Circle) {
                        info.radius = curve.Circle().Radius();
                    }
                }
            }

            TopoDS_Shape threadSolid = buildThreadSolid(info);
            if (threadSolid.IsNull()) {
                continue;
            }

            try {
                result.makeElementBoolean(
                    "fuse", {result, Part::TopoShape(0, threadSolid)}
                );
            }
            catch (...) {
                FCBRepAlgoAPI_Fuse mkFuse(result.getShape(), threadSolid);
                if (mkFuse.IsDone()) {
                    result = TopoShape(0, mkFuse.Shape());
                }
            }
        }

        result = refineShapeIfActive(result);

        if (!isSingleSolidRuleSatisfied(result.getShape())) {
            return new App::DocumentObjectExecReturn(QT_TRANSLATE_NOOP(
                "Exception",
                "Result has multiple solids: enable 'Allow Compound' in the active body."
            ));
        }

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

TopoDS_Shape Thread::buildThreadSolid(const FaceThreadInfo& info)
{
    int threadType = ThreadType.getValue();
    int threadSize = ThreadSize.getValue();

    const auto& desc = Hole::threadDescription[threadType][threadSize];
    double Rmaj = desc.diameter / 2.0;
    double Pitch = ThreadPitch.getValue();
    if (Pitch <= 0.0) {
        Pitch = desc.pitch;
    }
    if (Pitch <= 0.0) {
        return TopoDS_Shape();
    }

    bool leftHanded = (ThreadDirection.getValue() == 1);

    double clearance = 0.0;
    if (UseCustomClearance.getValue()) {
        clearance = CustomClearance.getValue() / 2.0;
    }
    double RmajC = Rmaj + clearance;

    std::string threadTypeStr = ThreadType.getValueAsString();
    bool isBSP = (threadTypeStr == "BSP" || threadTypeStr == "BSW" || threadTypeStr == "BSF");

    BRepBuilderAPI_MakeWire mkThreadWire;
    double H;
    double marginZ = 0.001;

    if (isBSP) {
        H = 0.960491 * Pitch;
        double radius = 0.137329 * Pitch;
        double marginX = std::tan(Base::toRadians(62.5)) * marginZ;

        gp_Pnt p1 = toPnt((RmajC - 5 * H / 6 + marginX) * gp_Vec(1, 0, 0) + marginZ * gp_Vec(0, 0, 1));
        gp_Pnt p4 = toPnt((RmajC - 5 * H / 6 + marginX) * gp_Vec(1, 0, 0) + (Pitch - marginZ) * gp_Vec(0, 0, 1));

        double p23x = RmajC - radius * 0.58284013094;

        gp_Pnt p2 = toPnt(p23x * gp_Vec(1, 0, 0) + 3 * Pitch / 8 * gp_Vec(0, 0, 1));
        gp_Pnt p3 = toPnt(p23x * gp_Vec(1, 0, 0) + 5 * Pitch / 8 * gp_Vec(0, 0, 1));
        gp_Pnt crest = toPnt(RmajC * gp_Vec(1, 0, 0) + Pitch / 2 * gp_Vec(0, 0, 1));

        mkThreadWire.Add(BRepBuilderAPI_MakeEdge(p1, p2).Edge());
        Handle(Geom_TrimmedCurve) arc1 = GC_MakeArcOfCircle(p2, crest, p3).Value();
        mkThreadWire.Add(BRepBuilderAPI_MakeEdge(arc1).Edge());
        mkThreadWire.Add(BRepBuilderAPI_MakeEdge(p3, p4).Edge());
        mkThreadWire.Add(BRepBuilderAPI_MakeEdge(p4, p1).Edge());
    }
    else {
        H = std::sqrt(3.0) / 2.0 * Pitch;
        double h = 7.0 * H / 8.0;
        double marginX = std::tan(Base::toRadians(60.0)) * marginZ;

        gp_Pnt p1 = toPnt((RmajC - h + marginX) * gp_Vec(1, 0, 0) + marginZ * gp_Vec(0, 0, 1));
        gp_Pnt p2 = toPnt(RmajC * gp_Vec(1, 0, 0) + 7.0 * Pitch / 16.0 * gp_Vec(0, 0, 1));
        gp_Pnt p3 = toPnt(RmajC * gp_Vec(1, 0, 0) + 9.0 * Pitch / 16.0 * gp_Vec(0, 0, 1));
        gp_Pnt p4 = toPnt((RmajC - h + marginX) * gp_Vec(1, 0, 0) + (Pitch - marginZ) * gp_Vec(0, 0, 1));

        mkThreadWire.Add(BRepBuilderAPI_MakeEdge(p1, p2).Edge());
        if (threadTypeStr == "ISOTyre") {
            gp_Pnt crest = toPnt((RmajC + Pitch / 32.0) * gp_Vec(1, 0, 0) + Pitch / 2.0 * gp_Vec(0, 0, 1));
            Handle(Geom_TrimmedCurve) arc1 = GC_MakeArcOfCircle(p2, crest, p3).Value();
            mkThreadWire.Add(BRepBuilderAPI_MakeEdge(arc1).Edge());
        }
        else {
            mkThreadWire.Add(BRepBuilderAPI_MakeEdge(p2, p3).Edge());
        }
        mkThreadWire.Add(BRepBuilderAPI_MakeEdge(p3, p4).Edge());
        mkThreadWire.Add(BRepBuilderAPI_MakeEdge(p4, p1).Edge());
    }

    mkThreadWire.Build();
    TopoDS_Wire threadWire = mkThreadWire.Wire();

    double threadLen = ThreadLength.getValue();
    if (threadLen <= 0.0) {
        threadLen = Pitch * 5.0;
    }
    if (ThreadDepthType.getValue() == 1) {
        threadLen = Pitch * 5.0;
    }
    double helixLength = threadLen + Pitch / 2.0;

    TopoDS_Shape helix = TopoShape().makeLongHelix(Pitch, helixLength, Rmaj, 0.0, leftHanded);

    gp_Pnt origo(0.0, 0.0, 0.0);
    gp_Dir dir_axis1(0.0, 0.0, 1.0);
    gp_Dir dir_axis2(1.0, 0.0, 0.0);

    gp_Trsf mov;
    mov.SetRotation(gp_Ax1(origo, dir_axis2), std::numbers::pi);
    TopLoc_Location loc1(mov);
    helix.Move(loc1);

    rotateToNormal(dir_axis1, info.axis, helix);

    gp_Trsf translation;
    translation.SetTranslation(gp_Vec(info.axisLocation.X(), info.axisLocation.Y(), info.axisLocation.Z()));
    TopLoc_Location loc2(translation);
    helix.Move(loc2);

    BRepOffsetAPI_MakePipeShell mkPS(TopoDS::Wire(helix));
    mkPS.SetTolerance(Precision::Confusion());
    mkPS.SetTransitionMode(BRepBuilderAPI_Transformed);
    mkPS.SetMode(true);
    mkPS.Add(threadWire);
    if (!mkPS.IsReady()) {
        return TopoDS_Shape();
    }
    TopoDS_Shape shell = mkPS.Shape();

    TopTools_ListOfShape sim;
    mkPS.Simulate(2, sim);
    std::vector<TopoDS_Wire> frontwires, backwires;
    frontwires.push_back(TopoDS::Wire(sim.First()));
    backwires.push_back(TopoDS::Wire(sim.Last()));

    TopoDS_Shape front = Part::FaceMakerCheese::makeFace(frontwires);
    TopoDS_Shape back = Part::FaceMakerCheese::makeFace(backwires);

    BRepBuilderAPI_Sewing sewer;
    sewer.SetTolerance(Precision::Confusion());
    sewer.Add(front);
    sewer.Add(back);
    sewer.Add(shell);
    sewer.Perform();

    BRepBuilderAPI_MakeSolid mkSolid;
    mkSolid.Add(TopoDS::Shell(sewer.SewedShape()));
    if (!mkSolid.Done()) {
        return TopoDS_Shape();
    }
    TopoDS_Shape result = mkSolid.Shape();

    BRepClass3d_SolidClassifier SC(result);
    SC.PerformInfinitePoint(Precision::Confusion());
    if (SC.State() == TopAbs_IN) {
        result.Reverse();
    }

    return result;
}

void Thread::rotateToNormal(const gp_Dir& helixAxis, const gp_Dir& normalAxis, TopoDS_Shape& helixShape) const
{
    if (helixAxis.IsEqual(normalAxis, Precision::Angular())) {
        return;
    }

    gp_Dir rotAxis;
    double angle;

    if (helixAxis.IsOpposite(normalAxis, Precision::Angular())) {
        gp_XYZ xyz(helixAxis.XYZ());
        if (std::abs(xyz.X()) <= std::abs(xyz.Y()) && std::abs(xyz.X()) <= std::abs(xyz.Z())) {
            xyz.SetX(1.0);
        }
        else if (std::abs(xyz.Y()) <= std::abs(xyz.X()) && std::abs(xyz.Y()) <= std::abs(xyz.Z())) {
            xyz.SetY(1.0);
        }
        else {
            xyz.SetZ(1.0);
        }
        rotAxis = helixAxis.Crossed(gp_Dir(xyz));
        angle = std::numbers::pi;
    }
    else {
        rotAxis = helixAxis.Crossed(normalAxis);
        angle = acos(helixAxis * normalAxis);
    }

    gp_Pnt origo(0.0, 0.0, 0.0);
    gp_Trsf trsf = helixShape.Location().Transformation();
    trsf.SetRotation(gp_Ax1(origo, rotAxis), angle);
    TopLoc_Location loc(trsf);
    helixShape.Move(loc);
}
