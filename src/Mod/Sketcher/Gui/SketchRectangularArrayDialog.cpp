// SPDX-License-Identifier: LGPL-2.1-or-later

/***************************************************************************
 *   Copyright (c) 2015 Abdullah Tahiri <abdullah.tahiri.yo@gmail.com>     *
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

#include <QDialog>


#include <Gui/MainWindow.h>

#include "SketchRectangularArrayDialog.h"
#include "ui_SketchRectangularArrayDialog.h"


using namespace SketcherGui;

SketchRectangularArrayDialog::SketchRectangularArrayDialog()
    : QDialog(Gui::getMainWindow())
    , ui(new Ui_SketchRectangularArrayDialog)
{
    ui->setupUi(this);

    ui->RowsQuantitySpinBox->onRestore();
    ui->ColsQuantitySpinBox->onRestore();
    ui->ConstraintSeparationCheckBox->onRestore();
    ui->EqualVerticalHorizontalSpacingCheckBox->onRestore();
    ui->CloneCheckBox->onRestore();
    ui->IndependentXYCheckBox->onRestore();
    ui->SpacingYSpinBox->onRestore();
    ui->SymmetricXCheckBox->onRestore();
    ui->SymmetricYCheckBox->onRestore();

    connect(
        ui->IndependentXYCheckBox,
        &QCheckBox::toggled,
        this,
        &SketchRectangularArrayDialog::onIndependentXYToggled
    );

    updateValues();
}

SketchRectangularArrayDialog::~SketchRectangularArrayDialog()
{}

void SketchRectangularArrayDialog::accept()
{
    ui->RowsQuantitySpinBox->onSave();
    ui->ColsQuantitySpinBox->onSave();
    ui->ConstraintSeparationCheckBox->onSave();
    ui->EqualVerticalHorizontalSpacingCheckBox->onSave();
    ui->CloneCheckBox->onSave();
    ui->IndependentXYCheckBox->onSave();
    ui->SpacingYSpinBox->onSave();
    ui->SymmetricXCheckBox->onSave();
    ui->SymmetricYCheckBox->onSave();

    updateValues();

    QDialog::accept();
}

void SketchRectangularArrayDialog::updateValues()
{
    Rows = ui->RowsQuantitySpinBox->value();
    Cols = ui->ColsQuantitySpinBox->value();
    ConstraintSeparation = ui->ConstraintSeparationCheckBox->isChecked();
    EqualVerticalHorizontalSpacing = ui->EqualVerticalHorizontalSpacingCheckBox->isChecked();
    Clone = ui->CloneCheckBox->isChecked();
    IndependentXYSpacing = ui->IndependentXYCheckBox->isChecked();
    SpacingX = 10.0;  // Will be set from drag distance
    SpacingY = ui->SpacingYSpinBox->value();
    SymmetricX = ui->SymmetricXCheckBox->isChecked();
    SymmetricY = ui->SymmetricYCheckBox->isChecked();
}

void SketchRectangularArrayDialog::onIndependentXYToggled(bool checked)
{
    ui->SpacingYSpinBox->setEnabled(checked);
    ui->spacingYLabel->setEnabled(checked);
}

#include "moc_SketchRectangularArrayDialog.cpp"
