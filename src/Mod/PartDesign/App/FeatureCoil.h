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


#ifndef PARTDESIGN_Coil_H
#define PARTDESIGN_Coil_H

#include <App/PropertyUnits.h>
#include "FeatureAddSub.h"
#include <Mod/Part/App/AttachExtension.h>
#include <TopoDS_Shape.hxx>

namespace PartDesign
{

enum class CoilSectionShape
{
    circular = 0,
    square,
    triangularExternal,
    triangularInternal
};

enum class CoilMode
{
    revolutionHeight = 0,
    revolutionPitch,
    heightPitch,
    spiral
};

enum class CoilSectionPosition
{
    inside = 0,
    onCenter,
    outside
};

class PartDesignExport Coil: public FeatureAddSub, public Part::AttachExtension
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::Coil);

public:
    Coil();

    App::PropertyVector Base;
    App::PropertyVector Axis;
    App::PropertyLength Radius;
    App::PropertyLength Pitch;
    App::PropertyLength Height;
    App::PropertyFloatConstraint Turns;
    App::PropertyAngle Angle;
    App::PropertyLength SectionSize;
    App::PropertyEnumeration SectionShape;
    App::PropertyEnumeration SectionPosition;
    App::PropertyEnumeration Mode;
    App::PropertyBool LeftHanded;
    App::PropertyBool Reversed;
    App::PropertyFloatConstraint Tolerance;

    App::PropertyLinkSub ReferenceAxis;

    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;

    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderCoil";
    }

protected:
    void updateAxis();
    TopoDS_Shape generateHelixPath(double breakAtTurn = 1.);
    TopoDS_Shape generateProfile();

    void onChanged(const App::Property* prop) override;

    static const App::PropertyFloatConstraint::Constraints floatTurns;
    static const App::PropertyAngle::Constraints floatAngle;
    static const App::PropertyFloatConstraint::Constraints floatTolerance;

private:
    static const char* ModeEnums[];
    static const char* SectionShapeEnums[];
    static const char* SectionPositionEnums[];

    void setReadWriteStatusForMode(CoilMode inputMode);
};


class PartDesignExport AdditiveCoil: public Coil
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::AdditiveCoil);

public:
    AdditiveCoil();
};


class PartDesignExport SubtractiveCoil: public Coil
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::SubtractiveCoil);

public:
    SubtractiveCoil();
};

}  // namespace PartDesign


#endif  // PARTDESIGN_Coil_H
