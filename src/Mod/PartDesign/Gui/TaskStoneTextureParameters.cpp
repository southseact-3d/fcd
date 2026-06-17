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


#include <QMessageBox>

#include <Base/Interpreter.h>
#include <Base/Converter.h>
#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Selection/Selection.h>
#include <Gui/ViewProvider.h>
#include <Mod/PartDesign/App/FeatureStoneTexture.h>

#include "ui_TaskStoneTextureParameters.h"
#include "TaskStoneTextureParameters.h"


using namespace PartDesignGui;
using namespace Gui;

TaskStoneTextureParameters::TaskStoneTextureParameters(ViewProviderDressUp* DressUpView, QWidget* parent)
    : TaskDressUpParameters(DressUpView, false, true, parent)
    , ui(new Ui_TaskStoneTextureParameters)
{
    allowPlanar = true;
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    this->groupLayout()->addWidget(proxy);

    PartDesign::StoneTexture* pcStone = DressUpView->getObject<PartDesign::StoneTexture>();

    ui->stonePattern->clear();
    ui->stonePattern->addItem(QStringLiteral("Ashlar"));
    ui->stonePattern->addItem(QStringLiteral("Rubble"));
    ui->stonePattern->addItem(QStringLiteral("Fieldstone"));
    ui->stonePattern->setCurrentIndex(pcStone->StonePattern.getValue());

    ui->stoneScale->clear();
    ui->stoneScale->addItem(QStringLiteral("Small"));
    ui->stoneScale->addItem(QStringLiteral("Medium"));
    ui->stoneScale->addItem(QStringLiteral("Large"));
    ui->stoneScale->setCurrentIndex(pcStone->StoneScale.getValue());

    ui->stoneSize->setUnit(Base::Unit::Length);
    ui->stoneSize->setValue(pcStone->StoneSize.getValue());
    ui->stoneSize->setMinimum(0.1);
    ui->stoneSize->bind(pcStone->StoneSize);

    ui->stoneDepth->setUnit(Base::Unit::Length);
    ui->stoneDepth->setValue(pcStone->StoneDepth.getValue());
    ui->stoneDepth->setMinimum(0.0);
    ui->stoneDepth->bind(pcStone->StoneDepth);

    ui->depthVariation->setValue(pcStone->DepthVariation.getValue());
    ui->depthVariation->bind(pcStone->DepthVariation);

    ui->roughness->setValue(pcStone->Roughness.getValue());
    ui->roughness->bind(pcStone->Roughness);

    ui->mortarThickness->setUnit(Base::Unit::Length);
    ui->mortarThickness->setValue(pcStone->MortarThickness.getValue());
    ui->mortarThickness->setMinimum(0.0);
    ui->mortarThickness->bind(pcStone->MortarThickness);

    ui->mortarDepth->setUnit(Base::Unit::Length);
    ui->mortarDepth->setValue(pcStone->MortarDepth.getValue());
    ui->mortarDepth->setMinimum(0.0);
    ui->mortarDepth->bind(pcStone->MortarDepth);

    ui->seed->setValue(pcStone->Seed.getValue());
    ui->seed->bind(pcStone->Seed);

    ui->scale->clear();
    ui->scale->addItem(QStringLiteral("1:1"));
    ui->scale->addItem(QStringLiteral("1:2"));
    ui->scale->addItem(QStringLiteral("1:5"));
    ui->scale->addItem(QStringLiteral("1:10"));
    ui->scale->addItem(QStringLiteral("1:20"));
    ui->scale->addItem(QStringLiteral("1:50"));
    ui->scale->addItem(QStringLiteral("1:76"));
    ui->scale->addItem(QStringLiteral("1:100"));
    ui->scale->addItem(QStringLiteral("1:200"));
    ui->scale->addItem(QStringLiteral("1:500"));
    ui->scale->addItem(QStringLiteral("Custom"));
    ui->scale->setCurrentIndex(pcStone->Scale.getValue());

    std::vector<std::string> strings = pcStone->Base.getSubValues();
    for (const auto& string : strings) {
        ui->listWidgetReferences->addItem(QString::fromStdString(string));
    }

    QMetaObject::connectSlotsByName(this);

    // clang-format off
    connect(ui->stonePattern, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &TaskStoneTextureParameters::onStonePatternChanged);
    connect(ui->stoneScale, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &TaskStoneTextureParameters::onStoneScaleChanged);
    connect(ui->stoneSize, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskStoneTextureParameters::onStoneSizeChanged);
    connect(ui->stoneDepth, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskStoneTextureParameters::onStoneDepthChanged);
    connect(ui->depthVariation, qOverload<int>(&Gui::IntSpinBox::valueChanged),
        this, &TaskStoneTextureParameters::onDepthVariationChanged);
    connect(ui->roughness, qOverload<int>(&Gui::IntSpinBox::valueChanged),
        this, &TaskStoneTextureParameters::onRoughnessChanged);
    connect(ui->mortarThickness, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskStoneTextureParameters::onMortarThicknessChanged);
    connect(ui->mortarDepth, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskStoneTextureParameters::onMortarDepthChanged);
    connect(ui->seed, qOverload<int>(&Gui::IntSpinBox::valueChanged),
        this, &TaskStoneTextureParameters::onSeedChanged);
    connect(ui->scale, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &TaskStoneTextureParameters::onScaleChanged);

    connect(ui->buttonRefSel, &QToolButton::toggled,
        this, &TaskStoneTextureParameters::onButtonRefSel);

    createDeleteAction(ui->listWidgetReferences);
    connect(deleteAction, &QAction::triggered, this, &TaskStoneTextureParameters::onRefDeleted);

    connect(ui->listWidgetReferences, &QListWidget::currentItemChanged,
        this, &TaskStoneTextureParameters::setSelection);
    connect(ui->listWidgetReferences, &QListWidget::itemClicked,
        this, &TaskStoneTextureParameters::setSelection);
    connect(ui->listWidgetReferences, &QListWidget::itemDoubleClicked,
        this, &TaskStoneTextureParameters::doubleClicked);
    // clang-format on

    if (strings.empty()) {
        setSelectionMode(refSel);
    }
    else {
        hideOnError();
    }
}

