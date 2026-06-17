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

#include <Precision.hxx>

#include <BRepOffset_MakeOffset.hxx>
#include <BRep_Tool.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

#include <App/Document.h>

#include "FeatureFaceOffset.h"


using namespace Part;

const char* Part::FaceOffset::JoinEnums[] = {"Arc", "Tangent", "Intersection", nullptr};

PROPERTY_SOURCE(Part::FaceOffset, Part::Feature)

FaceOffset::FaceOffset()
{
    ADD_PROPERTY_TYPE(Source, (nullptr), "FaceOffset", App::Prop_None, "Source shape");
    ADD_PROPERTY_TYPE(Value, (1.0), "FaceOffset", App::Prop_None, "Offset value");
    ADD_PROPERTY_TYPE(Join, (long(2)), "FaceOffset", App::Prop_None, "Join type");
    Join.setEnums(JoinEnums);
    ADD_PROPERTY_TYPE(Intersection, (false), "FaceOffset", App::Prop_None, "Intersection");

    Source.setScope(App::LinkScope::Global);
}

FaceOffset::~FaceOffset() = default;

short FaceOffset::mustExecute() const
{
    if (Source.isTouched()) {
        return 1;
    }
    if (Value.isTouched()) {
        return 1;
    }
    if (Join.isTouched()) {
        return 1;
    }
    if (Intersection.isTouched()) {
        return 1;
    }
    return 0;
}

App::DocumentObjectExecReturn* FaceOffset::execute()
{
    App::DocumentObject* source = Source.getValue();
    if (!source) {
        return new App::DocumentObjectExecReturn("No source shape linked.");
    }

    auto base = getTopoShape(source, ShapeOption::ResolveLink | ShapeOption::Transform);
    if (base.isNull()) {
        return new App::DocumentObjectExecReturn("Invalid source shape.");
    }

    if (base.countSubShapes(TopAbs_SOLID) != 1) {
        return new App::DocumentObjectExecReturn("Source shape must be a single solid.");
    }

    const auto& subs = Source.getSubValues(true);
    if (subs.empty()) {
        return new App::DocumentObjectExecReturn("No faces selected for offset.");
    }

    double offset = Value.getValue();
    bool inter = Intersection.getValue();
    auto join = static_cast<GeomAbs_JoinType>(Join.getValue());

    const TopoDS_Shape& shape = base.getShape();

    try {
        BRepOffset_MakeOffset builder;
        builder.Initialize(
            shape,
            1.0,
            Precision::Confusion(),
            BRepOffset_Skin,
            inter ? Standard_True : Standard_False,
            Standard_False,
            join
        );

        // Set all faces to zero offset (keep unchanged)
        TopExp_Explorer xp(shape, TopAbs_FACE);
        while (xp.More()) {
            builder.SetOffsetOnFace(TopoDS::Face(xp.Current()), 0.0);
            xp.Next();
        }

        // Set selected faces to desired offset
        for (const auto& sub : subs) {
            TopoShape faceShape = base.getSubTopoShape(sub.c_str());
            if (faceShape.isNull() || faceShape.getShape().ShapeType() != TopAbs_FACE) {
                return new App::DocumentObjectExecReturn("Invalid face selection.");
            }
            builder.SetOffsetOnFace(TopoDS::Face(faceShape.getShape()), offset);
        }

        builder.MakeOffsetShape();
        TopoDS_Shape result = builder.Shape();

        if (result.IsNull()) {
            return new App::DocumentObjectExecReturn("Offset operation failed to produce a result.");
        }

        TopoShape res;
        res.setShape(result);
        this->Shape.setValue(res);

        return App::DocumentObject::StdReturn;
    }
    catch (Standard_Failure& e) {
        return new App::DocumentObjectExecReturn(e.GetMessageString());
    }
    catch (Base::Exception& e) {
        return new App::DocumentObjectExecReturn(e.what());
    }
}
