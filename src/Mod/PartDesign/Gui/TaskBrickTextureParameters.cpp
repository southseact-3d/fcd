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
#include <Mod/PartDesign/App/FeatureBrickTexture.h>

#include "ui_TaskBrickTextureParameters.h"
#include "TaskBrickTextureParameters.h"


using namespace PartDesignGui;
using namespace Gui;

TaskBrickTextureParameters::TaskBrickTextureParameters(ViewProviderDressUp* DressUpView, QWidget* parent)
    : TaskDressUpParameters(DressUpView, false, true, parent)
    , ui(new Ui_TaskBrickTextureParameters)
{
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    this->groupLayout()->addWidget(proxy);

    PartDesign::BrickTexture* pcBrick = DressUpView->getObject<PartDesign::BrickTexture>();

    ui->brickWidth->setUnit(Base::Unit::Length);
    ui->brickWidth->setValue(pcBrick->BrickWidth.getValue());
    ui->brickWidth->setMinimum(0.1);
    ui->brickWidth->bind(pcBrick->BrickWidth);

    ui->brickHeight->setUnit(Base::Unit::Length);
    ui->brickHeight->setValue(pcBrick->BrickHeight.getValue());
    ui->brickHeight->setMinimum(0.1);
    ui->brickHeight->bind(pcBrick->BrickHeight);

    ui->brickDepth->setUnit(Base::Unit::Length);
    ui->brickDepth->setValue(pcBrick->BrickDepth.getValue());
    ui->brickDepth->setMinimum(0.0);
    ui->brickDepth->bind(pcBrick->BrickDepth);

    ui->mortarThickness->setUnit(Base::Unit::Length);
    ui->mortarThickness->setValue(pcBrick->MortarThickness.getValue());
    ui->mortarThickness->setMinimum(0.0);
    ui->mortarThickness->bind(pcBrick->MortarThickness);

    ui->mortarDepth->setUnit(Base::Unit::Length);
    ui->mortarDepth->setValue(pcBrick->MortarDepth.getValue());
    ui->mortarDepth->setMinimum(0.0);
    ui->mortarDepth->bind(pcBrick->MortarDepth);

    ui->rowOffset->setValue(pcBrick->RowOffset.getValue());
    ui->rowOffset->bind(pcBrick->RowOffset);

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
    ui->scale->setCurrentIndex(pcBrick->Scale.getValue());

    std::vector<std::string> strings = pcBrick->Base.getSubValues();
    for (const auto& string : strings) {
        ui->listWidgetReferences->addItem(QString::fromStdString(string));
    }

    QMetaObject::connectSlotsByName(this);

    // clang-format off
    connect(ui->brickWidth, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskBrickTextureParameters::onBrickWidthChanged);
    connect(ui->brickHeight, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskBrickTextureParameters::onBrickHeightChanged);
    connect(ui->brickDepth, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskBrickTextureParameters::onBrickDepthChanged);
    connect(ui->mortarThickness, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskBrickTextureParameters::onMortarThicknessChanged);
    connect(ui->mortarDepth, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
        this, &TaskBrickTextureParameters::onMortarDepthChanged);
    connect(ui->rowOffset, qOverload<int>(&Gui::IntSpinBox::valueChanged),
        this, &TaskBrickTextureParameters::onRowOffsetChanged);
    connect(ui->scale, qOverload<int>(&QComboBox::currentIndexChanged),
        this, &TaskBrickTextureParameters::onScaleChanged);

    connect(ui->buttonRefSel, &QToolButton::toggled,
        this, &TaskBrickTextureParameters::onButtonRefSel);

    createDeleteAction(ui->listWidgetReferences);
    connect(deleteAction, &QAction::triggered, this, &TaskBrickTextureParameters::onRefDeleted);

    connect(ui->listWidgetReferences, &QListWidget::currentItemChanged,
        this, &TaskBrickTextureParameters::setSelection);
    connect(ui->listWidgetReferences, &QListWidget::itemClicked,
        this, &TaskBrickTextureParameters::setSelection);
    connect(ui->listWidgetReferences, &QListWidget::itemDoubleClicked,
        this, &TaskBrickTextureParameters::doubleClicked);
    // clang-format on

    if (strings.empty()) {
        setSelectionMode(refSel);
    }
    else {
        hideOnError();
    }
}

