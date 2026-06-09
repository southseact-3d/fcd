// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2025 liamh <liamh[at]users.sourceforge.net>             *
 *                                                                          *
 *   This file is part of FreeCAD.                                          *
 *                                                                          *
 *   FreeCAD is free software: you can redistribute it and/or modify it     *
 *   under the terms of the GNU Lesser General Public License as            *
 *   published by the Free Software Foundation, either version 2.1 of the   *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   FreeCAD is distributed in the hope that it will be useful, but         *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with FreeCAD. If not, see                                *
 *   <https://www.gnu.org/licenses/>.                                       *
 *                                                                          *
 ***************************************************************************/

#ifndef WIDGETSTYLEPARAMETERS_H
#define WIDGETSTYLEPARAMETERS_H

#include <Gui/StyleParameters/ParameterManager.h>

namespace Gui::StyleParameters::Widget
{
// Tool button (icon button in grouped toolbar)
DEFINE_STYLE_PARAMETER(ToolButtonBorderRadius, Gui::StyleParameters::Numeric(3));
DEFINE_STYLE_PARAMETER(ToolButtonHoverBackground, std::string("@PrimaryColorLighten4"));
DEFINE_STYLE_PARAMETER(ToolButtonHoverBorderColor, std::string("@GeneralBorderHoverColor"));
DEFINE_STYLE_PARAMETER(ToolButtonPressedBackground, std::string("@PrimaryColorDarken1"));

// Label / dropdown button (section label below tool buttons)
DEFINE_STYLE_PARAMETER(LabelBorderTopColor, std::string("@GeneralBorderColor"));
DEFINE_STYLE_PARAMETER(LabelFontSize, Gui::StyleParameters::Numeric(10));
DEFINE_STYLE_PARAMETER(LabelTextColor, std::string("@TextDisabledColor"));
DEFINE_STYLE_PARAMETER(LabelHoverColor, std::string("@AccentColor"));
}  // namespace Gui::StyleParameters::Widget

#endif  // WIDGETSTYLEPARAMETERS_H
