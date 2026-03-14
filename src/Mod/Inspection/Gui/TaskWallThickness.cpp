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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QGroupBox>
#include <QProgressBar>
#include <QMessageBox>
#include <QTimer>
#endif

#include "TaskWallThickness.h"
#include <Gui/Application.h>
#include <Gui/Document.h>
#include <Gui/Control.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Mod/Part/App/PartFeature.h>
#include <Mod/Inspection/App/FeatureWallThickness.h>

using namespace InspectionGui;

TaskWallThickness::TaskWallThickness(QWidget* parent)
    : Gui::TaskView::TaskBox(QPixmap(), tr("Wall Thickness Analysis"), true, parent)
    , comboSource(nullptr)
    , spinTargetThickness(nullptr)
    , spinMinThickness(nullptr)
    , spinMaxThickness(nullptr)
    , comboAnalysisMode(nullptr)
    , comboSampleDensity(nullptr)
    , checkAutoRefresh(nullptr)
    , checkShowOnlyViolations(nullptr)
    , checkEnableHeatMap(nullptr)
    , buttonAnalyze(nullptr)
    , buttonRefresh(nullptr)
    , labelStatsMin(nullptr)
    , labelStatsMax(nullptr)
    , labelStatsAvg(nullptr)
    , labelStatsBelowMin(nullptr)
    , labelStatsAboveMax(nullptr)
    , labelStatsTotal(nullptr)
    , progressBar(nullptr)
    , feature(nullptr)
{
    setupUI();
    fillSourceCombo();
}

TaskWallThickness::~TaskWallThickness() = default;

