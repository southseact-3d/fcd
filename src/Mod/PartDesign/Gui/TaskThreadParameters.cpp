// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten3D contributors                            *
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

#include <QMessageBox>

#include <App/Document.h>
#include <App/DocumentObject.h>
#include <Gui/Selection/Selection.h>
#include <Gui/Tools.h>
#include <Gui/ViewProvider.h>
#include <Mod/PartDesign/App/FeatureThread.h>
#include <Mod/PartDesign/App/FeatureHole.h>

#include "ui_TaskThreadParameters.h"
#include "TaskThreadParameters.h"


using namespace PartDesignGui;
using namespace Gui;

/* TRANSLATOR PartDesignGui::TaskThreadParameters */

TaskThreadParameters::TaskThreadParameters(ViewProviderDressUp* DressUpView, QWidget* parent)
    : TaskDressUpParameters(DressUpView, false, true, parent)
    , ui(new Ui_TaskThreadParameters)
{
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    this->groupLayout()->addWidget(proxy);

    PartDesign::Thread* pcThread = DressUpView->getObject<PartDesign::Thread>();

    const int typeIdx = pcThread->ThreadType.getValue();
    ui->threadType->setCurrentIndex(typeIdx);

    const int sizeIdx = pcThread->ThreadSize.getValue();
    ui->threadSize->setCurrentIndex(sizeIdx);

    ui->threadDirection->setCurrentIndex(pcThread->ThreadDirection.getValue());
    ui->threadMode->setCurrentIndex(pcThread->ThreadMode.getValue());

    ui->threadLength->setUnit(Base::Unit::Length);
    ui->threadLength->setMinimum(0);
    ui->threadLength->setValue(pcThread->ThreadLength.getValue());
    ui->threadLength->bind(pcThread->ThreadLength);

    ui->threadPitch->setUnit(Base::Unit::Length);
    ui->threadPitch->setMinimum(0);
    ui->threadPitch->setValue(pcThread->ThreadPitch.getValue());
    ui->threadPitch->bind(pcThread->ThreadPitch);

    ui->threadDepth->setUnit(Base::Unit::Length);
    ui->threadDepth->setMinimum(0);
    ui->threadDepth->setValue(pcThread->ThreadDepth.getValue());
    ui->threadDepth->bind(pcThread->ThreadDepth);

    ui->taperAngle->setUnit(Base::Unit::Angle);
    ui->taperAngle->setMinimum(0);
    ui->taperAngle->setMaximum(90);
    ui->taperAngle->setValue(pcThread->TaperAngle.getValue());
    ui->taperAngle->bind(pcThread->TaperAngle);

    ui->checkBoxUseCustomClearance->setChecked(pcThread->UseCustomClearance.getValue());
    ui->customClearance->setUnit(Base::Unit::Length);
    ui->customClearance->setMinimum(0);
    ui->customClearance->setValue(pcThread->CustomClearance.getValue());
    ui->customClearance->bind(pcThread->CustomClearance);
    ui->customClearance->setEnabled(pcThread->UseCustomClearance.getValue());

    ui->threadDepthType->setCurrentIndex(pcThread->ThreadDepthType.getValue());

    std::vector<std::string> strings = pcThread->Base.getSubValues();
    for (const auto& string : strings) {
        ui->listWidgetReferences->addItem(QString::fromStdString(string));
    }

    QMetaObject::connectSlotsByName(this);

    // clang-format off
    connect(ui->threadType, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &TaskThreadParameters::onThreadTypeChanged);
    connect(ui->threadSize, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &TaskThreadParameters::onThreadSizeChanged);
    connect(ui->threadDirection, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &TaskThreadParameters::onThreadDirectionChanged);
    connect(ui->threadMode, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &TaskThreadParameters::onThreadModeChanged);
    connect(ui->threadLength, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
            this, &TaskThreadParameters::onThreadLengthChanged);
    connect(ui->threadPitch, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
            this, &TaskThreadParameters::onThreadPitchChanged);
    connect(ui->threadDepth, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
            this, &TaskThreadParameters::onThreadDepthChanged);
    connect(ui->checkBoxUseCustomClearance, &QCheckBox::toggled,
            this, &TaskThreadParameters::onUseCustomClearanceChanged);
    connect(ui->customClearance, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
            this, &TaskThreadParameters::onCustomClearanceChanged);
    connect(ui->taperAngle, qOverload<double>(&Gui::QuantitySpinBox::valueChanged),
            this, &TaskThreadParameters::onTaperAngleChanged);
    connect(ui->buttonRefSel, &QToolButton::toggled,
            this, &TaskThreadParameters::onButtonRefSel);
    connect(ui->threadDepthType, qOverload<int>(&QComboBox::currentIndexChanged),
            [this](int index) {
                if (auto thread = getObject<PartDesign::Thread>()) {
                    thread->ThreadDepthType.setValue(index);
                    thread->recomputeFeature();
                    hideOnError();
                }
            });
    // clang-format on

    createDeleteAction(ui->listWidgetReferences);
    connect(deleteAction, &QAction::triggered,
            this, &TaskThreadParameters::onRefDeleted);

    connect(ui->listWidgetReferences, &QListWidget::currentItemChanged,
            this, &TaskThreadParameters::setSelection);
    connect(ui->listWidgetReferences, &QListWidget::itemClicked,
            this, &TaskThreadParameters::setSelection);
    connect(ui->listWidgetReferences, &QListWidget::itemDoubleClicked,
            this, &TaskThreadParameters::doubleClicked);

    if (strings.size() == 0) {
        setSelectionMode(refSel);
    }
    else {
        hideOnError();
    }
}

