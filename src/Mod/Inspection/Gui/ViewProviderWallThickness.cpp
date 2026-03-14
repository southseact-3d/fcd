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

#include "PreCompiled.h"
#ifndef _PreComp_
#include <QApplication>
#endif

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoMaterialBinding.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoDrawStyle.h>

#include <Gui/Application.h>
#include <Gui/BitmapFactory.h>
#include <Gui/SoFCColorBar.h>
#include <Mod/Inspection/App/FeatureWallThickness.h>

#include "ViewProviderWallThickness.h"

using namespace InspectionGui;

PROPERTY_SOURCE(InspectionGui::ViewProviderWallThickness, Gui::ViewProviderDocumentObject)

ViewProviderWallThickness::ViewProviderWallThickness()
    : pcRoot(nullptr)
    , pcCoords(nullptr)
    , pcMaterial(nullptr)
    , pcMatBinding(nullptr)
    , pcPointSet(nullptr)
    , pcDrawStyle(nullptr)
    , pcColorBar(nullptr)
{
    // Add properties
    ADD_PROPERTY_TYPE(PointSize, (3.0f), "Display", App::Prop_None, "Size of thickness visualization points");
    ADD_PROPERTY_TYPE(ShowColorBar, (true), "Display", App::Prop_None, "Show color bar legend");
    
    // Create scene graph nodes
    pcRoot = new SoSeparator();
    pcRoot->ref();
    
    pcDrawStyle = new SoDrawStyle();
    pcDrawStyle->ref();
    pcRoot->addChild(pcDrawStyle);
    
    pcCoords = new SoCoordinate3();
    pcCoords->ref();
    pcRoot->addChild(pcCoords);
    
    pcMatBinding = new SoMaterialBinding();
    pcMatBinding->ref();
    pcMatBinding->value = SoMaterialBinding::PER_VERTEX;
    pcRoot->addChild(pcMatBinding);
    
    pcMaterial = new SoMaterial();
    pcMaterial->ref();
    pcRoot->addChild(pcMaterial);
    
    pcPointSet = new SoPointSet();
    pcPointSet->ref();
    pcRoot->addChild(pcPointSet);
    
    pcColorBar = new Gui::SoFCColorBar();
    pcColorBar->ref();
}

ViewProviderWallThickness::~ViewProviderWallThickness()
{
    if (pcRoot) {
        pcRoot->unref();
    }
    if (pcCoords) {
        pcCoords->unref();
    }
    if (pcMaterial) {
        pcMaterial->unref();
    }
    if (pcMatBinding) {
        pcMatBinding->unref();
    }
    if (pcPointSet) {
        pcPointSet->unref();
    }
    if (pcDrawStyle) {
        pcDrawStyle->unref();
    }
    if (pcColorBar) {
        pcColorBar->unref();
    }
}

void ViewProviderWallThickness::attach(App::DocumentObject* pcFeat)
{
    Gui::ViewProviderDocumentObject::attach(pcFeat);
    addDisplayMaskMode(pcRoot, "Points");
}

void ViewProviderWallThickness::setDisplayMode(const char* ModeName)
{
    if (strcmp(ModeName, "Points") == 0) {
        setDisplayMaskMode("Points");
    }
    Gui::ViewProviderDocumentObject::setDisplayMode(ModeName);
}

std::vector<std::string> ViewProviderWallThickness::getDisplayModes() const
{
    std::vector<std::string> StrList;
    StrList.emplace_back("Points");
    return StrList;
}

void ViewProviderWallThickness::updateData(const App::Property* prop)
{
    Inspection::FeatureWallThickness* feature = static_cast<Inspection::FeatureWallThickness*>(pcObject);
    
    if (prop == &feature->Source || 
        prop == &feature->AnalysisUpToDate) {
        // Rebuild visualization when analysis is complete
        if (feature->AnalysisUpToDate.getValue()) {
            setupVisualization();
        }
    }
    
    Gui::ViewProviderDocumentObject::updateData(prop);
}

void ViewProviderWallThickness::onChanged(const App::Property* prop)
{
    if (prop == &PointSize) {
        pcDrawStyle->pointSize = PointSize.getValue();
    }
    else if (prop == &ShowColorBar) {
        if (pcColorBar) {
            pcColorBar->setVisible(ShowColorBar.getValue());
        }
    }
    
    Gui::ViewProviderDocumentObject::onChanged(prop);
}

QIcon ViewProviderWallThickness::getIcon() const
{
    return Gui::BitmapFactory().pixmap("Inspection_ThicknessAnalysis");
}

SoSeparator* ViewProviderWallThickness::getFrontRoot() const
{
    return pcColorBar;
}

