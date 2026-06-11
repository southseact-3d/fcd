// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2024 Tungsten CAD contributors                           *
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

#include <App/Document.h>
#include <App/DocumentObject.h>
#include <App/Origin.h>
#include <Base/Console.h>
#include <Base/Converter.h>
#include <Base/Tools.h>
#include <Gui/Application.h>
#include <Gui/CommandT.h>
#include <Gui/Document.h>
#include <Gui/Selection/Selection.h>
#include <Gui/WaitCursor.h>
#include <Mod/Part/App/Tools.h>
#include <Gui/ViewProviderCoordinateSystem.h>
#include <Mod/PartDesign/App/Body.h>
#include <Mod/PartDesign/App/FeatureCoil.h>

#include "ReferenceSelection.h"
#include "ui_TaskCoilParameters.h"
#include "TaskCoilParameters.h"

using namespace PartDesignGui;
using PartDesign::CoilMode;
using PartDesign::CoilSectionShape;
using PartDesign::CoilSectionPosition;
using namespace Gui;


/* TRANSLATOR PartDesignGui::TaskCoilParameters */

TaskCoilParameters::TaskCoilParameters(PartDesignGui::ViewProviderCoil* CoilView, QWidget* parent)
    : TaskSketchBasedParameters(CoilView, parent, "PartDesign_AdditiveCoil", tr("Coil Parameters"))
    , ui(new Ui_TaskCoilParameters)
{
    proxy = new QWidget(this);
    ui->setupUi(proxy);
    this->groupLayout()->addWidget(proxy);

    initializeCoil();

    assignProperties();
    setValuesFromProperties();

    updateUI();

    bindProperties();

    connectSlots();
    setFocus();
}

void TaskCoilParameters::initializeCoil()
{
    // no initialization needed for coil (unlike helix which proposes parameters)
}

void TaskCoilParameters::assignProperties()
{
    auto coil = getObject<PartDesign::Coil>();
    propRadius = &(coil->Radius);
    propPitch = &(coil->Pitch);
    propHeight = &(coil->Height);
    propTurns = &(coil->Turns);
    propAngle = &(coil->Angle);
    propSectionSize = &(coil->SectionSize);
    propSectionShape = &(coil->SectionShape);
    propSectionPosition = &(coil->SectionPosition);
    propLeftHanded = &(coil->LeftHanded);
    propReversed = &(coil->Reversed);
    propReferenceAxis = &(coil->ReferenceAxis);
    propMode = &(coil->Mode);
}

void TaskCoilParameters::setValuesFromProperties()
{
    ui->radius->setValue(propRadius->getValue());
    ui->pitch->setValue(propPitch->getValue());
    ui->height->setValue(propHeight->getValue());
    ui->turns->setValue(propTurns->getValue());
    ui->coneAngle->setValue(propAngle->getValue());
    ui->coneAngle->setMinimum(propAngle->getMinimum());
    ui->coneAngle->setMaximum(propAngle->getMaximum());
    ui->sectionSize->setValue(propSectionSize->getValue());
    ui->sectionShape->setCurrentIndex(propSectionShape->getValue());
    ui->sectionPosition->setCurrentIndex(propSectionPosition->getValue());
    ui->checkBoxLeftHanded->setChecked(propLeftHanded->getValue());
    ui->checkBoxReversed->setChecked(propReversed->getValue());
    ui->inputMode->setCurrentIndex(propMode->getValue());
}

void TaskCoilParameters::bindProperties()
{
    auto coil = getObject<PartDesign::Coil>();
    ui->radius->bind(coil->Radius);
    ui->pitch->bind(coil->Pitch);
    ui->height->bind(coil->Height);
    ui->turns->bind(coil->Turns);
    ui->coneAngle->bind(coil->Angle);
    ui->sectionSize->bind(coil->SectionSize);
}

