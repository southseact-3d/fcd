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
#include <Mod/PartDesign/App/FeatureWoodPlankTexture.h>

#include "ui_TaskWoodPlankTextureParameters.h"
#include "TaskWoodPlankTextureParameters.h"


using namespace PartDesignGui;
using namespace Gui;

TaskWoodPlankTextureParameters::TaskWoodPlankTextureParameters(ViewProviderDressUp* DressUpView, QWidget* parent)
    : TaskDressUpParameters(DressUpView, false, true, parent)
    , ui(new Ui_TaskWoodPlankTextureParameters)
{
    allowPlanar = true;
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    this->groupLayout()->addWidget(proxy);

    PartDesign::WoodPlankTexture* pcPlank = DressUpView->getObject<PartDesign::WoodPlankTexture>();

    ui->plankLength->setUnit(Base::Unit::Length);
    ui->plankLength->setValue(pcPlank->PlankLength.getValue());
    ui->plankLength->setMinimum(0.1);
    ui->plankLength->bind(pcPlank->PlankLength);

    ui->plankHeight->setUnit(Base::Unit::Length);
    ui->plankHeight->setValue(pcPlank->PlankHeight.getValue());
    ui->plankHeight->setMinimum(0.1);
    ui->plankHeight->bind(pcPlank->PlankHeight);

    ui->plankDepth->setUnit(Base::Unit::Length);
    ui->plankDepth->setValue(pcPlank->PlankDepth.getValue());
    ui->plankDepth->setMinimum(0.0);
    ui->plankDepth->bind(pcPlank->PlankDepth);

    ui->gapWidth->setUnit(Base::Unit::Length);
    ui->gapWidth->setValue(pcPlank->GapWidth.getValue());
    ui->gapWidth->setMinimum(0.0);
    ui->gapWidth->bind(pcPlank->GapWidth);

    ui->gapDepth->setUnit(Base::Unit::Length);
    ui->gapDepth->setValue(pcPlank->GapDepth.getValue());
    ui->gapDepth->setMinimum(0.0);
    ui->gapDepth->bind(pcPlank->GapDepth);

    ui->orientation->clear();
    ui->orientation->addItem(QStringLiteral("Horizontal"));
    ui->orientation->addItem(QStringLiteral("Vertical"));
    ui->orientation->addItem(QStringLiteral("Diagonal"));
    ui->orientation->setCurrentIndex(pcPlank->Orientation.getValue());

    ui->endOffset->setValue(pcPlank->EndOffset.getValue());
    ui->endOffset->bind(pcPlank->EndOffset);

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
    ui->scale->setCurrentIndex(pcPlank->Scale.getValue());

    std::vector<std::string> strings = pcPlank->Base.getSubValues();
    for (const auto& string : strings) {
        ui->listWidgetReferences->addItem(QString::fromStdString(string));
    }

    QMetaObject::connectSlotsByName(this);

    // clang-format off
    connect(ui->plankLength, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskWoodPlankTextureParameters::onPlankLengthChanged);
    connect(ui->plankHeight, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskWoodPlankTextureParameters::onPlankHeightChanged);
    connect(ui->plankDepth, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskWoodPlankTextureParameters::onPlankDepthChanged);
    connect(ui->gapWidth, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskWoodPlankTextureParameters::onGapWidthChanged);
    connect(ui->gapDepth, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskWoodPlankTextureParameters::onGapDepthChanged);
    connect(ui->orientation, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &TaskWoodPlankTextureParameters::onOrientationChanged);
    connect(ui->endOffset, qOverload<int>(&Gui::IntSpinBox::valueChanged),
        this, &TaskWoodPlankTextureParameters::onEndOffsetChanged);
    connect(ui->scale, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &TaskWoodPlankTextureParameters::onScaleChanged);

    connect(ui->buttonRefSel, &QToolButton::toggled,
        this, &TaskWoodPlankTextureParameters::onButtonRefSel);

    createDeleteAction(ui->listWidgetReferences);
    connect(deleteAction, &QAction::triggered, this, &TaskWoodPlankTextureParameters::onRefDeleted);

    connect(ui->listWidgetReferences, &QListWidget::currentItemChanged,
        this, &TaskWoodPlankTextureParameters::setSelection);
    connect(ui->listWidgetReferences, &QListWidget::itemClicked,
        this, &TaskWoodPlankTextureParameters::setSelection);
    connect(ui->listWidgetReferences, &QListWidget::itemDoubleClicked,
        this, &TaskWoodPlankTextureParameters::doubleClicked);
    // clang-format on

    if (strings.empty()) {
        setSelectionMode(refSel);
    }
    else {
        hideOnError();
    }
}