void TaskWallThickness::setupUI()
{
    auto widget = new QWidget(this);
    auto layout = new QVBoxLayout(widget);
    layout->setSpacing(6);
    layout->setContentsMargins(6, 6, 6, 6);
    
    // Source selection
    auto sourceGroup = new QGroupBox(tr("Source"), widget);
    auto sourceLayout = new QGridLayout(sourceGroup);
    
    sourceLayout->addWidget(new QLabel(tr("Shape to analyze:")), 0, 0);
    comboSource = new QComboBox(sourceGroup);
    sourceLayout->addWidget(comboSource, 0, 1);
    
    layout->addWidget(sourceGroup);
    
    // Thickness settings
    auto thicknessGroup = new QGroupBox(tr("Thickness Settings"), widget);
    auto thicknessLayout = new QGridLayout(thicknessGroup);
    
    thicknessLayout->addWidget(new QLabel(tr("Target thickness:")), 0, 0);
    spinTargetThickness = new QDoubleSpinBox(thicknessGroup);
    spinTargetThickness->setRange(0.1, 1000.0);
    spinTargetThickness->setValue(1.0);
    spinTargetThickness->setSuffix(tr(" mm"));
    spinTargetThickness->setDecimals(2);
    thicknessLayout->addWidget(spinTargetThickness, 0, 1);
    
    thicknessLayout->addWidget(new QLabel(tr("Minimum thickness:")), 1, 0);
    spinMinThickness = new QDoubleSpinBox(thicknessGroup);
    spinMinThickness->setRange(0.1, 1000.0);
    spinMinThickness->setValue(0.8);
    spinMinThickness->setSuffix(tr(" mm"));
    spinMinThickness->setDecimals(2);
    thicknessLayout->addWidget(spinMinThickness, 1, 1);
    
    thicknessLayout->addWidget(new QLabel(tr("Maximum thickness:")), 2, 0);
    spinMaxThickness = new QDoubleSpinBox(thicknessGroup);
    spinMaxThickness->setRange(0.1, 1000.0);
    spinMaxThickness->setValue(5.0);
    spinMaxThickness->setSuffix(tr(" mm"));
    spinMaxThickness->setDecimals(2);
    thicknessLayout->addWidget(spinMaxThickness, 2, 1);
    
    layout->addWidget(thicknessGroup);
    
    // Analysis settings
    auto analysisGroup = new QGroupBox(tr("Analysis Settings"), widget);
    auto analysisLayout = new QGridLayout(analysisGroup);
    
    analysisLayout->addWidget(new QLabel(tr("Analysis mode:")), 0, 0);
    comboAnalysisMode = new QComboBox(analysisGroup);
    comboAnalysisMode->addItem(tr("Fast"));
    comboAnalysisMode->addItem(tr("Slow (more accurate)"));
    analysisLayout->addWidget(comboAnalysisMode, 0, 1);
    
    analysisLayout->addWidget(new QLabel(tr("Sample density:")), 1, 0);
    comboSampleDensity = new QComboBox(analysisGroup);
    comboSampleDensity->addItem(tr("Low"));
    comboSampleDensity->addItem(tr("Medium"));
    comboSampleDensity->addItem(tr("High"));
    comboSampleDensity->setCurrentIndex(1);
    analysisLayout->addWidget(comboSampleDensity, 1, 1);
    
    layout->addWidget(analysisGroup);
    
    // Display options
    auto displayGroup = new QGroupBox(tr("Display Options"), widget);
    auto displayLayout = new QVBoxLayout(displayGroup);
    
    checkAutoRefresh = new QCheckBox(tr("Auto-refresh when shape changes"), displayGroup);
    checkAutoRefresh->setChecked(true);
    displayLayout->addWidget(checkAutoRefresh);
    
    checkShowOnlyViolations = new QCheckBox(tr("Show only violations"), displayGroup);
    displayLayout->addWidget(checkShowOnlyViolations);
    
    checkEnableHeatMap = new QCheckBox(tr("Enable heat map"), displayGroup);
    checkEnableHeatMap->setChecked(true);
    displayLayout->addWidget(checkEnableHeatMap);
    
    layout->addWidget(displayGroup);
    
    // Action buttons
    auto buttonLayout = new QHBoxLayout();
    buttonAnalyze = new QPushButton(tr("Analyze"), widget);
    buttonLayout->addWidget(buttonAnalyze);
    buttonRefresh = new QPushButton(tr("Refresh"), widget);
    buttonLayout->addWidget(buttonRefresh);
    layout->addLayout(buttonLayout);
    
    // Progress bar
    progressBar = new QProgressBar(widget);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setVisible(false);
    layout->addWidget(progressBar);
    
    // Statistics
    auto statsGroup = new QGroupBox(tr("Statistics"), widget);
    auto statsLayout = new QGridLayout(statsGroup);
    
    labelStatsMin = new QLabel(tr("Min: -"), statsGroup);
    statsLayout->addWidget(labelStatsMin, 0, 0);
    
    labelStatsMax = new QLabel(tr("Max: -"), statsGroup);
    statsLayout->addWidget(labelStatsMax, 0, 1);
    
    labelStatsAvg = new QLabel(tr("Avg: -"), statsGroup);
    statsLayout->addWidget(labelStatsAvg, 1, 0);
    
    labelStatsTotal = new QLabel(tr("Total points: -"), statsGroup);
    statsLayout->addWidget(labelStatsTotal, 1, 1);
    
    labelStatsBelowMin = new QLabel(tr("Below min: -"), statsGroup);
    statsLayout->addWidget(labelStatsBelowMin, 2, 0);
    
    labelStatsAboveMax = new QLabel(tr("Above max: -"), statsGroup);
    statsLayout->addWidget(labelStatsAboveMax, 2, 1);
    
    layout->addWidget(statsGroup);
    
    layout->addStretch();
    
    this->groupLayout()->addWidget(widget);
    
    // Connect signals
    connect(comboSource, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TaskWallThickness::onSourceChanged);
    connect(spinTargetThickness, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TaskWallThickness::onTargetThicknessChanged);
    connect(spinMinThickness, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TaskWallThickness::onMinThicknessChanged);
    connect(spinMaxThickness, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &TaskWallThickness::onMaxThicknessChanged);
    connect(comboAnalysisMode, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TaskWallThickness::onAnalysisModeChanged);
    connect(comboSampleDensity, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TaskWallThickness::onSampleDensityChanged);
    connect(checkAutoRefresh, &QCheckBox::toggled,
            this, &TaskWallThickness::onAutoRefreshToggled);
    connect(checkShowOnlyViolations, &QCheckBox::toggled,
            this, &TaskWallThickness::onShowOnlyViolationsToggled);
    connect(checkEnableHeatMap, &QCheckBox::toggled,
            this, &TaskWallThickness::onEnableHeatMapToggled);
    connect(buttonAnalyze, &QPushButton::clicked,
            this, &TaskWallThickness::onAnalyzeClicked);
    connect(buttonRefresh, &QPushButton::clicked,
            this, &TaskWallThickness::onRefreshClicked);
}

void TaskWallThickness::fillSourceCombo()
{
    comboSource->clear();
    comboSource->addItem(tr("<Select shape>"), QVariant());
    
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }
    
    std::vector<App::DocumentObject*> objects = doc->getObjectsOfType<Part::Feature>();
    for (auto obj : objects) {
        comboSource->addItem(QString::fromUtf8(obj->Label.getValue()), 
                            QString::fromUtf8(obj->getNameInDocument()));
    }
}

void TaskWallThickness::onSourceChanged(int index)
{
    if (index <= 0 || !feature) {
        return;
    }
    
    QString objName = comboSource->itemData(index).toString();
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return;
    }
    
    App::DocumentObject* obj = doc->getObject(objName.toUtf8().constData());
    if (obj) {
        feature->Source.setValue(obj);
    }
}