void TaskCoilParameters::connectSlots()
{
    QMetaObject::connectSlotsByName(this);

    // clang-format off
    connect(ui->radius, qOverload<double>(&QuantitySpinBox::valueChanged),
            this, &TaskCoilParameters::onRadiusChanged);
    connect(ui->pitch, qOverload<double>(&QuantitySpinBox::valueChanged),
            this, &TaskCoilParameters::onPitchChanged);
    connect(ui->height, qOverload<double>(&QuantitySpinBox::valueChanged),
            this, &TaskCoilParameters::onHeightChanged);
    connect(ui->turns, qOverload<double>(&QuantitySpinBox::valueChanged),
            this, &TaskCoilParameters::onTurnsChanged);
    connect(ui->coneAngle, qOverload<double>(&QuantitySpinBox::valueChanged),
            this, &TaskCoilParameters::onAngleChanged);
    connect(ui->sectionSize, qOverload<double>(&QuantitySpinBox::valueChanged),
            this, &TaskCoilParameters::onSectionSizeChanged);
    connect(ui->sectionShape, qOverload<int>(&QComboBox::activated),
            this, &TaskCoilParameters::onSectionShapeChanged);
    connect(ui->sectionPosition, qOverload<int>(&QComboBox::activated),
            this, &TaskCoilParameters::onSectionPositionChanged);
    connect(ui->axis, qOverload<int>(&QComboBox::activated),
            this, &TaskCoilParameters::onAxisChanged);
    connect(ui->checkBoxLeftHanded, &QCheckBox::toggled,
            this, &TaskCoilParameters::onLeftHandedChanged);
    connect(ui->checkBoxReversed, &QCheckBox::toggled,
            this, &TaskCoilParameters::onReversedChanged);
    connect(ui->checkBoxUpdateView, &QCheckBox::toggled,
            this, &TaskCoilParameters::onUpdateView);
    connect(ui->inputMode, qOverload<int>(&QComboBox::activated),
            this, &TaskCoilParameters::onModeChanged);
    // clang-format on
}

void TaskCoilParameters::fillAxisCombo(bool forceRefill)
{
    Base::StateLocker lock(getUpdateBlockRef(), true);

    if (axesInList.empty()) {
        forceRefill = true;
    }

    if (forceRefill) {
        ui->axis->clear();
        this->axesInList.clear();

        addPartAxes();

        addAxisToCombo(nullptr, std::string(), tr("Select reference..."));
    }

    int indexOfCurrent = addCurrentLink();
    if (indexOfCurrent != -1) {
        ui->axis->setCurrentIndex(indexOfCurrent);
    }
}

void TaskCoilParameters::addPartAxes()
{
    auto coil = getObject<PartDesign::Coil>();
    if (PartDesign::Body* body = PartDesign::Body::findBodyOf(coil)) {
        try {
            App::Origin* orig = body->getOrigin();
            addAxisToCombo(orig->getX(), "", tr("Base X-axis"));
            addAxisToCombo(orig->getY(), "", tr("Base Y-axis"));
            addAxisToCombo(orig->getZ(), "", tr("Base Z-axis"));
        }
        catch (const Base::Exception& ex) {
            ex.reportException();
        }
    }
}

int TaskCoilParameters::addCurrentLink()
{
    int indexOfCurrent = -1;
    App::DocumentObject* ax = propReferenceAxis->getValue();
    const std::vector<std::string>& subList = propReferenceAxis->getSubValues();
    for (size_t i = 0; i < axesInList.size(); i++) {
        if (ax == axesInList[i]->getValue() && subList == axesInList[i]->getSubValues()) {
            indexOfCurrent = i;
            break;
        }
    }

    if (indexOfCurrent == -1 && ax) {
        assert(subList.size() <= 1);
        std::string sub;
        if (!subList.empty()) {
            sub = subList[0];
        }
        addAxisToCombo(ax, sub, getRefStr(ax, subList));
        indexOfCurrent = axesInList.size() - 1;
    }

    return indexOfCurrent;
}

void TaskCoilParameters::addAxisToCombo(
    App::DocumentObject* linkObj,
    std::string linkSubname,
    QString itemText
)
{
    this->ui->axis->addItem(itemText);
    this->axesInList.emplace_back(new App::PropertyLinkSub);
    App::PropertyLinkSub& lnk = *(axesInList.back());
    lnk.setValue(linkObj, std::vector<std::string>(1, linkSubname));
}

void TaskCoilParameters::updateUI()
{
    fillAxisCombo();
    assignToolTipsFromPropertyDocs();
    adaptVisibilityToMode();
}

void TaskCoilParameters::adaptVisibilityToMode()
{
    CoilMode mode = static_cast<CoilMode>(propMode->getValue());

    bool isPitchVisible = false;
    bool isHeightVisible = false;
    bool isTurnsVisible = false;

    if (mode == CoilMode::revolutionHeight) {
        isTurnsVisible = true;
        isHeightVisible = true;
    }
    else if (mode == CoilMode::revolutionPitch) {
        isTurnsVisible = true;
        isPitchVisible = true;
    }
    else if (mode == CoilMode::heightPitch) {
        isHeightVisible = true;
        isPitchVisible = true;
    }
    else if (mode == CoilMode::spiral) {
        isTurnsVisible = true;
        isPitchVisible = true;
    }

    ui->pitch->setVisible(isPitchVisible);
    ui->labelPitch->setVisible(isPitchVisible);

    ui->height->setVisible(isHeightVisible);
    ui->labelHeight->setVisible(isHeightVisible);

    ui->turns->setVisible(isTurnsVisible);
    ui->labelTurns->setVisible(isTurnsVisible);
}

