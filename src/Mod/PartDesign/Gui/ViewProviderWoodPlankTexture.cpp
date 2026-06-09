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


#include "TaskWoodPlankTextureParameters.h"
#include "ViewProviderWoodPlankTexture.h"

using namespace PartDesignGui;

PROPERTY_SOURCE(PartDesignGui::ViewProviderWoodPlankTexture, PartDesignGui::ViewProviderDressUp)


const std::string& ViewProviderWoodPlankTexture::featureName() const
{
    static const std::string name = "WoodPlankTexture";
    return name;
}

void ViewProviderWoodPlankTexture::setupContextMenu(QMenu* menu, QObject* receiver, const char* member)
{
    addDefaultAction(menu, QObject::tr("Edit Wood Plank Texture"));
    PartDesignGui::ViewProvider::setupContextMenu(menu, receiver, member);
}

TaskDlgFeatureParameters* ViewProviderWoodPlankTexture::getEditDialog()
{
    return new TaskDlgWoodPlankTextureParameters(this);
}
