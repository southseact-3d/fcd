#include <QDialog>


#include <Gui/MainWindow.h>

#include "SketcherRegularPolygonDialog.h"
#include "ui_SketcherRegularPolygonDialog.h"


using namespace SketcherGui;

SketcherRegularPolygonDialog::SketcherRegularPolygonDialog()
    : QDialog(Gui::getMainWindow())
    , ui(new Ui_SketcherRegularPolygonDialog)
{
    ui->setupUi(this);

    ui->sidesQuantitySpinBox->onRestore();

    updateValues();
}

SketcherRegularPolygonDialog::~SketcherRegularPolygonDialog()
{}

void SketcherRegularPolygonDialog::accept()
{
    ui->sidesQuantitySpinBox->onSave();

    updateValues();

    QDialog::accept();
}

void SketcherRegularPolygonDialog::updateValues()
{
    sides = ui->sidesQuantitySpinBox->value();
}

#include "moc_SketcherRegularPolygonDialog.cpp"