TaskWoodPlankTextureParameters::~TaskWoodPlankTextureParameters()
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

void TaskWoodPlankTextureParameters::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type == Gui::SelectionChanges::AddSelection) {
        if (selectionMode == refSel) {
            referenceSelected(msg, ui->listWidgetReferences);
        }
    }
}

void TaskWoodPlankTextureParameters::setButtons(const selectionModes mode)
{
    ui->buttonRefSel->setChecked(mode == refSel);
    ui->buttonRefSel->setText(mode == refSel ? stopSelectionLabel() : startSelectionLabel());
}

void TaskWoodPlankTextureParameters::onRefDeleted()
{
    TaskDressUpParameters::deleteRef(ui->listWidgetReferences);
}

void TaskWoodPlankTextureParameters::onPlankLengthChanged(double val)
{
    if (auto plank = getObject<PartDesign::WoodPlankTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        plank->PlankLength.setValue(val);
        plank->recomputeFeature();
        hideOnError();
    }
}

void TaskWoodPlankTextureParameters::onPlankHeightChanged(double val)
{
    if (auto plank = getObject<PartDesign::WoodPlankTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        plank->PlankHeight.setValue(val);
        plank->recomputeFeature();
        hideOnError();
    }
}

void TaskWoodPlankTextureParameters::onPlankDepthChanged(double val)
{
    if (auto plank = getObject<PartDesign::WoodPlankTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        plank->PlankDepth.setValue(val);
        plank->recomputeFeature();
        hideOnError();
    }
}

void TaskWoodPlankTextureParameters::onGapWidthChanged(double val)
{
    if (auto plank = getObject<PartDesign::WoodPlankTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        plank->GapWidth.setValue(val);
        plank->recomputeFeature();
        hideOnError();
    }
}

void TaskWoodPlankTextureParameters::onGapDepthChanged(double val)
{
    if (auto plank = getObject<PartDesign::WoodPlankTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        plank->GapDepth.setValue(val);
        plank->recomputeFeature();
        hideOnError();
    }
}

void TaskWoodPlankTextureParameters::onOrientationChanged(int val)
{
    if (auto plank = getObject<PartDesign::WoodPlankTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        plank->Orientation.setValue(val);
        plank->recomputeFeature();
        hideOnError();
    }
}

void TaskWoodPlankTextureParameters::onEndOffsetChanged(int val)
{
    if (auto plank = getObject<PartDesign::WoodPlankTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        plank->EndOffset.setValue(val);
        plank->recomputeFeature();
        hideOnError();
    }
}

void TaskWoodPlankTextureParameters::onScaleChanged(int val)
{
    if (auto plank = getObject<PartDesign::WoodPlankTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        plank->Scale.setValue(val);
        plank->recomputeFeature();
        hideOnError();
    }
}

void TaskWoodPlankTextureParameters::changeEvent(QEvent* e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(proxy);
    }
}

void TaskWoodPlankTextureParameters::apply()
{
    ui->plankLength->apply();
    ui->plankHeight->apply();
    ui->plankDepth->apply();
    ui->gapWidth->apply();
    ui->gapDepth->apply();
    ui->endOffset->apply();
}

//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgWoodPlankTextureParameters::TaskDlgWoodPlankTextureParameters(ViewProviderWoodPlankTexture* DressUpView)
    : TaskDlgDressUpParameters(DressUpView)
{
    parameter = new TaskWoodPlankTextureParameters(DressUpView);

    Content.push_back(parameter);
    Content.push_back(preview);
}

TaskDlgWoodPlankTextureParameters::~TaskDlgWoodPlankTextureParameters() = default;

bool TaskDlgWoodPlankTextureParameters::accept()
{
    auto obj = getObject();
    if (!obj->isError()) {
        getViewObject()->showPreviousFeature(false);
    }

    parameter->apply();

    return TaskDlgDressUpParameters::accept();
}

#include "moc_TaskWoodPlankTextureParameters.cpp"
