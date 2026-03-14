// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2025 FreeCAD contributors                               *
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

#ifndef INSPECTIOGUI_VIEWPROVIDERWALLTHICKNESS_H
#define INSPECTIOGUI_VIEWPROVIDERWALLTHICKNESS_H

#include <Gui/ViewProviderDocumentObject.h>
#include <App/PropertyStandard.h>

class SoSeparator;
class SoCoordinate3;
class SoMaterial;
class SoMaterialBinding;
class SoPointSet;
class SoDrawStyle;
class SoTransform;

namespace Gui
{
class SoFCColorBar;
}

namespace InspectionGui
{

/**
 * @brief View provider for wall thickness analysis with heat map visualization
 */
class ViewProviderWallThickness : public Gui::ViewProviderDocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(InspectionGui::ViewProviderWallThickness);

public:
    ViewProviderWallThickness();
    ~ViewProviderWallThickness() override;

    void attach(App::DocumentObject* pcFeat) override;
    void setDisplayMode(const char* ModeName) override;
    std::vector<std::string> getDisplayModes() const override;
    void updateData(const App::Property* prop) override;
    void onChanged(const App::Property* prop) override;
    QIcon getIcon() const override;
    SoSeparator* getFrontRoot() const override;

    /// Show/hide the visualization
    void show() override;
    void hide() override;

protected:
    void setupVisualization();
    void updateHeatMapColors();
    void clearVisualization();
    
    /// Convert thickness value to RGB color for heat map
    void thicknessToColor(double thickness, float& r, float& g, float& b) const;

private:
    // Coin3D scene graph nodes
    SoSeparator* pcRoot;
    SoCoordinate3* pcCoords;
    SoMaterial* pcMaterial;
    SoMaterialBinding* pcMatBinding;
    SoPointSet* pcPointSet;
    SoDrawStyle* pcDrawStyle;
    Gui::SoFCColorBar* pcColorBar;
    
    // Visualization properties
    App::PropertyFloat PointSize;
    App::PropertyBool ShowColorBar;
};

} // namespace InspectionGui

#endif // INSPECTIOGUI_VIEWPROVIDERWALLTHICKNESS_H