TaskBrickTextureParameters::~TaskBrickTextureParameters()
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

void TaskBrickTextureParameters::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type == Gui::SelectionChanges::AddSelection) {
        if (selectionMode == refSel) {
            referenceSelected(msg, ui->listWidgetReferences);
        }
    }
}

void TaskBrickTextureParameters::setButtons(const selectionModes mode)
{
    ui->buttonRefSel->setChecked(mode == refSel);
    ui->buttonRefSel->setText(mode == refSel ? stopSelectionLabel() : startSelectionLabel());
}

void TaskBrickTextureParameters::onRefDeleted()
{
    TaskDressUpParameters::deleteRef(ui->listWidgetReferences);
}

void TaskBrickTextureParameters::onBrickWidthChanged(double val)
{
    if (auto brick = getObject<PartDesign::BrickTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        brick->BrickWidth.setValue(val);
        brick->recomputeFeature();
        hideOnError();
    }
}

void TaskBrickTextureParameters::onBrickHeightChanged(double val)
{
    if (auto brick = getObject<PartDesign::BrickTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        brick->BrickHeight.setValue(val);
        brick->recomputeFeature();
        hideOnError();
    }
}

void TaskBrickTextureParameters::onBrickDepthChanged(double val)
{
    if (auto brick = getObject<PartDesign::BrickTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        brick->BrickDepth.setValue(val);
        brick->recomputeFeature();
        hideOnError();
    }
}

void TaskBrickTextureParameters::onMortarThicknessChanged(double val)
{
    if (auto brick = getObject<PartDesign::BrickTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        brick->MortarThickness.setValue(val);
        brick->recomputeFeature();
        hideOnError();
    }
}

void TaskBrickTextureParameters::onMortarDepthChanged(double val)
{
    if (auto brick = getObject<PartDesign::BrickTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        brick->MortarDepth.setValue(val);
        brick->recomputeFeature();
        hideOnError();
    }
}

void TaskBrickTextureParameters::onRowOffsetChanged(int val)
{
    if (auto brick = getObject<PartDesign::BrickTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        brick->RowOffset.setValue(val);
        brick->recomputeFeature();
        hideOnError();
    }
}

void TaskBrickTextureParameters::onScaleChanged(int val)
{
    if (auto brick = getObject<PartDesign::BrickTexture>()) {
        setSelectionMode(none);
        setupTransaction();
        brick->Scale.setValue(val);
        brick->recomputeFeature();
        hideOnError();
    }
}

void TaskBrickTextureParameters::changeEvent(QEvent* e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(proxy);
    }
}

void TaskBrickTextureParameters::apply()
{
    ui->brickWidth->apply();
    ui->brickHeight->apply();
    ui->brickDepth->apply();
    ui->mortarThickness->apply();
    ui->mortarDepth->apply();
    ui->rowOffset->apply();
}

//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgBrickTextureParameters::TaskDlgBrickTextureParameters(ViewProviderBrickTexture* DressUpView)
    : TaskDlgDressUpParameters(DressUpView)
{
    parameter = new TaskBrickTextureParameters(DressUpView);

    Content.push_back(parameter);
    Content.push_back(preview);
}

TaskDlgBrickTextureParameters::~TaskDlgBrickTextureParameters() = default;

bool TaskDlgBrickTextureParameters::accept()
{
    auto obj = getObject();
    if (!obj->isError()) {
        getViewObject()->showPreviousFeature(false);
    }

    parameter->apply();

    return TaskDlgDressUpParameters::accept();
}

#include "moc_TaskBrickTextureParameters.cpp"