TaskThreadParameters::~TaskThreadParameters()
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

void TaskThreadParameters::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type == Gui::SelectionChanges::AddSelection) {
        if (selectionMode == refSel) {
            referenceSelected(msg, ui->listWidgetReferences);
        }
    }
}

void TaskThreadParameters::setButtons(const selectionModes mode)
{
    ui->buttonRefSel->setChecked(mode == refSel);
    ui->buttonRefSel->setText(mode == refSel ? stopSelectionLabel() : startSelectionLabel());
}

void TaskThreadParameters::onRefDeleted()
{
    TaskDressUpParameters::deleteRef(ui->listWidgetReferences);
}

void TaskThreadParameters::onThreadTypeChanged(int index)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        thread->ThreadType.setValue(index);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::onThreadSizeChanged(int index)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        setupTransaction();
        thread->ThreadSize.setValue(index);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::onThreadDirectionChanged(int index)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        setupTransaction();
        thread->ThreadDirection.setValue(index);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::onThreadModeChanged(int index)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        setupTransaction();
        thread->ThreadMode.setValue(index);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::onThreadLengthChanged(double len)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        setupTransaction();
        thread->ThreadLength.setValue(len);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::onThreadPitchChanged(double pitch)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        setupTransaction();
        thread->ThreadPitch.setValue(pitch);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::onThreadDepthChanged(double depth)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        setupTransaction();
        thread->ThreadDepth.setValue(depth);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::onUseCustomClearanceChanged(bool checked)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        thread->UseCustomClearance.setValue(checked);
        ui->customClearance->setEnabled(checked);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::onCustomClearanceChanged(double clearance)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        setupTransaction();
        thread->CustomClearance.setValue(clearance);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::onTaperAngleChanged(double angle)
{
    if (auto thread = getObject<PartDesign::Thread>()) {
        setSelectionMode(none);
        setupTransaction();
        thread->TaperAngle.setValue(angle);
        thread->recomputeFeature();
        hideOnError();
    }
}

void TaskThreadParameters::changeEvent(QEvent* e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(proxy);
    }
}

void TaskThreadParameters::apply()
{
    auto thread = getObject<PartDesign::Thread>();

    ui->threadLength->apply();
    ui->threadPitch->apply();
    ui->threadDepth->apply();
    ui->customClearance->apply();
    ui->taperAngle->apply();

    if (ui->listWidgetReferences->count() == 0) {
        Base::Console().warning(tr("Empty thread created!\n").toStdString().c_str());
    }
}


//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

TaskDlgThreadParameters::TaskDlgThreadParameters(ViewProviderThread* DressUpView)
    : TaskDlgDressUpParameters(DressUpView)
{
    parameter = new TaskThreadParameters(DressUpView);

    Content.push_back(parameter);
    Content.push_back(preview);
}

TaskDlgThreadParameters::~TaskDlgThreadParameters() = default;

bool TaskDlgThreadParameters::accept()
{
    auto obj = getObject();
    if (!obj->isError()) {
        getViewObject()->showPreviousFeature(false);
    }

    parameter->apply();

    return TaskDlgDressUpParameters::accept();
}

#include "moc_TaskThreadParameters.cpp"
