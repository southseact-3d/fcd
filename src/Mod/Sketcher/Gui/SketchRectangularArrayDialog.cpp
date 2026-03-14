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
}

#include "moc_SketchRectangularArrayDialog.cpp"