TaskStoneTextureParameters::~TaskStoneTextureParameters()
{
    try {
        Gui::Selection().clearSelection();
        Gui::Selection().rmvSelectionGate();
    }
    catch (const Py::Exception&) {
        Base::PyException e;
        e.reportException();
    }
}

void TaskStoneTextureParameters::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type == Gui::SelectionChanges::AddSelection) {
        if (selectionMode == refSel) {
            referenceSelected(msg, ui->listWidgetReferences);
        }
    }
}

void TaskStoneTextureParameters::setButtons(const selectionModes mode)
{
    ui->buttonRefSel->setChecked(mode == refSel);
    ui->buttonRefSel->setText(mode == refSel ? stopSelectionLabel() : startSelectionLabel());
}

void TaskStoneTextureParameters::onRefDeleted()
{
    TaskDressUpParameters::deleteRef(ui->listWidgetReferences);
}

void TaskStoneTextureParameters::onStonePatternChanged(int val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->StonePattern.setValue(val);
        // Update size and roughness from preset
        if (val == 0) {
            stone->StoneSize.setValue(200.0);
            stone->Roughness.setValue(10);
            ui->stoneSize->setValue(200.0);
            ui->roughness->setValue(10);
        }
        else if (val == 1) {
            stone->StoneSize.setValue(150.0);
            stone->Roughness.setValue(50);
            ui->stoneSize->setValue(150.0);
            ui->roughness->setValue(50);
        }
        else {
            stone->StoneSize.setValue(100.0);
            stone->Roughness.setValue(80);
            ui->stoneSize->setValue(100.0);
            ui->roughness->setValue(80);
        }
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::onStoneScaleChanged(int val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->StoneScale.setValue(val);
        // Update size from preset
        if (val == 0) {
            stone->StoneSize.setValue(50.0);
            ui->stoneSize->setValue(50.0);
        }
        else if (val == 1) {
            stone->StoneSize.setValue(150.0);
            ui->stoneSize->setValue(150.0);
        }
        else {
            stone->StoneSize.setValue(300.0);
            ui->stoneSize->setValue(300.0);
        }
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::onStoneSizeChanged(double val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->StoneSize.setValue(val);
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::onStoneDepthChanged(double val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->StoneDepth.setValue(val);
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::onDepthVariationChanged(int val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->DepthVariation.setValue(val);
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::onRoughnessChanged(int val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->Roughness.setValue(val);
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::onMortarThicknessChanged(double val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->MortarThickness.setValue(val);
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::onMortarDepthChanged(double val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->MortarDepth.setValue(val);
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::onSeedChanged(int val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->Seed.setValue(val);
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::onScaleChanged(int val)
{
    if (auto stone = getObject<PartDesign::StoneTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        stone->Scale.setValue(val);
        stone->recomputeFeature();
        hideOnError();
    }
}

void TaskStoneTextureParameters::changeEvent(QEvent* e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(proxy);
    }
}

void TaskStoneTextureParameters::apply()
{
    ui->stoneSize->apply();
    ui->stoneDepth->apply();
    ui->depthVariation->apply();
    ui->roughness->apply();
    ui->mortarThickness->apply();
    ui->mortarDepth->apply();
    ui->seed->apply();
}

//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgStoneTextureParameters::TaskDlgStoneTextureParameters(ViewProviderStoneTexture* DressUpView)
    : TaskDlgDressUpParameters(DressUpView)
{
    parameter = new TaskStoneTextureParameters(DressUpView);

    Content.push_back(parameter);
    Content.push_back(preview);
}

TaskDlgStoneTextureParameters::~TaskDlgStoneTextureParameters() = default;

bool TaskDlgStoneTextureParameters::accept()
{
    auto obj = getObject();
    if (!obj->isError()) {
        getViewObject()->showPreviousFeature(false);
    }

    parameter->apply();

    return TaskDlgDressUpParameters::accept();
}

#include "moc_TaskStoneTextureParameters.cpp"