void TaskCoilParameters::assignToolTipsFromPropertyDocs()
{
    auto coil = getObject<PartDesign::Coil>();
    const char* propCategory = "App::Property";
    QString toolTip;

    toolTip = QApplication::translate(propCategory, coil->ReferenceAxis.getDocumentation());
    ui->axis->setToolTip(toolTip);
    ui->labelAxis->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->Mode.getDocumentation());
    ui->inputMode->setToolTip(toolTip);
    ui->labelMode->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->Radius.getDocumentation());
    ui->radius->setToolTip(toolTip);
    ui->labelRadius->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->Pitch.getDocumentation());
    ui->pitch->setToolTip(toolTip);
    ui->labelPitch->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->Height.getDocumentation());
    ui->height->setToolTip(toolTip);
    ui->labelHeight->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->Turns.getDocumentation());
    ui->turns->setToolTip(toolTip);
    ui->labelTurns->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->Angle.getDocumentation());
    ui->coneAngle->setToolTip(toolTip);
    ui->labelAngle->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->SectionSize.getDocumentation());
    ui->sectionSize->setToolTip(toolTip);
    ui->labelSectionSize->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->SectionShape.getDocumentation());
    ui->sectionShape->setToolTip(toolTip);
    ui->labelSectionShape->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->SectionPosition.getDocumentation());
    ui->sectionPosition->setToolTip(toolTip);
    ui->labelSectionPosition->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->LeftHanded.getDocumentation());
    ui->checkBoxLeftHanded->setToolTip(toolTip);

    toolTip = QApplication::translate(propCategory, coil->Reversed.getDocumentation());
    ui->checkBoxReversed->setToolTip(toolTip);
}

void TaskCoilParameters::onSelectionChanged(const Gui::SelectionChanges& msg)
{
    if (msg.Type == Gui::SelectionChanges::AddSelection) {
        std::vector<std::string> axis;
        App::DocumentObject* selObj {};
        if (getReferencedSelection(getObject(), msg, selObj, axis) && selObj) {
            exitSelectionMode();
            propReferenceAxis->setValue(selObj, axis);
            recomputeFeature();
            updateUI();
        }
    }
}

void TaskCoilParameters::onRadiusChanged(double len)
{
    if (getObject()) {
        propRadius->setValue(len);
        recomputeFeature();
        updateUI();
    }
}

void TaskCoilParameters::onPitchChanged(double len)
{
    if (getObject()) {
        propPitch->setValue(len);
        recomputeFeature();
        updateUI();
    }
}

void TaskCoilParameters::onHeightChanged(double len)
{
    if (getObject()) {
        propHeight->setValue(len);
        recomputeFeature();
        updateUI();
    }
}

void TaskCoilParameters::onTurnsChanged(double len)
{
    if (getObject()) {
        propTurns->setValue(len);
        recomputeFeature();
        updateUI();
    }
}

void TaskCoilParameters::onAngleChanged(double len)
{
    if (getObject()) {
        propAngle->setValue(len);
        recomputeFeature();
        updateUI();
    }
}

void TaskCoilParameters::onSectionSizeChanged(double len)
{
    if (getObject()) {
        propSectionSize->setValue(len);
        recomputeFeature();
        updateUI();
    }
}

void TaskCoilParameters::onSectionShapeChanged(int index)
{
    if (getObject()) {
        propSectionShape->setValue(index);
        recomputeFeature();
        updateUI();
    }
}

void TaskCoilParameters::onSectionPositionChanged(int index)
{
    if (getObject()) {
        propSectionPosition->setValue(index);
        recomputeFeature();
        updateUI();
    }
}

void TaskCoilParameters::onAxisChanged(int num)
{
    if (axesInList.empty()) {
        return;
    }

    App::PropertyLinkSub& lnk = *(axesInList[num]);
    if (!lnk.getValue()) {
        // enter reference selection mode
        TaskSketchBasedParameters::onSelectReference(
            AllowSelection::EDGE | AllowSelection::PLANAR | AllowSelection::CIRCLE
        );
        return;
    }
    else {
        if (!getObject()->getDocument()->isIn(lnk.getValue())) {
            Base::Console().error("Object was deleted\n");
            return;
        }
        propReferenceAxis->Paste(lnk);
        exitSelectionMode();
    }

    try {
        recomputeFeature();
    }
    catch (const Base::Exception& e) {
        e.reportException();
    }
}

