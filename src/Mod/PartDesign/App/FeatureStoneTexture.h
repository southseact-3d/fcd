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


#ifndef PARTDESIGN_FEATURESTONETEXTURE_H
#define PARTDESIGN_FEATURESTONETEXTURE_H

#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include "FeatureDressUp.h"

namespace PartDesign
{

class PartDesignExport StoneTexture: public DressUp
{
    PROPERTY_HEADER_WITH_OVERRIDE(PartDesign::StoneTexture);

public:
    StoneTexture();

    App::PropertyEnumeration StonePattern;
    App::PropertyEnumeration StoneScale;
    App::PropertyQuantityConstraint StoneSize;
    App::PropertyQuantityConstraint StoneDepth;
    App::PropertyPercent DepthVariation;
    App::PropertyPercent Roughness;
    App::PropertyQuantityConstraint MortarThickness;
    App::PropertyQuantityConstraint MortarDepth;
    App::PropertyInteger Seed;
    App::PropertyEnumeration Scale;

    App::DocumentObjectExecReturn* execute() override;
    short mustExecute() const override;

    const char* getViewProviderName() const override
    {
        return "PartDesignGui::ViewProviderStoneTexture";
    }

protected:
    void onChanged(const App::Property* prop) override;

private:
    static const App::PropertyQuantityConstraint::Constraints floatSize;
    static const App::PropertyIntegerConstraint::Constraints intPercent;
    static const char* ScaleEnums[];
    static const char* PatternEnums[];
    static const char* StoneScaleEnums[];

    App::DocumentObjectExecReturn* executeAshlar(
        const Part::TopoShape& TopShape,
        const std::vector<Part::TopoShape>& faces,
        double scaleFactor);
    App::DocumentObjectExecReturn* executeRubble(
        const Part::TopoShape& TopShape,
        const std::vector<Part::TopoShape>& faces,
        double scaleFactor);
};

}  // namespace PartDesign


#endif  // PARTDESIGN_FEATURESTONETEXTURE_H
