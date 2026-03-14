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

#ifndef INSPECTIONGUI_TASKWALLTHICKNESS_H
#define INSPECTIONGUI_TASKWALLTHICKNESS_H

#include <Gui/TaskView/TaskDialog.h>
#include <Gui/TaskView/TaskView.h>

class QPushButton;
class QLabel;
class QComboBox;
class QDoubleSpinBox;
class QCheckBox;
class QGroupBox;
class QProgressBar;

namespace App {
class DocumentObject;
}

namespace InspectionGui
{

/**
 * @brief Task panel for wall thickness analysis
 */
class TaskWallThickness : public Gui::TaskView::TaskBox
{
    Q_OBJECT

public:
    explicit TaskWallThickness(QWidget* parent = nullptr);
    ~TaskWallThickness() override;

    bool accept() override;
    bool reject() override;

private Q_SLOTS:
    void onSourceChanged(int index);
    void onTargetThicknessChanged(double value);
    void onMinThicknessChanged(double value);
    void onMaxThicknessChanged(double value);
    void onAnalysisModeChanged(int index);
    void onSampleDensityChanged(int index);
    void onAutoRefreshToggled(bool checked);
    void onShowOnlyViolationsToggled(bool checked);
    void onEnableHeatMapToggled(bool checked);
    void onAnalyzeClicked();
    void onRefreshClicked();
    void updateStatistics();

private:
    void setupUI();
    void fillSourceCombo();
    Inspection::FeatureWallThickness* createAnalysisObject();
    
    QComboBox* comboSource;
    QDoubleSpinBox* spinTargetThickness;
    QDoubleSpinBox* spinMinThickness;
    QDoubleSpinBox* spinMaxThickness;
    QComboBox* comboAnalysisMode;
    QComboBox* comboSampleDensity;
    QCheckBox* checkAutoRefresh;
    QCheckBox* checkShowOnlyViolations;
    QCheckBox* checkEnableHeatMap;
    QPushButton* buttonAnalyze;
    QPushButton* buttonRefresh;
    
    // Statistics labels
    QLabel* labelStatsMin;
    QLabel* labelStatsMax;
    QLabel* labelStatsAvg;
    QLabel* labelStatsBelowMin;
    QLabel* labelStatsAboveMax;
    QLabel* labelStatsTotal;
    
    QProgressBar* progressBar;
    
    Inspection::FeatureWallThickness* feature;
};

/**
 * @brief Task dialog for wall thickness analysis
 */
class TaskDlgWallThickness : public Gui::TaskView::TaskDialog
{
    Q_OBJECT

public:
    TaskDlgWallThickness();
    ~TaskDlgWallThickness() override;

    void open() override;
    void clicked(int) override;
    bool accept() override;
    bool reject() override;

    QDialogButtonBox::StandardButtons getStandardButtons() const override
    {
        return QDialogButtonBox::Close;
    }

private:
    TaskWallThickness* widget;
};

} // namespace InspectionGui

#endif // INSPECTIONGUI_TASKWALLTHICKNESS_H