void TaskWallThickness::onTargetThicknessChanged(double value)
{
    if (feature) {
        feature->TargetThickness.setValue(value);
    }
}

void TaskWallThickness::onMinThicknessChanged(double value)
{
    if (feature) {
        feature->MinThickness.setValue(value);
    }
}

void TaskWallThickness::onMaxThicknessChanged(double value)
{
    if (feature) {
        feature->MaxThickness.setValue(value);
    }
}

void TaskWallThickness::onAnalysisModeChanged(int index)
{
    if (feature) {
        feature->AnalysisMode.setValue(index);
    }
}

void TaskWallThickness::onSampleDensityChanged(int index)
{
    if (feature) {
        feature->SampleDensity.setValue(index);
    }
}

void TaskWallThickness::onAutoRefreshToggled(bool checked)
{
    if (feature) {
        feature->AutoRefresh.setValue(checked);
    }
}

void TaskWallThickness::onShowOnlyViolationsToggled(bool checked)
{
    if (feature) {
        feature->ShowOnlyViolations.setValue(checked);
    }
}

void TaskWallThickness::onEnableHeatMapToggled(bool checked)
{
    if (feature) {
        feature->EnableHeatMap.setValue(checked);
    }
}

void TaskWallThickness::onAnalyzeClicked()
{
    if (!feature) {
        feature = createAnalysisObject();
        if (!feature) {
            QMessageBox::warning(this, tr("Error"), 
                               tr("Failed to create analysis object"));
            return;
        }
    }
    
    // Apply current settings
    onSourceChanged(comboSource->currentIndex());
    onTargetThicknessChanged(spinTargetThickness->value());
    onMinThicknessChanged(spinMinThickness->value());
    onMaxThicknessChanged(spinMaxThickness->value());
    onAnalysisModeChanged(comboAnalysisMode->currentIndex());
    onSampleDensityChanged(comboSampleDensity->currentIndex());
    onAutoRefreshToggled(checkAutoRefresh->isChecked());
    onShowOnlyViolationsToggled(checkShowOnlyViolations->isChecked());
    onEnableHeatMapToggled(checkEnableHeatMap->isChecked());
    
    // Trigger analysis
    feature->recomputeAnalysis();
    
    // Update statistics after a brief delay to allow computation
    QTimer::singleShot(100, this, &TaskWallThickness::updateStatistics);
}

void TaskWallThickness::onRefreshClicked()
{
    if (feature) {
        feature->recomputeAnalysis();
        QTimer::singleShot(100, this, &TaskWallThickness::updateStatistics);
    }
}

void TaskWallThickness::updateStatistics()
{
    if (!feature) {
        return;
    }
    
    double minThick, maxThick, avgThick;
    int belowMin, aboveMax, total;
    feature->getStatistics(minThick, maxThick, avgThick, belowMin, aboveMax, total);
    
    labelStatsMin->setText(tr("Min: %1 mm").arg(minThick, 0, 'f', 2));
    labelStatsMax->setText(tr("Max: %1 mm").arg(maxThick, 0, 'f', 2));
    labelStatsAvg->setText(tr("Avg: %1 mm").arg(avgThick, 0, 'f', 2));
    labelStatsBelowMin->setText(tr("Below min: %1").arg(belowMin));
    labelStatsAboveMax->setText(tr("Above max: %1").arg(aboveMax));
    labelStatsTotal->setText(tr("Total points: %1").arg(total));
}

Inspection::FeatureWallThickness* TaskWallThickness::createAnalysisObject()
{
    App::Document* doc = App::GetApplication().getActiveDocument();
    if (!doc) {
        return nullptr;
    }
    
    // Create new wall thickness analysis object
    App::DocumentObject* obj = doc->addObject("Inspection::FeatureWallThickness", 
                                               "WallThicknessAnalysis");
    return static_cast<Inspection::FeatureWallThickness*>(obj);
}

bool TaskWallThickness::accept()
{
    return true;
}

bool TaskWallThickness::reject()
{
    return true;
}

// TaskDlgWallThickness implementation

TaskDlgWallThickness::TaskDlgWallThickness()
{
    widget = new TaskWallThickness();
    Content.push_back(widget);
}

TaskDlgWallThickness::~TaskDlgWallThickness() = default;

void TaskDlgWallThickness::open()
{
}

void TaskDlgWallThickness::clicked(int)
{
}

bool TaskDlgWallThickness::accept()
{
    return widget->accept();
}

bool TaskDlgWallThickness::reject()
{
    return widget->reject();
}