void ViewProviderWallThickness::show()
{
    pcRoot->whichChild = SO_SWITCH_ALL;
    Gui::ViewProviderDocumentObject::show();
}

void ViewProviderWallThickness::hide()
{
    pcRoot->whichChild = SO_SWITCH_NONE;
    Gui::ViewProviderDocumentObject::hide();
}

void ViewProviderWallThickness::setupVisualization()
{
    Inspection::FeatureWallThickness* feature = static_cast<Inspection::FeatureWallThickness*>(pcObject);
    if (!feature) {
        return;
    }
    
    const std::vector<Inspection::ThicknessPoint>& results = feature->getResults();
    if (results.empty()) {
        clearVisualization();
        return;
    }
    
    // Check if we should only show violations
    bool onlyViolations = feature->ShowOnlyViolations.getValue();
    double minThick = feature->MinThickness.getValue();
    double maxThick = feature->MaxThickness.getValue();
    
    // Count points to display
    int numPoints = 0;
    for (const auto& result : results) {
        if (result.isValid) {
            if (!onlyViolations || 
                result.thickness < minThick || 
                result.thickness > maxThick) {
                numPoints++;
            }
        }
    }
    
    if (numPoints == 0) {
        clearVisualization();
        return;
    }
    
    // Set up coordinates and colors
    pcCoords->point.setNum(numPoints);
    pcMaterial->diffuseColor.setNum(numPoints);
    
    SbVec3f* coords = pcCoords->point.startEditing();
    SbColor* colors = pcMaterial->diffuseColor.startEditing();
    
    int idx = 0;
    for (const auto& result : results) {
        if (!result.isValid) {
            continue;
        }
        
        if (onlyViolations && 
            result.thickness >= minThick && 
            result.thickness <= maxThick) {
            continue;
        }
        
        // Set coordinate
        coords[idx].setValue(
            static_cast<float>(result.position.x),
            static_cast<float>(result.position.y),
            static_cast<float>(result.position.z)
        );
        
        // Set color based on thickness (heat map)
        float r, g, b;
        thicknessToColor(result.thickness, r, g, b);
        colors[idx].setValue(r, g, b);
        
        idx++;
    }
    
    pcCoords->point.finishEditing();
    pcMaterial->diffuseColor.finishEditing();
    
    // Update point set
    pcPointSet->numPoints = numPoints;
    
    // Update color bar
    if (pcColorBar && feature->EnableHeatMap.getValue()) {
        // Set color bar range based on min/max thickness
        double minT = std::numeric_limits<double>::max();
        double maxT = 0.0;
        for (const auto& result : results) {
            if (result.isValid) {
                if (result.thickness < minT) minT = result.thickness;
                if (result.thickness > maxT) maxT = result.thickness;
            }
        }
        pcColorBar->setRange(minT, maxT);
    }
}

void ViewProviderWallThickness::updateHeatMapColors()
{
    setupVisualization();
}

void ViewProviderWallThickness::clearVisualization()
{
    pcCoords->point.setNum(0);
    pcMaterial->diffuseColor.setNum(0);
    pcPointSet->numPoints = 0;
}

void ViewProviderWallThickness::thicknessToColor(double thickness, float& r, float& g, float& b) const
{
    Inspection::FeatureWallThickness* feature = static_cast<Inspection::FeatureWallThickness*>(pcObject);
    if (!feature) {
        r = g = b = 0.5f;
        return;
    }
    
    double target = feature->TargetThickness.getValue();
    double minThick = feature->MinThickness.getValue();
    double maxThick = feature->MaxThickness.getValue();
    
    // Heat map coloring:
    // - Below minimum: Red (critical - too thin)
    // - At target: Green (ideal)
    // - Above maximum: Blue (too thick)
    // - Gradient in between
    
    if (thickness < minThick) {
        // Too thin - red, intensity based on how thin
        double ratio = thickness / minThick;
        r = 1.0f;
        g = static_cast<float>(ratio);
        b = static_cast<float>(ratio);
    }
    else if (thickness > maxThick) {
        // Too thick - blue
        double ratio = maxThick / thickness;
        r = static_cast<float>(ratio);
        g = static_cast<float>(ratio);
        b = 1.0f;
    }
    else if (thickness < target) {
        // Between min and target - yellow to green
        double ratio = (thickness - minThick) / (target - minThick);
        r = static_cast<float>(1.0 - ratio * 0.5);
        g = 1.0f;
        b = 0.0f;
    }
    else {
        // Between target and max - green to cyan
        double ratio = (thickness - target) / (maxThick - target);
        r = 0.5f;
        g = static_cast<float>(1.0 - ratio * 0.5);
        b = static_cast<float>(ratio * 0.5);
    }
}