void TaskCoilParameters::onModeChanged(int index)
{
    propMode->setValue(index);

    ui->radius->setValue(propRadius->getValue());
    ui->pitch->setValue(propPitch->getValue());
    ui->height->setValue(propHeight->getValue());
    ui->turns->setValue(propTurns->getValue());
    ui->coneAngle->setValue(propAngle->getValue());

    recomputeFeature();
    updateUI();
}

void TaskCoilParameters::onLeftHandedChanged(bool on)
{
    if (getObject()) {
        propLeftHanded->setValue(on);
        recomputeFeature();
        updateUI();
    }
}

void TaskCoilParameters::onReversedChanged(bool on)
{
    if (getObject()) {
        propReversed->setValue(on);
        recomputeFeature();
        updateUI();
    }
}


TaskCoilParameters::~TaskCoilParameters() = default;

void TaskCoilParameters::changeEvent(QEvent* e)
{
    TaskBox::changeEvent(e);
    if (e->type() == QEvent::LanguageChange) {
        int axis = ui->axis->currentIndex();
        int mode = ui->inputMode->currentIndex();
        ui->retranslateUi(proxy);
        assignToolTipsFromPropertyDocs();

        fillAxisCombo(true);

        if (axis < ui->axis->count()) {
            ui->axis->setCurrentIndex(axis);
        }
        ui->inputMode->setCurrentIndex(mode);
    }
}

void TaskCoilParameters::getReferenceAxis(App::DocumentObject*& obj, std::vector<std::string>& sub) const
{
    if (axesInList.empty()) {
        throw Base::RuntimeError("Not initialized!");
    }

    int num = ui->axis->currentIndex();
    const App::PropertyLinkSub& lnk = *(axesInList.at(num));
    if (!lnk.getValue()) {
        throw Base::RuntimeError("Still in reference selection mode; reference was not selected yet");
    }
    else {
        if (!getObject()->getDocument()->isIn(lnk.getValue())) {
            throw Base::RuntimeError("Object was deleted");
        }

        obj = lnk.getValue();
        sub = lnk.getSubValues();
    }
}

void TaskCoilParameters::startReferenceSelection(App::DocumentObject* profile, App::DocumentObject* base)
{
    TaskSketchBasedParameters::startReferenceSelection(profile, base);
}

void TaskCoilParameters::apply()
{
    std::vector<std::string> sub;
    App::DocumentObject* obj {};
    getReferenceAxis(obj, sub);
    std::string axis = buildLinkSingleSubPythonStr(obj, sub);
    auto tobj = getObject();
    FCMD_OBJ_CMD(tobj, "ReferenceAxis = " << axis);
    FCMD_OBJ_CMD(tobj, "Mode = " << propMode->getValue());
    FCMD_OBJ_CMD(tobj, "Radius = " << propRadius->getValue());
    FCMD_OBJ_CMD(tobj, "Pitch = " << propPitch->getValue());
    FCMD_OBJ_CMD(tobj, "Height = " << propHeight->getValue());
    FCMD_OBJ_CMD(tobj, "Turns = " << propTurns->getValue());
    FCMD_OBJ_CMD(tobj, "Angle = " << propAngle->getValue());
    FCMD_OBJ_CMD(tobj, "SectionSize = " << propSectionSize->getValue());
    FCMD_OBJ_CMD(tobj, "SectionShape = " << propSectionShape->getValue());
    FCMD_OBJ_CMD(tobj, "SectionPosition = " << propSectionPosition->getValue());
    FCMD_OBJ_CMD(tobj, "LeftHanded = " << (propLeftHanded->getValue() ? 1 : 0));
    FCMD_OBJ_CMD(tobj, "Reversed = " << (propReversed->getValue() ? 1 : 0));
}


//**************************************************************************
// TaskDialog
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
TaskDlgCoilParameters::TaskDlgCoilParameters(ViewProviderCoil* CoilView)
    : TaskDlgSketchBasedParameters(CoilView)
{
    assert(CoilView);
    Content.push_back(new TaskCoilParameters(CoilView));
    Content.push_back(preview);
}


#include "moc_TaskCoilParameters.cpp"
