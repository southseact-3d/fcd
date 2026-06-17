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

#ifndef PARTDESIGN_FeaturePatternOnPath_H
#define PARTDESIGN_FeaturePatternOnPath_H

#include <App/PropertyUnits.h>
#include "FeatureTransformed.h"

namespace PartDesign
{

enum class PathPatternMode
{
    Extent,
    Spacing
};

enum class PathAlignment
{
    None,
    Tangent,
    Frenet
};

class PartDesignExport PatternOnPath: public PartDesign::Transformed
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::PatternOnPath);

public:
    PatternOnPath();

    App::PropertyLinkSub Path;
    App::PropertyBool Reversed;
    App::PropertyEnumeration Mode;
    App::PropertyLength Length;
    App::PropertyLength Offset;
    App::PropertyIntegerConstraint Occurrences;
    App::PropertyLength StartOffset;
    App::PropertyLength EndOffset;
    App::PropertyEnumeration Alignment;
    App::PropertyFloatList Spacings;
    App::PropertyFloatList SpacingPattern;

    short mustExecute() const override;

    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderPatternOnPath";
    }

    const std::list<gp_Trsf> getTransformations(const std::vector<App::DocumentObject*>) override;

protected:
    void onChanged(const App::Property* prop) override;

private:
    static const App::PropertyIntegerConstraint::Constraints intOccurrences;
    static const char* ModeEnums[];
    static const char* AlignmentEnums[];

    void setReadWriteStatusForMode();
    void syncLengthAndOffset();
    void updateSpacings();

    gp_Dir getPathDirection() const;
};

}  // namespace PartDesign

#endif  // PARTDESIGN_FeaturePatternOnPath_H
