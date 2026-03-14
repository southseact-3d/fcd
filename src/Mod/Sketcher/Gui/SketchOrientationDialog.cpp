#include <QDialog>


#include <Gui/BitmapFactory.h>
#include <Gui/MainWindow.h>

#include "SketchOrientationDialog.h"
#include "ui_SketchOrientationDialog.h"


using namespace SketcherGui;

SketchOrientationDialog::SketchOrientationDialog()
    : QDialog(Gui::getMainWindow())
    , ui(new Ui_SketchOrientationDialog)
{
    DirType = 0;
    ui->setupUi(this);
    onPreview();

    connect(ui->Reverse_checkBox, &QCheckBox::clicked, this, &SketchOrientationDialog::onPreview);
    connect(ui->XY_radioButton, &QRadioButton::clicked, this, &SketchOrientationDialog::onPreview);
    connect(ui->XZ_radioButton, &QRadioButton::clicked, this, &SketchOrientationDialog::onPreview);
    connect(ui->YZ_radioButton, &QRadioButton::clicked, this, &SketchOrientationDialog::onPreview);
}

SketchOrientationDialog::~SketchOrientationDialog()
{}

void SketchOrientationDialog::accept()
{
    double offset = ui->Offset_doubleSpinBox->value().getValue();
    bool reverse = ui->Reverse_checkBox->isChecked();
    if (ui->XY_radioButton->isChecked()) {
        if (reverse) {
            Pos = Base::Placement(Base::Vector3d(0, 0, offset), Base::Rotation(1.0, 0.0, 0.0, 0.0));
            DirType = 1;
        }
        else {
            Pos = Base::Placement(Base::Vector3d(0, 0, offset), Base::Rotation());
            DirType = 0;
        }
    }
    else if (ui->XZ_radioButton->isChecked()) {
        if (reverse) {
            Pos = Base::Placement(Base::Vector3d(0, offset, 0), Base::Rotation(0.0, 1.0, 1.0, 0.0));
            DirType = 3;
        }
        else {
            Pos = Base::Placement(Base::Vector3d(0, offset, 0), Base::Rotation(1.0, 0.0, 0.0, 1.0));
            DirType = 2;
        }
    }
    else if (ui->YZ_radioButton->isChecked()) {
        if (reverse) {
            Pos = Base::Placement(Base::Vector3d(offset, 0, 0), Base::Rotation(-1.0, 1.0, 1.0, -1.0));
            DirType = 5;
        }
        else {
            Pos = Base::Placement(Base::Vector3d(offset, 0, 0), Base::Rotation(1.0, 1.0, 1.0, 1.0));
            DirType = 4;
        }
    }

    QDialog::accept();
}

void SketchOrientationDialog::onPreview()
{
    std::string icon;
    bool reverse = ui->Reverse_checkBox->isChecked();
    if (ui->XY_radioButton->isChecked()) {
        if (reverse) {
            icon = "view-bottom";
        }
        else {
            icon = "view-top";
        }
    }
    else if (ui->XZ_radioButton->isChecked()) {
        if (reverse) {
            icon = "view-rear";
        }
        else {
            icon = "view-front";
        }
    }
    else if (ui->YZ_radioButton->isChecked()) {
        if (reverse) {
            icon = "view-left";
        }
        else {
            icon = "view-right";
        }
    }

    ui->previewLabel->setPixmap(
        Gui::BitmapFactory().pixmapFromSvg(icon.c_str(), ui->previewLabel->size())
    );
}

#include "moc_SketchOrientationDialog.cpp"
