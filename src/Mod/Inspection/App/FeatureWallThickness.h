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

#ifndef INSPECTION_FEATUREWALLTHICKNESS_H
#define INSPECTION_FEATUREWALLTHICKNESS_H

#include <App/DocumentObject.h>
#include <App/PropertyStandard.h>
#include <App/PropertyUnits.h>
#include <App/PropertyLinks.h>
#include <Mod/Inspection/InspectionGlobal.h>

#include <vector>

namespace Part {
class TopoShape;
}

namespace Inspection
{

/**
 * @brief Data structure to store thickness analysis results
 */
struct ThicknessPoint {
    Base::Vector3d position;      // Point on surface
    Base::Vector3d normal;        // Surface normal at point
    double thickness;             // Computed thickness value
    bool isValid;                 // Whether thickness could be computed
};

/**
 * @brief Document object for wall thickness analysis
 * 
 * Analyzes a 3D model to determine wall thickness at sample points
 * and provides heat map visualization for 3D printing analysis.
 */
class InspectionExport FeatureWallThickness : public App::DocumentObject
{
    PROPERTY_HEADER_WITH_OVERRIDE(Inspection::FeatureWallThickness);

public:
    /// Constructor
    FeatureWallThickness();
    ~FeatureWallThickness() override;

    /** @name Properties */
    //@{
    /// Source shape to analyze
    App::PropertyLink Source;
    
    /// Target wall thickness for 3D printing (e.g., 0.8mm, 1.2mm)
    App::PropertyLength TargetThickness;
    
    /// Minimum acceptable thickness (below this is critical)
    App::PropertyLength MinThickness;
    
    /// Maximum acceptable thickness (above this may cause issues)
    App::PropertyLength MaxThickness;
    
    /// Analysis mode: 0=Fast (surface sampling), 1=Slow (detailed ray casting)
    App::PropertyEnumeration AnalysisMode;
    
    /// Sample density: Low, Medium, High
    App::PropertyEnumeration SampleDensity;
    
    /// Auto-refresh when source shape changes
    App::PropertyBool AutoRefresh;
    
    /// Show only regions with thickness violations
    App::PropertyBool ShowOnlyViolations;
    
    /// Enable heat map visualization
    App::PropertyBool EnableHeatMap;
    
    /// Analysis is up-to-date
    App::PropertyBool AnalysisUpToDate;
    //@}

    /** @name Actions */
    //@{
    short mustExecute() const override;
    App::DocumentObjectExecReturn* execute() override;
    void onChanged(const App::Property* prop) override;
    //@}

    /// Returns the type name of the ViewProvider
    const char* getViewProviderName() const override {
        return "InspectionGui::ViewProviderWallThickness";
    }

    /// Get the analysis results
    const std::vector<ThicknessPoint>& getResults() const { return _results; }
    
    /// Get statistics
    void getStatistics(double& minThick, double& maxThick, double& avgThick, 
                       int& belowMin, int& aboveMax, int& total) const;
    
    /// Force re-analysis
    void recomputeAnalysis();

private:
    std::vector<ThicknessPoint> _results;
    
    // Analysis algorithms
    void analyzeFast(const Part::TopoShape& shape);
    void analyzeSlow(const Part::TopoShape& shape);
    
    // Sample point generation
    void generateSamplePoints(const Part::TopoShape& shape, std::vector<Base::Vector3d>& points, 
                              std::vector<Base::Vector3d>& normals);
    
    // Compute thickness at a point
    double computeThicknessAtPoint(const Part::TopoShape& shape, const Base::Vector3d& point, 
                                   const Base::Vector3d& normal);
    
    // Get sample density value
    double getSampleSpacing() const;
};

} // namespace Inspection

#endif // INSPECTION_FEATUREWALLTHICKNESS_H
