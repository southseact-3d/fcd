#include "DlgEvaluateSettings.h"
#include "ui_DlgEvaluateSettings.h"


using namespace MeshGui;

/* TRANSLATOR MeshGui::DlgEvaluateSettings */

DlgEvaluateSettings::DlgEvaluateSettings(QWidget* parent, Qt::WindowFlags fl)
    : QDialog(parent, fl)
    , ui(new Ui_DlgEvaluateSettings)
{
    ui->setupUi(this);
}

DlgEvaluateSettings::~DlgEvaluateSettings()
{
    delete ui;
}

void DlgEvaluateSettings::setNonmanifoldPointsChecked(bool on)
{
    ui->checkNonmanifoldPoints->setChecked(on);
}

bool DlgEvaluateSettings::isNonmanifoldPointsChecked() const
{
    return ui->checkNonmanifoldPoints->isChecked();
}

void DlgEvaluateSettings::setFoldsChecked(bool on)
{
    ui->checkFolds->setChecked(on);
}

bool DlgEvaluateSettings::isFoldsChecked() const
{
    return ui->checkFolds->isChecked();
}

void DlgEvaluateSettings::setDegeneratedFacetsChecked(bool on)
{
    ui->checkDegenerated->setChecked(on);
}

bool DlgEvaluateSettings::isDegeneratedFacetsChecked() const
{
    return ui->checkDegenerated->isChecked();
}

#include "moc_DlgEvaluateSettings.cpp"
